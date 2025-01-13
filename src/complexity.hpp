#ifndef COMPLEXITY_H
#define COMPLEXITY_H

#include <string>
#include <optional>
#include <random>

#if __cpp_lib_format >= 201907L
#define USE_FORMAT
#include <format>
#endif

struct program_opts
{
    bool version;
    bool help;
    bool usage;
    uint8_t threads{6};
    std::optional<std::string> program;
    std::optional<std::string> output;
    std::optional<std::random_device::result_type> seed;
};

struct program_params
{
    std::string program;
    uint32_t numbers;
    uint32_t iterations;
    std::optional<int> objective;
    std::optional<std::string> checker;
};

struct results_t
{
    std::vector<uint32_t> results;
    uint32_t worst{0};
    uint32_t best{std::numeric_limits<uint32_t>::max()};
    uint64_t total{0};
    uint32_t done{0};
    uint32_t aboveObjective{0};
};

auto prettyPrint() -> bool;

auto hideCursor() -> void;
auto showCursor() -> void;
auto getVersion() -> std::string_view;
auto getUsage() -> std::string_view;
auto getHelp() -> std::string;
auto printStart(const program_opts& opts, const program_params& params) -> void;
auto printStatus(const program_params& params, const results_t& results) -> void;

auto isExecutable(const std::string& path) -> bool;
auto assertExecutable(const std::string& path) -> void;

auto getOptions(int& argc, char**& argv) -> program_opts;
auto getParameters(int argc, char** argv) -> program_params;

#endif
