//
// Created by Simon Cros on 12/01/2025.
//

#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/wait.h>
#include <csignal>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>

#include "executor.hpp"

#include "complexity.hpp"

static auto setNonblocking(const int fd) -> int
{
    const int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

auto Executor::execute(const std::string& program, const program_opts& opts, ArgumentsIterator args) -> execution_result_t
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

        const execution_result_t&& result = Executor::monitorChild(opts, stdoutPipe[0], stderrPipe[0], pid);

        close(stdoutPipe[0]);
        close(stderrPipe[0]);

        return result;
    }
}

auto Executor::monitorChild(const program_opts& opts, const int stdoutFd, const int stderrFd, const pid_t pid) -> execution_result_t
{
    using namespace std::chrono_literals;

    execution_result_t result{};

    char buffer[500];
    ssize_t bytesRead;

    pollfd pfds[] = {
        {stdoutFd, POLLIN, {}},
        {stderrFd, POLLIN, {}}
    };

    // ------------------------------------------------------------------------------------------------
    // Be very careful when modifying this code, the behavior changes depending on the operating system
    // ------------------------------------------------------------------------------------------------

    const auto startTime = std::chrono::steady_clock::now();
    bool hup = false;
    while (true)
    {
        if (std::chrono::steady_clock::now() - startTime > opts.timeout)
        {
            result.timedOut = true;
            kill(pid, SIGKILL);
            break;
        }

        const int events = poll(pfds, 2, 5);
        if (events == -1)
            break;

        if (pfds[0].revents & (POLLHUP | POLLERR))
        {
            hup = true;
        }

        if (pfds[0].revents & POLLIN)
        {
            if ((bytesRead = read(pfds[0].fd, buffer, sizeof(buffer))) > 0)
            {
                result.lines += std::count(buffer, buffer + bytesRead, '\n');
            }
            else if (hup)
            {
                break;
            }
        }
        // if (pfds[1].revents & POLLIN)
        // {
        //     if ((bytesRead = read(pfds[1].fd, buffer, sizeof(buffer))) > 0)
        //     {
        //         m_output.append(buffer, bytesRead);
        //     }
        // }

        if (hup && !(pfds[0].revents & POLLIN))
        {
            break;
        }
    }

    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    result.status = waitChildProcessCompletion(pid);
    result.executionTime = std::chrono::steady_clock::now() - startTime;

    return result;
}

auto Executor::waitChildProcessCompletion(const pid_t pid) -> int
{
    // Wait for the child process to finish
    int status;

    do
    {
        if (waitpid(pid, &status, 0) == -1)
            return -1;
    }
    while (!WIFEXITED(status));

    return WEXITSTATUS(status);

    // std::cout << m_output << std::endl;
    // args.reset();
    // while (const auto arg = args.next())
    //     std::cout << arg << " ";
    // std::cout << std::endl;
}
