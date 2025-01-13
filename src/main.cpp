//
// Created by Simon Cros on 12/01/2025.
//

// ReSharper disable CppPossiblyErroneousEmptyStatements

#include <iostream>
#include <atomic>
#include <mutex>
#include <thread>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <unistd.h>
#include <signal.h>

#include "complexity.hpp"
#include "executor.hpp"
#include "arguments_generator.hpp"

std::atomic_uint remaining{0};
std::atomic_bool stopWorkers{false};

std::mutex results_access;
results_t results{};

auto worker(const program_opts& opts, const program_params& params) -> void
{
    Executor executor;
    ArgumentsGenerator generator(params.numbers);

    while (!stopWorkers.load(std::memory_order_relaxed))
    {
        unsigned int current = remaining.load(std::memory_order_relaxed);
        while (current > 0 && !remaining.compare_exchange_weak(current, current - 1, std::memory_order_relaxed));

        if (current == 0)
            break;

        auto& buffer = executor.execute("/Users/simoncros/Projects/push_swap/push_swap",
                                        generator.generate(std::numeric_limits<int>::min(),
                                                           std::numeric_limits<int>::max()));
        // const auto lines = std::ranges::count(buffer, '\n');
        const auto lines = std::count(buffer.begin(), buffer.end(), '\n');
        if (lines >= std::numeric_limits<unsigned int>::max()) {
            throw std::runtime_error(
#ifdef USE_FORMAT
                std::format("push_swap printed more than {} lines", std::numeric_limits<unsigned int>::max())
#else
                std::string("push_swap printed more than ") + std::to_string(std::numeric_limits<unsigned int>::max()) + "lines"
#endif
            );
        }

        {
            std::scoped_lock lock(results_access);

            ++results.done;
            results.total += lines;
            results.results.push_back(lines);
            if (lines < results.best)
                results.best = lines;
            if (lines > results.worst)
                results.worst = lines;
            if (params.objective.has_value() && params.objective.value() < lines)
                ++results.aboveObjective;
        }
    }
}

auto monitor(const program_params& params) -> void
{
    if (!prettyPrint())
        return;

    while (!stopWorkers.load(std::memory_order_relaxed))
    {
        if (remaining.load(std::memory_order_relaxed) == 0)
            stopWorkers.store(true, std::memory_order_relaxed);

        {
            std::scoped_lock lock(results_access);
            printStatus(params, results);
        }
        std::cout << "\033[7A" << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

auto waitAllThreads(std::vector<std::thread>& threads) -> void
{
    for (auto& i : threads)
    {
        if (i.joinable())
            i.join();
    }
}

auto main(int argc, char* argv[]) -> int
{
    program_opts opts;
    program_params params;

    opts = getOptions(argc, argv);
    if (opts.help || opts.version)
    {
        if (opts.usage)
            std::cout << getUsage() << std::endl; // TODO return error
        else if (opts.version)
            std::cout << getVersion() << std::endl;
        else if (opts.help)
            std::cout << getHelp() << std::endl;
        return EXIT_SUCCESS;
    }

    params = getParameters(argc, argv); // TODO can throw if not an int
    if (opts.program.has_value())
        params.program = *opts.program;
    else if (isExecutable("../push_swap"))
        params.program = "../push_swap";
    else if (isExecutable("./push_swap"))
        params.program = "./push_swap";
    assertExecutable(params.program);

    remaining.store(params.iterations, std::memory_order_release);
    results.results.reserve(params.iterations);

    signal(SIGINT, [](int)
    {
        stopWorkers.store(true, std::memory_order_relaxed);
    });

    std::vector<std::thread> threads;
    threads.reserve(opts.threads);
    for (int i = 0; i < opts.threads; ++i)
        threads.emplace_back(worker, opts, params);

    hideCursor();
    printStart(opts, params);
    monitor(params);
    waitAllThreads(threads);
    printStatus(params, results);
    showCursor();

    if (results.aboveObjective > 0)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
