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
#include <csignal>

#include "complexity.hpp"
#include "executor.hpp"
#include "arguments_generator.hpp"
#include "thread_safe_random.hpp"

std::atomic_uint remaining{0};
std::atomic_bool stopWorkers{false};
std::atomic_bool killed{false};

std::mutex results_access;
results_t results{};

auto worker(const program_opts& opts, const program_params& params,
            const std::reference_wrapper<ThreadSafeRandom> random) -> void
{
    Executor executor;
    ArgumentsGenerator generator(params.numbers);

    while (!stopWorkers.load(std::memory_order_relaxed))
    {
        unsigned int current = remaining.load(std::memory_order_relaxed);
        while (current > 0 && !remaining.compare_exchange_weak(current, current - 1, std::memory_order_relaxed));

        if (current == 0)
            break;

        const auto result = executor.execute(params.program, opts, generator.generate(random));

        // If execution gets killed by a signal, result.status may not be 0, so results should be ignored as they can be invalid
        if (killed.load(std::memory_order_relaxed))
            break;

        {
            std::scoped_lock lock(results_access);

            ++results.finished;
            if (result.timedOut)
            {
                ++results.timedOut;
            }
            else if (result.status != 0)
            {
                ++results.error;
            }
            else
            {
                ++results.success;

                {
                    const double delta1 = result.lines - results.mean;
                    results.mean += delta1 / results.success;
                    const double delta2 = result.lines - results.mean;
                    results.m2 += delta1 * delta2;
                }

                {
                    const auto delta =
                        std::chrono::duration_cast<std::chrono::microseconds>(result.executionTime)
                        - results.meanExecutionTime;
                    results.meanExecutionTime += delta / results.success;
                }

                if (result.lines < results.best)
                    results.best = result.lines;
                if (result.lines > results.worst)
                    results.worst = result.lines;
                if (params.objective.has_value() && params.objective.value() < result.lines)
                    ++results.aboveObjective;
            }
        }
    }
}

auto monitor(const program_params& params) -> void
{
    while (!stopWorkers.load(std::memory_order_relaxed))
    {
        if (remaining.load(std::memory_order_relaxed) == 0)
            stopWorkers.store(true, std::memory_order_relaxed);

        {
            std::scoped_lock lock(results_access);
            printStatus(params, results);
        }
        std::cout << (params.objective.has_value() ? "\033[5A" : "\033[4A") << std::flush;
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

    try
    {
        opts = getOptions(argc, argv);
        if (opts.help || opts.version)
        {
            if (opts.version)
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
        else
            throw std::runtime_error("No valid push_swap executable found.");
        assertExecutable(params.program);
    }
    catch (const std::exception& ex)
    {
        if (ex.what() != nullptr)
        {
            std::cerr << ex.what() << '\n';
        }
        std::cerr << getUsage() << std::endl;
        return EXIT_FAILURE;
    }

    remaining.store(params.iterations, std::memory_order_release);

    signal(SIGINT, [](int)
    {
        killed.store(true, std::memory_order_relaxed);
        stopWorkers.store(true, std::memory_order_relaxed);
    });

    ThreadSafeRandom::seed_type seed;
    if (opts.seed)
        seed = *opts.seed;
    else
        seed = std::random_device{}();

    ThreadSafeRandom random(seed);
    std::vector<std::thread> threads;
    threads.reserve(opts.threads);
    for (int i = 0; i < opts.threads; ++i)
        threads.emplace_back(worker, opts, params, std::reference_wrapper(random));

    hideCursor();
    if (prettyPrint() && !opts.json)
    {
        printStart(opts, params, seed);
        monitor(params);
    }
    waitAllThreads(threads);
    if ((prettyPrint() || opts.noJson) && !opts.json)
        printStatus(params, results);
    else
        printJson(params, results, seed);
    showCursor();

    if (results.aboveObjective > 0 || results.error > 0 || results.timedOut > 0)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
