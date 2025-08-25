#include "SymbolTable.hpp"
#include "codegen/AstNodeIterator.hpp"
#include "util/log.hpp"
#include "util/result.hpp"

util::Result<std::string, void> _find_name(cg::AstNode const &node) {
	if (node.tok().type() == cg::Token::Ident) {
		return {node.tok().content()};
	}

	for (auto &child : node) {
		if (auto str = _find_name(child)) {
			return str;
		}
	}
	return {};
}

Symbol::Symbol(uint32_t address, cg::AstNode const &node) {
	if (auto n = _find_name(node)) {
		_name = n.value();
	} else {
		log_fatal_error() << "Cannot find symbol name in node " << node << std::endl;
	}
	_name = _find_name(node).value();
	_type = CPType::create(node);
	_address = address;
	_statement = &node;
	if (node.child_count() == 4) {
		_expression = &node.begin()[2];
	} else {
		_expression = nullptr;
	}
}

uint32_t Symbol::address() const {
	return _address;
}

std::string const &Symbol::name() const {
	return _name;
}

cg::AstNode const *Symbol::expression() const {
	return _expression;
}

cg::AstNode const *Symbol::statement() const {
	return _statement;
}

CPType const &Symbol::type() const {
	return _type;
}

std::ostream &Symbol::print(std::ostream &os) const {
	return os << _name << "@" << std::hex << _address << std::dec << ": " << _type << std::endl;
}

SymbolTable SymbolTable::create(cg::AstNode const &node) {
	log_assert(node.cfg_rule() == "statements", "Root must be statements");
	uint32_t addr = 0;
	auto table = SymbolTable();
	for (auto &child : node) {
		if (child.cfg_rule() != "statement") continue;
		auto symbol = Symbol(addr, child);

		table._symbol_map[symbol.name()] = table._symbols.size();
		table._symbols.push_back(symbol);
		addr++;
	}
	return table;
}

std::ostream &SymbolTable::print(std::ostream &os) const {
	for (auto &symbol : _symbols) {
		os << symbol;
	}
	return os;
}

Symbol *SymbolTable::begin() {
	return _symbols.data();
}

Symbol *SymbolTable::end() {
	return _symbols.data() + _symbols.size();
}

size_t SymbolTable::size() const {
	return _symbols.size();
}

Symbol &SymbolTable::operator[](uint32_t i) {
	return _symbols[i];
}

Symbol &SymbolTable::operator[](std::string const &name) {
	return _symbols[symbol_index(name)];
}

size_t SymbolTable::symbol_index(std::string const &name) const {
	log_assert(_symbol_map.contains(name), util::f("Unknown symbol: ", name));
	return _symbol_map.at(name);
}
