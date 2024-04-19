#include "complexity.hpp"

#include <iostream>
#include <getopt.h>

using namespace std;

static long parseNumber(string str, int min) {
	size_t end;
	long number;

	try {
		number = stol(str.c_str(), &end);
		if (str.length() > end != 0)
			throw invalid_argument("");
	}
	catch(const exception &e) {
		throw invalid_argument(str + " is not a valid number");
	}
	if (number < min)
		throw invalid_argument(str + " must be at least equal to " + to_string(min));
	return number;
}

struct program_opts getOptions(int &argc, char **&argv) {
	static struct option long_options[] = {
		{"version", no_argument, NULL, 'v'},
		{"help", no_argument, NULL, 'h'},
		{"sorted", no_argument, NULL, 42},
		{"output", required_argument, NULL, 'o'},
		{"file", required_argument, NULL, 'f'},
		{"seed", required_argument, NULL, 's'},
		{NULL, 0, NULL, 0}
	};

	program_opts opts = {false, false, false, nullopt, nullopt, nullopt};

	int ch;
	while ((ch = getopt_long(argc, argv, "vhs:f:o:", long_options, NULL)) != -1) {
		switch (ch) {
			case 'v':
				opts.version = true;
				break;
			case 'h':
				opts.help = true;
				break;
			case 'f':
				opts.program = optarg;
				break;
			case 'o':
				opts.output = optarg;
				break;
			case 42:
				opts.sorted = true;
				break;
			case 's':
				opts.seed = parseNumber(optarg, 0);
				break;
			default:
				cerr << getUsage() << endl;
				exit(EXIT_FAILURE);
		}
	}
	argc -= optind;
	argv += optind;

	return opts;
}

struct program_params getParameters(int argc, char **argv) {
	program_params params;

	if (argc < 2 || argc > 4)
		throw invalid_argument("");
	params.numbers = parseNumber(argv[0], 0);
	params.iterations = parseNumber(argv[1], 1);
	if (argc >= 3)
		params.objective = parseNumber(argv[2], 0);
	if (argc >= 4)
		params.checker = assertExecutable(argv[3]);
	return params;
}
