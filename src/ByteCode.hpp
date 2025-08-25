#pragma once

#include <vector>
#include "util/result.hpp"
#include "util/KError.hpp"
#include "codegen/AstNode.hpp"
#include "SymbolTable.hpp"
#include "codegen/ParserContext.hpp"

enum Command: uint8_t {
	Next,
	Load,
	Set,
	Deref,
	Inc,
	Dec,
	Jump,
	Tern,
	Read,
	Write,
};

class ByteCode {
	public:
		ByteCode() = default;

		static util::Result<ByteCode, KError> create(cg::AstNode const &tree, cg::ParserContext &p_context);

		void execute();

		std::ostream &print(std::ostream &os) const;
		static const char *command_str(Command c);

	private:
		cg::ParserContext *_context = nullptr;
		SymbolTable _table;
		std::vector<Command> _commands;
		std::vector<uint32_t> _line_indexes;
		std::vector<std::pair<uint32_t, uint32_t>> _initial_values;

		util::Result<uint32_t, void> _eval_comptime(cg::AstNode const &node);

		util::Result<CPType, KError> _compile_exp(cg::AstNode const &node);
};

inline std::ostream &operator<<(std::ostream &os, Command const &command) {
	return os << ByteCode::command_str(command);
}

inline std::ostream &operator<<(std::ostream &os, ByteCode const &code) {
	return code.print(os);
}
