//
// Created by Simon Cros on 12/01/2025.
//

#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/wait.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>

#include "executor.hpp"

static int setNonblocking(const int fd)
{
    const int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

auto Executor::execute(const std::string& program, ArgumentsIterator args) -> const std::string&
{
    m_output.clear();
    m_execArgs.clear();

    int stdoutPipe[2];
    int stderrPipe[2];
    if (pipe(stdoutPipe) == -1)
    {
        throw std::runtime_error("Failed to create pipe");
    }
    if (pipe(stderrPipe) == -1)
    {
        close(stdoutPipe[0]);
        close(stdoutPipe[1]);
        throw std::runtime_error("Failed to create pipe");
    }

    setNonblocking(stdoutPipe[0]);
    setNonblocking(stderrPipe[0]);

    const pid_t pid = fork();
    if (pid == -1)
    {
        throw std::runtime_error("Failed to fork");
    }

    if (pid == 0)
    {
        // Child process
        close(stdoutPipe[0]);
        dup2(stdoutPipe[1], STDOUT_FILENO);
        close(stdoutPipe[1]);

        dup2(stderrPipe[1], STDERR_FILENO);
        close(stderrPipe[0]);
        close(stderrPipe[1]);

        m_execArgs.push_back(const_cast<char*>(program.c_str()));
        while (const auto arg = args.next())
            m_execArgs.push_back(arg);
        m_execArgs.push_back(nullptr);

        execvp(program.c_str(), m_execArgs.data());
        perror("execvp failed");
        exit(1);
    }
    else
    {
        // Parent process
        close(stdoutPipe[1]);
        close(stderrPipe[1]);

        char buffer[4096];
        ssize_t bytesRead;

        pollfd pfds[] = {
            {stdoutPipe[0], POLLIN, {}},
            {stderrPipe[0], POLLIN, {}}
        };

        size_t lines = 0;
        bool timedOut = false;
        const auto startTime = std::chrono::steady_clock::now();
        while (true)
        {
            const auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count() > 100)
            {
                timedOut = true;
                kill(pid, SIGKILL);
                break;
            }

            const int events = poll(pfds, 2, 0);
            if (events == -1)
                break;
            if (events == 0)
                continue;

            if (pfds[0].revents & POLLIN)
            {
                if ((bytesRead = read(pfds[0].fd, buffer, sizeof(buffer))) > 0)
                {
                    lines += std::count(buffer, buffer + bytesRead, '\n');
                }
            }
            if (pfds[1].revents & POLLIN) {
                if ((bytesRead = read(pfds[1].fd, buffer, sizeof(buffer))) > 0)
                {
                    m_output.append(buffer, bytesRead);
                }
            }

            if (pfds[0].revents & (POLLHUP | POLLERR)) {
                break;
            }
        }

        close(stdoutPipe[0]);
        close(stderrPipe[0]);

        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        {
            std::cout << m_output << std::endl;
            args.reset();
            while (const auto arg = args.next())
                std::cout << arg << " ";
            std::cout << std::endl;

            throw std::runtime_error("Child process failed with exit code " + std::to_string(WEXITSTATUS(status)));
        }

        return m_output;
    }
}
