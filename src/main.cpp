#include "complexity.hpp"

#include <cmath>
#include <iostream>
#include <vector>
#include <numeric>
#include <optional>
#include <algorithm>
#include <climits>

#include <langinfo.h>
#include <signal.h>

using namespace std;

bool fails = false;

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

void launchTest(program_opts& opts, program_params& params) {
	std::default_random_engine rng = std::default_random_engine(opts.seed.value());

	vector<string> args;
	vector<char*> cargs;
	vector<int> results_list;

	args.reserve(params.numbers);
	cargs.reserve(params.numbers + 2);
	results_list.reserve(params.iterations);

	int done = 0, worst = 0, best = -1, total = 0, successful = 0, ok = 0;
	double mean, stddev;

	hideCursor();
	while (done < params.iterations) {
		generate_numbers(rng, opts, params, args, cargs);

		string result = exec(cargs.data());
		int lines = std::count(result.begin(), result.end(), '\n');

		done++;
		results_list.push_back(lines);
		total += lines;

		if (params.checker.has_value()) {
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

		mean = double(total) / done;
		stddev = 0.0;
		for (int result : results_list)
			stddev += pow(mean - result, 2.0);

		stddev = sqrt(stddev / done);

		print(params, done, round(mean), stddev, best, worst, successful, ok);
		cout << "\033[7A";
	}

	if (params.checker.has_value() && (done - ok > 0))
		fails = true;
	else if (params.objective.has_value() && (done - successful > 0))
		fails = true;
}

void start(program_opts& opts, program_params& params) {
	std::random_device rd;
	opts.seed = opts.seed.value_or(rd());

	printStart(opts, params);
	launchTest(opts, params);
	printEnd(opts, params);
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

	std::atexit([]() {
		cout << "\033[7B\033[0m";
		showCursor();
	});
	signal(SIGINT, [](int) {
		exit(fails);
	});
	start(opts, params);
	
	return fails;
}
