#pragma once

#include "util/log.hpp"
#include <vector>
#include <string>
#include <filesystem>

struct Args {
	public:
		enum Mode {
			Character,
			Integer,
		} mode = Character;
		std::filesystem::path file;
		std::string runtime;

		void parse_args(int argc, char **argv);

		void print_help(std::ostream &os);

	private:
		int _parse_arg(int argc, char **argv);
};

extern Args g_args;

