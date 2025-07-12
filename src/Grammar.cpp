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

		c.prim("declaration") = T::Int + c["whitespace"] + c["decl1"];

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
			| c["decl_sing"];

		c.temp("decl_sing")
			= T::Ident + c["whitespace"];

		c.prim("decl_jmp") = T::JmpPtr + c["whitespace"];
		c.prim("decl_ternary") = T::TernPtr + c["whitespace"];

		c.prim("decl_cout") = T::WritePtr + c["decl2"];
		c.prim("decl_cin") = T::ReadPtr + c["decl2"];
		c.prim("decl_dec") = T::DecPtr + c["decl2"];
		c.prim("decl_inc") = T::IncPtr + c["decl2"];

		c.prim("expression") = T::Digit;

		TRY(c.prep());
		c.simplify();

		cg::Parser::Ptr parser = std::move(cg::AbsoluteSolver::create(std::move(ctx)).value());

		return std::move(parser);
	} catch_kerror;
}
