#include "complexity.hpp"

#include <iostream>

using namespace std;

void hideCursor() {
	cout << "\e[?25l";
}

void showCursor() {
	cout << "\e[?25h";
}

const string getVersion() {
	return "Complexity 1.1.0";
}

const string getUsage() {
	return "usage: ./complexity [-vhs] [-f push_swap] numbers iterations [goal] [checker]";
}

const string getHelp() {
	string help;

	help += "\033[1mNAME\033[0m\n";
	help += "     \033[1mcomplexity\033[0m -- lance un benchmark de votre push_swap\n";
	help += "\n";
	help += "\033[1mSYNOPSIS\033[0m\n";
	help += "     \033[1m./complexity\033[0m [\033[1m-hs\033[0m] [\033[1m-p\033[0m \033[4mfile\033[0m] \033[4mnumbers\033[0m \033[4miterations\033[0m [\033[4mgoal\033[0m] [\033[4mchecker\033[0m]\n";
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
	help += "     \033[1m-s\033[0m, \033[1m--sorted\033[0m\n";
	help += "             Envoie uniquement des nombres triés au programme.\n";
	help += "     \n";
	help += "     \033[1m-f\033[0m \033[4mpush_swap\033[0m, \033[1m--file\033[0m=\033[4mpush_swap\033[0m\n";
	help += "             Utilise \033[4mpush_swap\033[0m en tant qu'exécutable push_swap.\n";
	return help;
}

void print(program_params params, int done, int total, int best, int worst, int successful, int ok) {
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
