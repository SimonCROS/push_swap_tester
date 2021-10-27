#include <iostream>
#include <getopt.h>
#include <algorithm>
#include <vector>
#include <array>
#include <random>
#include <string>
#include <iterator>
#include <numeric>
#include <sys/stat.h>
#include <signal.h>
#include <optional>

using namespace std;

struct program_opts
{
	bool				help;
	bool				sorted;
	optional<string>	program;
};

struct program_params
{
	string				program;
	int					numbers;
	int					iterations;
	optional<int>		objective;
	optional<string>	checker;
};

static const string getUsage()
{
	return "usage: ./complexity [-hs] [-p file] numbers iterations [goal] [checker]";
}

static const string getHelp()
{
	string help;

	help += "\033[1mNAME\033[0m\n";
	help += "     \033[1m./complexity\033[0m -- lance un benchmark de votre push_swap\n";
	help += "\n";
	help += "\033[1mSYNOPSIS\033[0m\n";
	help += "     \033[1m./complexity\033[0m [\033[1m-hs\033[0m] [\033[1m-p\033[0m \033[4mfile\033[0m] \033[4mnumbers\033[0m \033[4miterations\033[0m [\033[4mgoal\033[0m] [\033[4mchecker\033[0m]\n";
	help += "\n";
	help += "\033[1mDESCRIPTION\033[0m\n";
	help += "     L'exécutable push_swap est cherché par défaut dans le répertoire courant et parent.\n";
	help += "     \n";
	help += "     Les options suivantes sont disponibles :\n";
	help += "     \n";
	help += "     \033[1m-h\033[0m, \033[1m--help\033[0m\n";
	help += "             Afficher l'aide.\n";
	help += "     \n";
	help += "     \033[1m-s\033[0m, \033[1m--sorted\033[0m\n";
	help += "             Envoie uniquement des nombres triés au program.\n";
	help += "     \n";
	help += "     \033[1m-p\033[0m \033[4mfile\033[0m, \033[1m--program\033[0m=\033[4mfile\033[0m\n";
	help += "             Utilise \033[4mfile\033[0m en tant qu'exécutable push_swap.\n";
	return help;
}

static const string& assertExecutable(const string& path)
{
	struct stat  st;

	if (stat(path.c_str(), &st) < 0 ||
		(st.st_mode & S_IEXEC) == 0 ||
		(st.st_mode & S_IFREG) == 0)
		throw invalid_argument(path + " is not a valid file");
	return path;
}

static int parseNumber(string str, int min)
{
	size_t end;
	int number;

	try
	{
		number = stoi(str.c_str(), &end);
		if (str.length() > end != 0)
			throw invalid_argument("");
	}
	catch(const exception& e)
	{
		throw invalid_argument(str + " is not a valid number");
	}
	if (number < min)
		throw invalid_argument(str + " must be at least equal to " + to_string(min));
	return number;
}

static struct program_opts getOptions(int& argc, char **&argv)
{
	static struct option long_options[] =
	{
		{"help", no_argument, NULL, 'h'},
		{"sorted", no_argument, NULL, 's'},
		{"program", required_argument, NULL, 'p'},
		{NULL, 0, NULL, 0}
	};

	program_opts opts = {false, false, nullopt};

	int ch;
	while ((ch = getopt_long(argc, argv, "hsp:", long_options, NULL)) != -1)
	{
		switch (ch)
		{
			case 'h':
				opts.help = true;
				break;
			case 'p':
				opts.program = optarg;
				break;
			case 's':
				opts.sorted = true;
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

static struct program_params getParameters(int argc, char **argv)
{
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

static void hideCursor()
{
	cout << "\e[?25l";
}

static void showCursor()
{
	cout << "\e[?25h";
}

#define NUM_PIPES		  2
#define PARENT_WRITE_PIPE  0
#define PARENT_READ_PIPE   1
int pipes[NUM_PIPES][2];
#define READ_FD  0
#define WRITE_FD 1
#define PARENT_READ_FD  ( pipes[PARENT_READ_PIPE][READ_FD]   )
#define PARENT_WRITE_FD ( pipes[PARENT_WRITE_PIPE][WRITE_FD] )
#define CHILD_READ_FD   ( pipes[PARENT_WRITE_PIPE][READ_FD]  )
#define CHILD_WRITE_FD  ( pipes[PARENT_READ_PIPE][WRITE_FD]  )

static string exec(char** argv, optional<string> input = nullopt)
{
	std::array<char, 128> buffer;
	string result;

	pipe(pipes[PARENT_READ_PIPE]);
	pipe(pipes[PARENT_WRITE_PIPE]);

	if(fork() == 0)
	{ 
		dup2(CHILD_READ_FD, STDIN_FILENO);
		dup2(CHILD_WRITE_FD, STDOUT_FILENO);
		dup2(CHILD_WRITE_FD, STDERR_FILENO);

		close(CHILD_READ_FD);
		close(CHILD_WRITE_FD);
		close(PARENT_READ_FD);
		close(PARENT_WRITE_FD);

		execv(argv[0], argv);
		exit(EXIT_FAILURE);
	}
	else
	{
		close(CHILD_READ_FD);
		close(CHILD_WRITE_FD);
		if (input.has_value())
			write(PARENT_WRITE_FD, input.value().c_str(), input.value().size());
		close(PARENT_WRITE_FD);

		int len = 0;
		while ((len = read(PARENT_READ_FD, buffer.data(), buffer.size() - 1)) > 0)
		{
			buffer[len] = 0;
			result += buffer.data();
		}
		close(PARENT_READ_FD);

		waitpid(-1, NULL, 0);
	}
	return result;
}

static void print(program_params params, int done, int total, int best, int worst, int successful, int ok)
{
	cout << "Pire = \033[31m" << (worst) << "\033[0m instructions" << endl;
	cout << "Moyenne = \033[33m" << (total / done) << "\033[0m instructions" << endl;
	cout << "Meilleur = \033[36m" << (best) << "\033[0m instructions" << endl;
	if (params.objective.has_value())
		cout << "Objectif = \033[94m" << (successful * 100 / done) << "\033[0m % sous \033[94m" << (params.objective.value()) << "\033[0m (\033[91m" << (done - successful) << "\033[0m au dessus)   " << endl;
	else
		cout << "Objectif = entrez un nombre en troisième argument" << endl;
	if (params.checker.has_value())
		cout << "Précision = \033[97m" << (ok * 100 / done) << "\033[0m % OK (\033[91m" << (done - ok) << "\033[0m KO)   " << endl;
	else
		cout << "Précision = entrez un testeur en quatrième argument" << endl;
	cout << "\033[32m" << (done * 100 / params.iterations) << "\033[0m % effectué" << endl;
}

int main(int argc, char **argv)
{
	program_opts opts;
	program_params params;

	opts = getOptions(argc, argv);
	if (opts.help)
	{
		cout << getHelp() << endl;
		return (EXIT_SUCCESS);
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
			catch(const std::invalid_argument& e)
			{
				params.program = assertExecutable("./push_swap");
			}
		}
	}
	catch (const std::invalid_argument& e)
	{
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
	while (done < params.iterations)
	{
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
		for (const string& a : args)
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
