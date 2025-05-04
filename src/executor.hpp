//
// Created by Simon Cros on 12/01/2025.
//

#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

#include <string>
#include <vector>

#include "arguments_generator.hpp"
#include "complexity.hpp"

class Executor
{
private:
    std::string m_output;
    std::vector<char*> m_execArgs; // Cache to avoid new vector allocation

public:
    Executor() = default;

    explicit Executor(const size_t args)
    {
        m_execArgs.reserve(args + 2); // + 1 for program, + 1 for null termination
    }

    auto execute(const std::string& program, const program_opts& opts, ArgumentsIterator args) -> execution_result_t;

private:
    auto monitorChild(const program_opts& opts, int stdoutFd, int stderrFd, pid_t pid) -> execution_result_t;
    static auto waitChildProcessCompletion(pid_t pid) -> int;
};

#endif //EXECUTOR_HPP
