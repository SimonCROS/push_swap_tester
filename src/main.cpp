#include "complexity.hpp"

#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <optional>
#include <algorithm>
#include <climits>

#include <langinfo.h>
#include <signal.h>

using namespace std;

bool fails = false;
program_opts opts;
program_params params;
vector<string> failed_inputs;
vector<string> error_inputs;

/**
 * @brief Generates the numbers to be used in the test
 *
 * @param rng Random number generator
 * @param args Vector of strings to store the numbers
 * @param cargs Vector of char* to store the arguments
 */
void generate_numbers(std::default_random_engine &rng, vector<string> &args, vector<char *> &cargs)
{
	static vector<int> nums;

	// No need to generate numbers if they already exist, just shuffle them again
	if ((std::vector<int>::size_type)params.numbers != nums.size())
	{
		nums.resize(params.numbers);					// Reserve the space for the numbers
		std::iota(std::begin(nums), std::end(nums), 0); // Fills the vector with 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, ...
	}

	if (!opts.sorted)
	{
		std::shuffle(nums.begin(), nums.end(), rng);
	}

	args.clear();
	cargs.clear();

	std::transform(nums.begin(), nums.end(), std::back_inserter(args), [](int d)
				   { return std::to_string(d); });

	cargs.push_back(params.program.data());
	for (string &a : args)
		cargs.push_back(a.data());
	cargs.push_back(nullptr);
}

void launchTest()
{
	std::default_random_engine rng = std::default_random_engine(opts.seed.value());

	vector<string> args;
	vector<char *> cargs;
	vector<int> results_list;

	args.reserve(params.numbers);
	cargs.reserve(params.numbers + 2);
	results_list.reserve(params.iterations);

	int done = 0, worst = 0, best = -1, total = 0, successful = 0, ok = 0;
	double mean, stddev;

	hideCursor();
	while (done < params.iterations)
	{
		generate_numbers(rng, args, cargs);

		string result = exec(cargs.data());
		int lines = std::count(result.begin(), result.end(), '\n');
		bool input_saved = false;

		done++;
		results_list.push_back(lines);
		total += lines;

		if (params.checker.has_value())
		{
			cargs[0] = params.checker.value().data();

			if (exec(cargs.data(), result) == "OK\n")
			{
				ok++;
			}
			else
			{
				fails = true;
				if (opts.output.has_value() && !input_saved)
				{
					input_saved = true;
					error_inputs.push_back(join(args, " "));
				}
			}
		}

		if (params.objective.has_value())
		{
			if (lines <= params.objective.value())
			{
				successful++;
			}
			else
			{
				fails = true;
				if (opts.output.has_value() && !input_saved)
				{
					input_saved = true;
					failed_inputs.push_back(join(args, " "));
				}
			}
		}

		if (lines < best || best == -1)
			best = lines;
		if (lines > worst)
			worst = lines;

		mean = double(total) / done;
		stddev = 0.0;
		for (int result : results_list)
			stddev += pow(mean - result, 2.0);

		stddev = sqrt(stddev / done);

		print_status(params, done, round(mean), stddev, best, worst, successful, ok);
		cout << "\033[7A";
	}
}

void start()
{
	std::random_device rd;
	opts.seed = opts.seed.value_or(rd());

	printStart(opts, params);
	launchTest();

	if (opts.output.has_value())
	{
		ofstream file(opts.output.value());
		if (file.is_open())
		{
			if (params.objective.has_value())
			{
				file << "Failed inputs (over " << params.objective.value() << "):\n";
				for (string &input : failed_inputs)
					file << input << endl;
			}

			if (params.checker.has_value())
			{
				file << "Error inputs:\n";
				for (string &input : error_inputs)
					file << input << endl;
			}
			file.close();
		}
	}
}

int main(int argc, char **argv)
{
	opts = getOptions(argc, argv);
	if (opts.help || opts.version)
	{
		if (opts.version)
			cout << getVersion() << endl;
		if (opts.help)
			cout << getHelp() << endl;
		return EXIT_SUCCESS;
	}

	try
	{
		params = getParameters(argc, argv);
		if (opts.program.has_value())
			params.program = assertExecutable(opts.program.value());
		else
		{
			try
			{
				params.program = assertExecutable("../push_swap");
			}
			catch (const std::invalid_argument &e)
			{
				params.program = assertExecutable("./push_swap");
			}
		}
	}
	catch (const std::invalid_argument &e)
	{
		if (*e.what())
			cerr << e.what() << endl;
		cerr << getUsage() << endl;
		return EXIT_FAILURE;
	}
	if (opts.output.has_value() && !params.objective.has_value() && !params.checker.has_value())
	{
		cout << "You must specify an objective or a checker to use the output option." << endl;
		return EXIT_FAILURE;
	}
	if (opts.sorted)
		params.iterations = 1;

	std::atexit([]()
				{
		cout << "\033[7B\033[0m";
		showCursor();
		printEnd(opts, params); });
	signal(SIGINT, [](int)
		   { exit(fails); });

	start();

	return fails;
}
