#include <fstream>
#include <iostream>

#include "CPType.hpp"
#include "Grammar.hpp"
#include "util/file.hpp"
#include "codegen/AstNode.hpp"
#include "codegen/AstNodeIterator.hpp"
#include "util/log.hpp"
#include "SymbolTable.hpp"
#include "ByteCode.hpp"
#include "Args.hpp"

int main(int argc, char **argv) {

	g_args.parse_args(argc, argv);

	auto parser = std::move(create_parser().value());
	auto context = cg::ParserContext();
	auto f = std::ifstream(g_args.file);
	auto src = util::readFile(f);
	auto &node = *parser->parse(
		util::StringRef(src.c_str(), g_args.file.c_str()),
		context
	).value();
	node.compress(parser->cfg().prim_names());
	node.remove_children({"whitespace"});
	node.trim();

	auto node_file = std::ofstream("./assets/example1.dot");
	node.print_dot(node_file, "Program output");
	node_file.close();
	
	//auto table = SymbolTable::create(node);
	//log_debug() << table << std::endl;
	//std::cout << ByteCode::create(node).value();

	if (auto c = ByteCode::create(node, context)) {
		log_debug() << c.value() << std::endl;
		c->execute();
	} else {
		log_error(c);
	}
}
