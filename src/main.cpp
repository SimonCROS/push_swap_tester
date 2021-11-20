#include "complexity.hpp"

#include <iostream>
#include <vector>
#include <random>
#include <numeric>
#include <optional>

using namespace std;

int main(int argc, char **argv) {
	program_opts opts;
	program_params params;

	opts = getOptions(argc, argv);
	if (opts.help || opts.version) {
		if (opts.version)
			cout << getVersion() << endl;
		if (opts.help)
			cout << getHelp() << endl;
		return (EXIT_SUCCESS);
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

	atexit([]() {
		showCursor();
		cout << "\033[6B\033[0m";
	});
	signal(SIGINT, [](int) {exit(EXIT_SUCCESS);});
	
	auto rd = std::random_device {}; 
	auto rng = std::default_random_engine { rd() };

	int done = 0;
	int worst = 0;
	int best = -1;
	int	total = 0;
	int successful = 0;
	int ok = 0;

	hideCursor();
	cout << "\033[97mDémarrage du test : \033[95m" << params.numbers << "\033[97m éléments, \033[95m" << params.iterations << "\033[97m itérations\033[0m" << endl;
	while (done < params.iterations) {
		vector<int> nums(params.numbers);
		vector<string> args;

		iota(begin(nums), end(nums), 0);
		if (!opts.sorted)
			std::shuffle(nums.begin(), nums.end(), rng);
		std::transform(nums.begin(), nums.end(), std::back_inserter(args), [](int d) {
				return std::to_string(d);
			}
		);

		vector<char*> cargs;
		cargs.push_back(const_cast<char*>(params.program.c_str()));
		for (const string &a : args)
			cargs.push_back(const_cast<char*>(a.c_str()));
		cargs.push_back(nullptr);

		string result = exec(cargs.data());
		int lines = std::count(result.begin(), result.end(), '\n');

		done++;
		total += lines;

		if (params.checker.has_value())
		{
			cargs[0] = const_cast<char*>(params.checker.value().c_str());

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
	return EXIT_SUCCESS;
}
