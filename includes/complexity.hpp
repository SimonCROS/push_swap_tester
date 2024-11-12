#ifndef COMPLEXITY_H
#define COMPLEXITY_H

#include <sstream>
#include <iterator>
#include <string>
#include <optional>
#include <random>

using namespace std;

struct program_opts {
	bool					version;
	bool					help;
	bool					sorted;
	optional<std::string>	program;
	optional<std::string>	output;
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
void print_status(const program_params& params, int done, int mean, double stddev, int best, int worst, int successful, int ok);
void printEnd(const program_opts& opts, const program_params& params);

template <typename Range, typename Value = typename Range::value_type>
std::string join(Range const& elements, const char *const delimiter) {
    std::ostringstream os;
    auto b = begin(elements), e = end(elements);

    if (b != e) {
        std::copy(b, prev(e), std::ostream_iterator<Value>(os, delimiter));
        b = prev(e);
    }
    if (b != e) {
        os << *b;
    }

    return os.str();
}

#endif
