#include "Args.hpp"
#include "util/log.hpp"

void Args::parse_args(int argc, char **argv) {
	int i = 1;
	runtime = argv[0];

	while (i < argc) {
		i += _parse_arg(argc - i, argv + i);
	}

	if (file.empty()) {
		log_error() << "Must provide a file" << std::endl;
		print_help(std::cout);
		exit(1);
	}
}

int Args::_parse_arg(int argc, char **argv) {
	if (argc <= 0) {
		return 0;
	}
	if (strcmp(argv[0], "-h") == 0 || strcmp(argv[0], "--help") == 0) {
		print_help(std::cout);
		exit(0);
	} else if (strcmp(argv[0], "-v") == 0) {
		util::g_log_flags |= util::Importance::DEBUG;
		return 1;
	} else if (strcmp(argv[0], "-vv") == 0) {
		util::g_log_flags |= util::Importance::DEBUG | util::Importance::TRACE;
		return 1;
	} else if (strcmp(argv[0], "-c") == 0) {
		mode = Mode::Character;
		return 1;
	} else if (strcmp(argv[0], "-d") == 0) {
		mode = Mode::Integer;
		return 1;
	} else {
		if (file.empty()) {
			file = argv[0];
		} else {
			log_error() << "Unknown arg: " << argv[0] << "."
				<< "File being parsed is already " << file << std::endl;
		}
		return 1;
	}
}

void Args::print_help(std::ostream &os) {
	os << "usage: " << runtime << " [OPTION]... FILE" << std::endl;
	os << "FILE: the file to run" << std::endl;
	os << "OPTION:" << std::endl;
	os << "\t-h, --help  show this message" << std::endl;
	os << "\t-v          debug" << std::endl;
	os << "\t-vv         very debug" << std::endl;
	os << "\t-c          read/write UTF-32 characters" << std::endl;
	os << "\t-d          read/write integers" << std::endl;
}

Args g_args;
