#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <map>

#include "CPType.hpp"
#include "codegen/AstNode.hpp"

class Symbol {
	public:
		Symbol() = default;
		Symbol(uint32_t address, cg::AstNode const &node);

		uint32_t address() const;
		std::string const &name() const;
		cg::AstNode const *expression() const;
		CPType const &type() const;

		std::ostream &print(std::ostream &os) const;

	private:
		std::string _name;
		CPType _type;
		uint32_t _address;
		cg::AstNode const *_expression;
};

class SymbolTable {
	public:
		SymbolTable() = default;
		static SymbolTable create(cg::AstNode const &node);

		std::ostream &print(std::ostream &os) const;

		Symbol *begin();
		Symbol *end();

		size_t size() const;
		Symbol &operator[](uint32_t i);
		Symbol &operator[](std::string const &name);
		size_t symbol_index(std::string const &name) const;
	private:
		std::vector<Symbol> _symbols;
		std::map<std::string, uint32_t> _symbol_map;
};

inline std::ostream &operator<<(std::ostream &os, Symbol const &symbol) {
	return symbol.print(os);
}

inline std::ostream &operator<<(std::ostream &os, SymbolTable const &table) {
	return table.print(os);
}
