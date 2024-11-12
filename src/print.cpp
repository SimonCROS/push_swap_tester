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
	return "Complexity 1.7.1 (2024-11-12)";
}

const string getUsage() {
	return "usage: ./complexity [-vh] [-s seed] [-f push_swap] [--sorted] numbers iterations [goal] [checker]";
}
