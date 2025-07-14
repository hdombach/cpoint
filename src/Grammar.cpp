#include "Grammar.hpp"
#include "codegen/CfgContext.hpp"
#include "codegen/Tokenizer.hpp"
#include "codegen/AbsoluteSolver.hpp"

// Right now this can only be called once
util::Result<cg::Parser::Ptr, KError> create_parser() {
	try {
		auto ctx = cg::CfgContext::create();
		auto &c = *ctx;
		using T = cg::Token::Type;

		c.root("file") = c["statements"] + T::Eof;

		c.prim("whitespace") = T::Whitespace | c.empty();

		c.temp("statements")
			= c["statement"] + c["statements"]
			| c.empty();

		c.prim("statement")
			= c["declaration"] + T::SemiColon + c["whitespace"]
			| c["declaration"] + T::Equal + c["whitespace"] + c["expression"] + T::SemiColon + c["whitespace"];

		c.prim("declaration") = T::Int + c["whitespace"] + c["decl"];
		c.prim("decl") = c["decl1"];

		c.temp("decl1")
			= c["decl2"] + c["decl1_append"];
		c.temp("decl1_append")
			= c["decl_jmp"] + c["decl1_append"]
			| c["decl_ternary"] + c["decl1_append"]
			| c.empty();

		c.temp("decl2")
			= c["decl_cout"]
			| c["decl_cin"]
			| c["decl_dec"]
			| c["decl_inc"]
			| c["decl_ptr"]
			| c["decl_sing"];

		c.temp("decl_sing")
			= T::Ident + c["whitespace"]
			| T::ParanOpen + c["whitespace"] + c["decl"] + T::ParanClose + c["whitespace"];

		c.prim("decl_jmp") = T::JmpPtr + c["whitespace"];
		c.prim("decl_ternary") = T::TernPtr + c["whitespace"];

		c.prim("decl_cout") = T::WritePtr + c["whitespace"] + c["decl2"];
		c.prim("decl_cin") = T::ReadPtr + c["whitespace"] + c["decl2"];
		c.prim("decl_dec") = T::DecPtr + c["whitespace"] + c["decl2"];
		c.prim("decl_inc") = T::IncPtr + c["whitespace"] + c["decl2"];
		c.prim("decl_ptr") = T::Ptr + c["whitespace"] + c["decl2"];

		c.prim("expression")
			= T::Ptr + c["whitespace"] + c["expression"]
			| T::Amper + c["whitespace"] + c["expression"]
			| T::ParanOpen + c["whitespace"] + c["expression"] + T::ParanClose + c["whitespace"]
			| T::Ident + c["whitespace"];

		TRY(c.prep());
		c.simplify();

		cg::Parser::Ptr parser = std::move(cg::AbsoluteSolver::create(std::move(ctx)).value());

		return std::move(parser);
	} catch_kerror;
}
