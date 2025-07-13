#pragma once

#include <vector>
#include <ostream>

#include "codegen/AstNode.hpp"

struct CPType {
	public:
		enum PointerType {
			Normal,
			Incriment,
			Decriment,
			Jump,
			Ternary,
			Read,
			Write,
		};

		static CPType create(cg::AstNode const &node);
		std::ostream &print(std::ostream &os) const;

		std::vector<PointerType> pointer_types;

		static const char *pointer_type_str(PointerType type);
};

inline std::ostream &operator<<(std::ostream &os, CPType::PointerType type) {
	return os << CPType::pointer_type_str(type);
}

inline std::ostream &operator<<(std::ostream &os, CPType const &type) {
	return type.print(os);
}
