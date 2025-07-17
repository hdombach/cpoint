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

int main(int argc, char **argv) {

	util::g_log_flags |= util::Importance::DEBUG;

	if (argc != 2) {
		std::cout << "Provide file: " << std::endl;
		return 1;
	}

	auto parser = std::move(create_parser().value());
	auto context = cg::ParserContext();
	auto f = std::ifstream(argv[1]);
	auto src = util::readFile(f);
	auto &node = *parser->parse(
		util::StringRef(src.c_str(), argv[1]),
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
	std::cout << ByteCode::create(node).value();
}
