#include "ByteCode.hpp"
#include "Memory.hpp"
#include "SymbolTable.hpp"
#include "codegen/AstNodeIterator.hpp"
#include "CPType.hpp"
#include "util/PrintTools.hpp"
#include "util/log.hpp"

void _load_commamnd(uint32_t i, std::vector<Command> &commands) {
	//log_debug() << "loading value " << i << std::endl;
	commands.push_back(Command::Load);
	for (uint8_t j = 0; j < 4; j++) {
		commands.push_back(static_cast<Command>((i >> (j * 8)) & 0xff));
		//log_debug() << "pushed " << static_cast<uint32_t>(commands.back()) << std::endl;
	}
}

uint32_t _get_constant(uint32_t index, std::vector<Command> const &commands) {
	uint32_t r = 0;
	for (int j = 0; j < 4; j++) {
		r += commands[index + j] >> (j * 8);
	}
	return r;
}

util::Result<ByteCode, KError> ByteCode::create(const cg::AstNode &tree, cg::ParserContext &p_context) {
	try {
		auto code = ByteCode();
		code._context = &p_context;
		code._table = SymbolTable::create(tree);
		log_debug() << "loaded table " << code._table << std::endl;

		for (uint32_t i = 0; i < code._table.size(); i++) {
			auto exp = code._table[i].expression();
			log_debug() << "parsing exp " << exp << std::endl;
			code._line_indexes.push_back(code._commands.size());
			if (exp) {
				auto type = code._compile_exp(*exp).value();
				if (type.pointer_types.empty() || type.pointer_types.back() != CPType::Jump) {
					code._commands.push_back(Set);
					code._commands.push_back(Next);
				}

				if (auto value = code._eval_comptime(*exp)) {
					code._initial_values.push_back({i, value.value()});
				}
			} else {
				code._commands.push_back(Next);
			}

		}

		return code;
	} catch_kerror;
};

void ByteCode::execute() {
	auto memory = Memory();
	for (auto &[position, value] : _initial_values) {
		log_debug() << "init @" << position << " = " << value << std::endl;
		memory[position] = value;
	}

	uint32_t reg;
	//program counter
	uint32_t pc = 0;
	uint32_t cur_addr = 0;
	bool running = true;
	if (auto s = _table[cur_addr].statement()) {
		log_trace() << "executing line: (" << pc << ") " << _context->get_line(s->location()) << std::endl;
	}
	while (pc < _commands.size()) {
		auto command = _commands[pc];
		switch (command) {
			case Command::Next:
				cur_addr++;
				if (auto s = _table[cur_addr].statement()) {
		log_trace() << "executing line: (" << pc+1 << ") " << _context->get_line(s->location()) << std::endl;
				}
				//log_trace() << "line: " << cur_addr << std::endl;
				break;
			case Command::Load:
				reg = _get_constant(pc+1, _commands);
				pc += 4;
				log_trace() << "reg = " << reg << std::endl;
				break;
			case Command::Set:
				memory[cur_addr] = reg;
				log_trace() << "@" << cur_addr << " = reg(" << reg << ")" << std::endl;
				break;
			case Command::Deref:
				if (reg == 0) {
					pc = _commands.size(); // Manually stop
					log_trace() << "stopping" << std::endl;
				} else {
					auto &trace = log_trace() << "derefencing *" << reg;
					reg = memory[reg];
					trace << " -> " << reg << std::endl;
				}
				break;
			case Command::Inc:
				memory[reg]++;
				reg = memory[reg];
				log_trace() << "incrimented " << reg << std::endl;
				break;
			case Command::Dec:
				memory[reg]--;
				reg = memory[reg];
				log_trace() << "decrimented " << reg << std::endl;
				break;
			case Command::Jump:
				pc = _line_indexes[reg]-1;
				cur_addr = reg;
				log_trace() << "jumping to  " << reg << " ( pc = " << pc << ")" << std::endl;
				break;
			case Command::Tern:
				if (memory[cur_addr-1] == 0) {
					reg = memory[reg];
					log_trace() << "tern true: " << reg << std::endl;
				} else {
					log_trace() << "tern false: " << reg << std::endl;
				}
				break;
			case Command::Read:
				std::cin >> reg;
				log_trace() << "Read value: " << reg << std::endl;
				break;
			case Command::Write:
				std::cout << static_cast<int>(reg);
				log_trace() << "write reg " << reg << std::endl;
				reg = memory[reg];
				break;
		}
		pc++;
	}
}


