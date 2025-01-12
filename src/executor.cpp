//
// Created by Simon Cros on 12/01/2025.
//

#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include "executor.hpp"

auto Executor::execute(const std::string& program, ArgumentsIterator args) -> const std::string&
{
    m_output.clear();
    m_execArgs.clear();

    int pipefd[2]; // Pipe for communication between parent and child
    if (pipe(pipefd) == -1) {
        throw std::runtime_error("Failed to create pipe");
    }

    const pid_t pid = fork();
    if (pid == -1) {
        throw std::runtime_error("Failed to fork");
    }

    if (pid == 0) {
        // Child process
        close(pipefd[0]); // Close the read end of the pipe
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to the write end of the pipe
        dup2(pipefd[1], STDERR_FILENO); // Optionally redirect stderr too
        close(pipefd[1]); // Close the original write end of the pipe

        m_execArgs.push_back(const_cast<char*>(program.c_str()));
        while (const auto arg = args.next())
            m_execArgs.push_back(arg);
        m_execArgs.push_back(nullptr);

        execvp(program.c_str(), m_execArgs.data());

        // If execvp fails, terminate the child
        perror("execvp failed");
        exit(1);
    } else {
        // Parent process
        close(pipefd[1]); // Close the write end of the pipe

        char buffer[4096];
        ssize_t bytesRead;

        // Read the child's output from the pipe
        while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            m_output.append(buffer, bytesRead);
        }

        close(pipefd[0]); // Close the read end of the pipe

        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            std::cout << m_output << std::endl;
            throw std::runtime_error("Child process failed with exit code " + std::to_string(WEXITSTATUS(status)));
        }

        return m_output;
    }
}
