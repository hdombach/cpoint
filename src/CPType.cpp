#include "CPType.hpp"
#include "util/PrintTools.hpp"
#include "util/log.hpp"
#include "codegen/AstNode.hpp"
#include "codegen/AstNodeIterator.hpp"

void _add_type(CPType &type, cg::AstNode const &node) {
	auto cfg = node.cfg_rule();
	for (auto &child : node) {
		_add_type(type, child);
	}

	if (cfg == "decl_dec") {
		type.pointer_types.push_back(CPType::PointerType::Decriment);
	} else if (cfg == "decl_inc") {
		type.pointer_types.push_back(CPType::PointerType::Incriment);
	} else if (cfg == "decl_cout") {
		type.pointer_types.push_back(CPType::PointerType::Write);
	} else if (cfg == "decl_cin") {
		type.pointer_types.push_back(CPType::PointerType::Read);
	} else if (cfg == "decl_ptr") {
		type.pointer_types.push_back(CPType::PointerType::Normal);
	} else if (cfg == "decl_jmp") {
		type.pointer_types.push_back(CPType::PointerType::Jump);
	} else if (cfg == "decl_ternary") {
		type.pointer_types.push_back(CPType::Ternary);
	}
}

CPType CPType::create(cg::AstNode const &node) {
	auto t = CPType();
	_add_type(t, node);
	return t;
}

std::ostream &CPType::print(std::ostream &os) const {
	return os << util::plist(pointer_types);
}

bool CPType::is_lvalue() const {
	return pointer_types.empty();
}

const char *CPType::pointer_type_str(PointerType type) {
	switch (type) {
		case Normal:
			return "normal";
		case Incriment:
			return "inc";
		case Decriment:
			return "dec";
		case Jump:
			return "jump";
		case Ternary:
			return "tern";
		case Read:
			return "read";
		case Write:
			return "write";
	}
}
