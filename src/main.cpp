#include "complexity.hpp"

#include <iostream>
#include <vector>
#include <random>
#include <numeric>
#include <optional>
#include <algorithm>

#include <langinfo.h>
#include <signal.h>

using namespace std;

void generate_numbers(std::default_random_engine& rng, program_opts& opts, program_params& params, vector<string>& args, vector<char*>& cargs) {
	vector<int> nums(params.numbers);

	std::iota(std::begin(nums), std::end(nums), 0);
	if (!opts.sorted)
		std::shuffle(nums.begin(), nums.end(), rng);

	args.clear();
	cargs.clear();

	std::transform(nums.begin(), nums.end(), std::back_inserter(args), [](int d) {
		return std::to_string(d);
	});

	cargs.push_back(params.program.data());
	for (string &a : args)
		cargs.push_back(a.data());
	cargs.push_back(nullptr);
}

int launch_test(program_opts& opts, program_params& params) {
	std::random_device rd; 
	std::default_random_engine rng = std::default_random_engine(rd());

	vector<string> args;
	vector<char*> cargs;

	args.reserve(params.numbers);
	cargs.reserve(params.numbers + 2);

	int done = 0, worst = 0, best = -1, total = 0, successful = 0, ok = 0;

	hideCursor();
	print_start(params);
	while (done < params.iterations) {
		generate_numbers(rng, opts, params, args, cargs);

		string result = exec(cargs.data());
		int lines = std::count(result.begin(), result.end(), '\n');

		done++;
		total += lines;

		if (params.checker.has_value())
		{
			cargs[0] = params.checker.value().data();

			if (exec(cargs.data(), result) == "OK\n")
				ok++;
		}
		if (lines <= params.objective.value_or(-1))
			successful++;
		if (lines < best || best == -1)
			best = lines;
		if (lines > worst)
			worst = lines;

		print(params, done, total, best, worst, successful, ok);
		cout << "\033[6A";
	}
	return (((done - ok) * params.checker.has_value()) + (done - successful));
}

int main(int argc, char **argv) {
	program_opts opts;
	program_params params;

	opts = getOptions(argc, argv);
	if (opts.help || opts.version) {
		if (opts.version)
			cout << getVersion() << endl;
		if (opts.help)
			cout << getHelp() << endl;
		return EXIT_SUCCESS;
	}

	try {
		params = getParameters(argc, argv);
		if (opts.program.has_value())
			params.program = assertExecutable(opts.program.value());
		else {
			try {
				params.program = assertExecutable("../push_swap");
			} catch(const std::invalid_argument &e) {
				params.program = assertExecutable("./push_swap");
			}
		}
	} catch (const std::invalid_argument &e) {
		if (*e.what())
			cerr << e.what() << endl;
		cerr << getUsage() << endl;
		return EXIT_FAILURE;
	}
	if (opts.sorted)
		params.iterations = 1;

	atexit([]() {
		showCursor();
		cout << "\033[6B\033[0m";
	});
	signal(SIGINT, [](int) {exit(EXIT_SUCCESS);});
	
	return launch_test(opts, params);
}
