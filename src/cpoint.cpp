#include "Grammar.hpp"
#include "util/file.hpp"
#include <fstream>
#include <iostream>

int main(int argc, char **argv) {

	if (argc != 2) {
		std::cout << "Provide file: " << std::endl;
		return 1;
	}

	auto parser = create_parser();
	auto f = std::ifstream(argv[1]);
	auto node = parser->parse(
		util::readFile(f),
		"statements"
	);
	node->trim();
	node->print_dot(std::cout, "Program output");
}
