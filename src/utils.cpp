//
// Created by Simon Cros on 12/01/2025.
//

#include <string>
#include <stdexcept>
#include <unistd.h>
#include <sys/stat.h>

#include "complexity.hpp"

auto prettyPrint() -> bool
{
    return isatty(1);
}

auto isExecutable(const std::string& path) -> bool
{
    struct stat st{};

    return stat(path.c_str(), &st) >= 0 && (st.st_mode & S_IEXEC) == S_IEXEC && (st.st_mode & S_IFREG) == S_IFREG;
}

auto assertExecutable(const std::string& path) -> void
{
    if (!isExecutable(path))
        throw std::invalid_argument(path + " is not a valid file");
}

auto getStatus(const program_params& params, const results_t& results) -> status_t
{
    status_t status{};

    if (results.finished > 0)
    {
        status.underObjective = (results.finished - results.aboveObjective) * 100 / results.finished;
        status.percentDone = results.finished * 100 / params.iterations;

        if (results.finished > 1)
        {
            status.stddev = std::sqrt(results.m2 / (results.finished));
        }
    }

    return status;
}