std::ostream &ByteCode::print(std::ostream &os) const {
	size_t i = 0;
	os << "Initial values:" << std::endl;
	for (auto &[position, value] : _initial_values) {
		os << "mem[" << position << "] = " << value << std::endl;
	}
	os << "ByteCode:" << std::endl;
	while (i < _commands.size()) {
		os << i << ") ";
		os << _commands[i];
		if (_commands[i] == Command::Load) {
			os << " " << _get_constant(i+1, _commands);
			i += 5;
		} else {
			i++;
		}
		os << std::endl;
	}

	os << util::plist(_line_indexes) << std::endl;

	return os;
}

const char *ByteCode::command_str(Command c) {
	switch (c) {
		case Next:
			return "next";
		case Load:
			return "load";
		case Set:
			return "set";
		case Deref:
			return "deref";
		case Inc:
			return "inc";
		case Dec:
			return "dec";
		case Jump:
			return "jump";
		case Tern:
			return "tern";
		case Read:
			return "read";
		case Write:
			return "write";
	}
}

util::Result<uint32_t, void> ByteCode::_eval_comptime(cg::AstNode const &node) {
	auto &cfg_name = node.cfg_rule();
	if (cfg_name == "expression") {
		return _eval_comptime(node.begin()[0]);
	} else if (cfg_name == "exp_address") {
		auto identifier = node.begin()[1].begin()[0].begin()[0];
		auto ident_name = identifier.tok().content();
		return _table.symbol_index(ident_name);
	} else {
		return {};
	}
}

util::Result<CPType, KError> ByteCode::_compile_exp(cg::AstNode const &node) {
	try {
		log_debug() << "compiling " << node.cfg_rule() << std::endl;
		auto &cfg_name = node.cfg_rule();
		if (cfg_name == "expression") {
			return _compile_exp(node.begin()[0]);
		} else if (cfg_name == "exp_address") {
			if (node.begin()[1].begin()[0].cfg_rule() != "exp_ident") {
				return KError::compile("Cannot take address of value that is not an lvalue.");
			}
			auto identifier = node.begin()[1].begin()[0].begin()[0];
			log_assert(identifier.tok().type() == cg::Token::Type::Ident, "Node must be an identifier.");
			auto ident_name = identifier.tok().content();
			log_debug() << "about to log " << ident_name << std::endl;
			_load_commamnd(_table.symbol_index(ident_name), _commands);

			auto &symbol = _table[ident_name];
			auto type = symbol.type();
			type.pointer_types.push_back(CPType::PointerType::Normal);
			return type;
		} else if (cfg_name == "exp_ident") {
			auto ident_name = node.begin()[0].tok().content();
			_load_commamnd(_table.symbol_index(ident_name), _commands);
			_commands.push_back(Command::Deref);
			return _table[ident_name].type();
		} else if (cfg_name == "exp_deref") {
			auto type = _compile_exp(node.begin()[1]).value();
			if (type.pointer_types.size() == 0) {
				return KError::compile("Cannot deref again");
			}
			switch (type.pointer_types.back()) {
				case CPType::Normal:
					_commands.push_back(Deref);
					break;
				case CPType::Incriment:
					_commands.push_back(Inc);
					break;
				case CPType::Decriment:
					_commands.push_back(Dec);
					break;
				case CPType::Jump:
					_commands.push_back(Jump);
					if (type.pointer_types.size() > 1) {
						//return KError::compile("Cannot have pointers beyond a jump pointer", node.location());
					}
					break;
				case CPType::Ternary:
					_commands.push_back(Tern);
					break;
				case CPType::Read:
					_commands.push_back(Read);
					break;
				case CPType::Write:
					_commands.push_back(Write);
					break;
			}
			if (type.pointer_types.back() != CPType::Jump) {
				type.pointer_types.pop_back();
			}
			return type;
		} else {
			return KError::compile(util::f("Unimplimented node ", node.cfg_rule()));
		}
		return CPType();
	} catch_kerror;
}


