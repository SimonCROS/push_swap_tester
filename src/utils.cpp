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
    double mean = 0;
    double stddev = 0;
    uint64_t underObjective = 0;
    uint32_t percentDone = 0;

    if (results.done > 0)
    {
        mean = static_cast<double>(results.total) / results.done;
        for (const uint32_t result : results.results)
            stddev += pow(mean - result, 2.0);
        stddev = sqrt(stddev / results.done);
        underObjective = (results.done - results.aboveObjective) * 100 / results.done;
        percentDone = results.done * 100 / params.iterations;
    }

    return {
        .mean = mean,
        .stddev = stddev,
        .underObjective = underObjective,
        .percentDone = percentDone,
    };
}
