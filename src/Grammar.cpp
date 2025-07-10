#include "Grammar.hpp"
#include "codegen/CfgContext.hpp"
#include "codegen/SParser.hpp"

// Right now this can only be called once
util::Result<cg::SParser, KError> create_parser() {
	static auto c = cg::CfgContext();

	c.prim("whitespace")
		= c.s(" ") + c["whitespace"]
		| c.s("\t") + c["whitespace"]
		| c.s("\n") + c["whitespace"]
		| c.s("");

	c.temp("digit") = c.i("0123456789");

	c.temp("lower") = c.i("abcdefghijklmnopqrstuvwxyz");
	c.temp("upper") = c.i("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	c.temp("alpha") = c["lower"] | c["upper"];
	c.temp("alnum") = c["alpha"] | c["digit"];
	c.temp("identifier_start") = c.s("_") | c["alpha"];
	c.temp("identifier_rest")
		= c["alnum"] + c["identifier_rest"]
		| c.s("_") + c["identifier_rest"]
		| c.s("");

	c.prim("identifier") = c["identifier_start"] + c["identifier_rest"];

	c.prim("statement")
		= c["declaration"] + c.s(";") + c["whitespace"]
		| c["declaration"] + c.s("=") + c["expression"] + c.s(";") + c["whitespace"];

	c.prim("declaration") = c["whitespace"] + c.s("int") + c["decl1"];

	c.temp("decl1")
		= c["decl2"] + c["decl1_append"];
	c.temp("decl1_append")
		= c["decl_jmp"] + c["decl1_append"]
		| c["decl_ternary"] + c["decl1_append"]
		| c.s("");

	c.temp("decl2")
		= c["decl_cout"]
		| c["decl_cin"]
		| c["decl_dec"]
		| c["decl_inc"]
		| c["decl_ptr"]
		| c["decl_sing"];

	c.temp("decl_sing")
		= c["whitespace"] + c["identifier"] + c["whitespace"];

	c.prim("decl_jmp") = c["whitespace"] + c.s("^") + c["whitespace"];
	c.prim("decl_ternary") = c["whitespace"] + c.s("?") + c["whitespace"];

	c.prim("decl_cout") = c["whitespace"] + c.s("<<") + c["decl2"] + c["whitespace"];
	c.prim("decl_cin") = c["whitespace"] + c.s(">>") + c["decl2"] + c["whitespace"];
	c.prim("decl_dec") = c["whitespace"] + c.s("-") + c["decl2"] + c["whitespace"];
	c.prim("decl_inc") = c["whitespace"] + c.s("+") + c["decl2"] + c["whitespace"];
	c.prim("decl_ptr") = c["whitespace"] + c.s("*") + c["decl2"] + c["whitespace"];

	c.prim("expression") = c["whitespace"] + c["exp_int"] + c["whitespace"];

	c.prim("exp_int")
			= c["digit"] + c["exp_int"]
			| c["digit"];

	c.prim("statements")
		= c["statement"] + c["statements"]
		| c.s("");

	TRY(c.prep());

	return cg::SParser(c);
}
