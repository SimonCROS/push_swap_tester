#ifndef COMPLEXITY_H
#define COMPLEXITY_H

#include <string>
#include <optional>
#include <random>

#include "thread_safe_random.hpp"

struct program_opts
{
    std::chrono::milliseconds timeout{500};
    std::optional<std::string> program;
    std::optional<std::string> output;
    std::optional<ThreadSafeRandom::seed_type> seed;
    uint32_t outputBufferSizeKiB{10240}; // 10MiB
    uint8_t threads{6};
    bool version;
    bool help;
    bool json;
    bool noJson;
};

struct program_params
{
    std::string program;
    uint32_t numbers;
    uint32_t iterations;
    std::optional<uint32_t> objective;
    std::optional<std::string> checker;
};

struct execution_result_t
{
    std::chrono::steady_clock::duration executionTime;
    uint32_t lines{0};
    uint8_t status{0};
    bool timedOut{false};
};

struct results_t
{
    char* outputBuffer{nullptr};
    uint64_t outputBufferSize{0};

    std::chrono::microseconds meanExecutionTime;
    double mean{0};
    double m2{0};

    uint32_t finished{0};

    uint32_t success{0};
    uint32_t error{0};
    uint32_t timedOut{0};

    uint32_t worst{0};
    uint32_t best{std::numeric_limits<uint32_t>::max()};

    uint32_t aboveObjective{0};
};

struct status_t
{
    double stddev;
    uint64_t underObjective;
    uint32_t percentDone;
};

auto prettyPrint() -> bool;

auto getStatus(const program_params& params, const results_t& results) -> status_t;

auto hideCursor() -> void;
auto showCursor() -> void;
auto getVersion() -> std::string_view;
auto getUsage() -> std::string_view;
auto getHelp() -> std::string;
auto printStart(const program_params& params, ThreadSafeRandom::seed_type seed) -> void;
auto printStatus(const program_params& params, const results_t& results) -> void;
auto printJson(const program_params& params, const results_t& results, ThreadSafeRandom::seed_type seed) -> void;

auto isExecutable(const std::string& path) -> bool;
auto assertExecutable(const std::string& path) -> void;

auto getOptions(int& argc, char**& argv) -> program_opts;
auto getParameters(int argc, char** argv) -> program_params;

#endif
