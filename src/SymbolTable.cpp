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
	_name = _find_name(node).value();
	_type = CPType::create(node);
	_address = address;
	if (node.child_count() == 4) {
		_expression = &node.begin()[2];
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

std::ostream &Symbol::print(std::ostream &os) const {
	return os << _name << "@" << std::hex << _address << ": " << _type << std::endl;
}

SymbolTable SymbolTable::create(cg::AstNode const &node) {
	log_assert(node.cfg_rule() == "statements", "Root must be statements");
	uint32_t addr = 0;
	auto table = SymbolTable();
	for (auto &child : node) {
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

size_t SymbolTable::symbol_index(std::string const &name) const {
	return _symbol_map.at(name);
}
