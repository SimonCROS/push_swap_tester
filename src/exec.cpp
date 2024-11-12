#include "complexity.hpp"

#include <unistd.h>
#include <iostream>
#include <array>

#include <sys/wait.h>

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

string exec(char **argv, optional<string> input) {
	std::array<char, 128> buffer;
	string result;

	pipe(pipes[PARENT_READ_PIPE]);
	pipe(pipes[PARENT_WRITE_PIPE]);

	if(fork() == 0) {
		dup2(CHILD_READ_FD, STDIN_FILENO);
		dup2(CHILD_WRITE_FD, STDOUT_FILENO);
		dup2(CHILD_WRITE_FD, STDERR_FILENO);

		close(CHILD_READ_FD);
		close(CHILD_WRITE_FD);
		close(PARENT_READ_FD);
		close(PARENT_WRITE_FD);

		execv(argv[0], argv);
		exit(EXIT_FAILURE);
	} else {
		close(CHILD_READ_FD);
		close(CHILD_WRITE_FD);
		if (input.has_value())
			write(PARENT_WRITE_FD, input.value().c_str(), input.value().size());
		close(PARENT_WRITE_FD);

		int len = 0;
		while ((len = read(PARENT_READ_FD, buffer.data(), buffer.size() - 1)) > 0) {
			buffer[len] = 0;
			result += buffer.data();
		}
		close(PARENT_READ_FD);

		waitpid(-1, NULL, 0);
	}
	return result;
}
