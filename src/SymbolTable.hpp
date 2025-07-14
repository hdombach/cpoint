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

		std::ostream &print(std::ostream &os) const;

	private:
		std::string _name;
		CPType _type;
		uint32_t _address;
};

class SymbolTable {
	public:
		SymbolTable() = default;
		static SymbolTable create(cg::AstNode const &node);

		std::ostream &print(std::ostream &os) const;
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
