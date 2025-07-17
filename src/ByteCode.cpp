#include "ByteCode.hpp"
#include "Memory.hpp"
#include "SymbolTable.hpp"
#include "codegen/AstNodeIterator.hpp"
#include "CPType.hpp"
#include "util/log.hpp"

util::Result<ByteCode, KError> ByteCode::create(const cg::AstNode &tree) {
	auto code = ByteCode();
	code._table = SymbolTable::create(tree);

	for (uint32_t i = 0; i < code._table.size(); i++) {
		auto exp = code._table[i].expression();
		if (exp) {
			code._line_indexes.push_back(code._commands.size());
			code._compile_exp(*exp);
		}
	}

	return code;
};

void _load_constant(uint32_t i, std::vector<Command> &commands) {
	for (uint8_t j = 0; j < 4; j++) {
		commands.push_back(static_cast<Command>(i << j & 0xf));
	}
}

uint32_t _get_constant(uint32_t index, std::vector<Command> const &commands) {
	uint32_t r = 0;
	for (uint8_t j = 0; j < 4; j++) {
		r += commands[index + j] >> j;
	}
	return r;
}


std::ostream &ByteCode::print(std::ostream &os) const {
	size_t i = 0;
	os << "ByteCode" << std::endl;
	while (i < _commands.size()) {
		os << _commands[i];
		if (_commands[i] == Command::Load) {
			os << " " << _get_constant(i+1, _commands);
			i += 5;
		} else {
			i++;
		}
		os << std::endl;
	}
	return os;
}

const char *ByteCode::command_str(Command c) {
	switch (c) {
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

util::Result<CPType, KError> ByteCode::_compile_exp(cg::AstNode const &node) {
	try {
		log_trace() << "compiling " << node.cfg_rule() << std::endl;
		auto &cfg_name = node.cfg_rule();
		if (cfg_name == "expression") {
			return _compile_exp(node.begin()[0]);
		} else if (cfg_name == "exp_address") {
			auto child_type = _compile_exp(node.begin()[1]).value();
			if (!child_type.is_lvalue()) {
				return KError::compile("Cannot take address of value that is not an lvalue.");
			}
			child_type.pointer_types.push_back(CPType::PointerType::Normal);
			auto identifier = node.begin()[1].begin()[0].begin()[0];
			log_assert(identifier.tok().type() == cg::Token::Type::Ident, "Node must be an identifier.");
			_commands.push_back(Command::Load);
			auto ident_name = identifier.tok().content();
			_load_constant(_table.symbol_index(ident_name), _commands);
			return child_type;
		}
		return CPType();
	} catch_kerror;
}


