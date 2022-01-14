#include "complexity.hpp"

#include <stdexcept>

#include <sys/stat.h>

const string &assertExecutable(const string &path) {
	struct stat  st;

	if (stat(path.c_str(), &st) < 0 ||
		(st.st_mode  &S_IEXEC) == 0 ||
		(st.st_mode  &S_IFREG) == 0)
		throw invalid_argument(path + " is not a valid file");
	return path;
}
