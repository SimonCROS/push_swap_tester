#include "complexity.hpp"

#include <iomanip>
#include <iostream>

using namespace std;

const string getHelp() {
	string help;

	help += "\033[1mNAME\033[0m\n";
	help += "     \033[1mcomplexity\033[0m -- lance un benchmark de votre push_swap\n";
	help += "\n";
	help += "\033[1mSYNOPSIS\033[0m\n";
	help += "     \033[1m./complexity\033[0m [\033[1m-vh\033[0m]\n";
	help += "     \033[1m./complexity\033[0m [\033[1m-s\033[0m \033[4mseed\033[0m] [\033[1m-o\033[0m \033[4mfile\033[0m] [\033[1m-f\033[0m \033[4mfile\033[0m] [\033[1m--seed\033[0m] \033[4mnumbers\033[0m \033[4miterations\033[0m [\033[4mgoal\033[0m] [\033[4mchecker\033[0m]\n";
	help += "\n";
	help += "\033[1mDESCRIPTION\033[0m\n";
	help += "     L'exécutable push_swap est cherché par défaut dans le répertoire courant et parent.\n";
	help += "     \n";
	help += "     Les options suivantes sont disponibles :\n";
	help += "     \n";
	help += "     \033[1m-v\033[0m, \033[1m--version\033[0m\n";
	help += "             Affiche la version du testeur.\n";
	help += "     \n";
	help += "     \033[1m-h\033[0m, \033[1m--help\033[0m\n";
	help += "             Affiche l'aide.\n";
	help += "     \n";
	help += "     \033[1m-s\033[0m \033[4mseed\033[0m, \033[1m--seed\033[0m=\033[4mseed\033[0m\n";
	help += "             Génère les nombres en fonction de la graine.\n";
	help += "     \n";
	help += "     \033[1m-o\033[0m \033[4moutput\033[0m, \033[1m--output\033[0m=\033[4moutput\033[0m\n";
	help += "             Spécifie un fichier de sortie pour les logs.\n";
	help += "     \n";
	help += "     \033[1m-f\033[0m \033[4mpush_swap\033[0m, \033[1m--file\033[0m=\033[4mpush_swap\033[0m\n";
	help += "             Utilise \033[4mpush_swap\033[0m en tant qu'exécutable push_swap.\n";
	help += "     \n";
	help += "     \033[1m--sorted\033[0m\n";
	help += "             Envoie uniquement des nombres triés au programme.\n";
	return help;
}

void printStart(const program_opts& opts, const program_params& params) {
	cout << "\033[97mNew pre-release version available, try it on the `v2` branch!\033[0m\n";
	cout << "\033[97mDémarrage du test : \033[95m" << params.numbers << "\033[97m éléments, \033[95m" << params.iterations << "\033[97m itérations\033[0m";
	if (opts.seed.has_value())
		cout << " (seed " << opts.seed.value() << ")";
	cout << endl;
}

void print_status(const program_params& params, int done, int mean, double stddev, int best, int worst, int successful, int ok) {
	cout << "Pire = \033[31m" << (worst) << "\033[0m instructions" << endl;
	cout << "Moyenne = \033[33m" << (mean) << "\033[0m instructions" << endl;
	cout << "Meilleur = \033[36m" << (best) << "\033[0m instructions" << endl;
	cout << "Écart-type = \033[93m" << setiosflags(ios_base::fixed) << setprecision(1) << (stddev) << "\033[0m instructions" << endl;
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

void printEnd(const program_opts&, const program_params&) {
	cout << "\033[38m" << getVersion() << "\033[0m" << endl;
}
