#ifndef COMPLEXITY_H
#define COMPLEXITY_H

#include <string>
#include <optional>
#include <random>

using namespace std;

struct program_opts {
	bool					version;
	bool					help;
	bool					sorted;
	optional<std::string>	program;
	optional<std::default_random_engine::result_type> seed;
};

struct program_params {
	string				program;
	int					numbers;
	int					iterations;
	optional<int>		objective;
	optional<string>	checker;
};

void hideCursor();
void showCursor();
const string getHelp();
const string getUsage();
const string getVersion();
const string &assertExecutable(const string &path);
struct program_opts getOptions(int &argc, char **&argv);
struct program_params getParameters(int argc, char **argv);
string exec(char **argv, optional<string> input = nullopt);

// Print
void printStart(const program_opts& opts, const program_params& params);
void print(const program_params& params, int done, int mean, double stddev, int best, int worst, int successful, int ok);
void printEnd(const program_opts& opts, const program_params& params);

#endif
