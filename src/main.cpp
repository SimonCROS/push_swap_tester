//
// Created by Simon Cros on 12/01/2025.
//

// ReSharper disable CppPossiblyErroneousEmptyStatements

#include <iostream>
#include <atomic>
#include <mutex>
#include <thread>
#include <iomanip>
#include <csignal>

#include "complexity.hpp"
#include "thread_safe_random.hpp"
#include "worker.hpp"

std::atomic_uint remaining{0};
std::atomic_bool stopWorkers{false};
std::atomic_bool killed{false};
std::atomic_uint64_t outputBufferPos{0};
std::atomic_uint64_t outputBufferWritten{0};

std::mutex results_access;
results_t results{};

auto monitor(const program_params& params) -> void
{
    while (!stopWorkers.load(std::memory_order_relaxed))
    {
        if (remaining.load(std::memory_order_relaxed) == 0)
        {
            stopWorkers.store(true, std::memory_order_relaxed);
        }

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
            throw std::runtime_error("No valid push_swap executable found");
        assertExecutable(params.program);
    }
    catch (const std::exception& ex)
    {
        if (ex.what() != nullptr && *ex.what() != '\0')
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
    if (opts.seed.has_value())
        seed = *opts.seed;
    else
        seed = std::random_device{}();

    if (opts.output.has_value())
    {
        const uint64_t outputBufferSize{static_cast<uint64_t>(opts.outputBufferSizeKiB) * 1024ULL};
        results.outputBufferSize = outputBufferSize;
        results.outputBuffer = new char[outputBufferSize];
    }

    ThreadSafeRandom random(seed);
    std::vector<std::thread> threads;
    threads.reserve(opts.threads);
    for (int i = 0; i < opts.threads; ++i)
        threads.emplace_back(worker, opts, params, std::reference_wrapper(random));

    hideCursor();
    if (prettyPrint() && !opts.json)
    {
        printStart(params, seed);
        monitor(params);
    }
    waitAllThreads(threads);
    if ((prettyPrint() || opts.noJson) && !opts.json)
        printStatus(params, results);
    else
        printJson(params, results, seed);
    showCursor();

    if (results.outputBuffer != nullptr)
    {
        const uint64_t wantedBufferLength = outputBufferPos.load(std::memory_order_relaxed);
        const uint64_t finalBufferLength = outputBufferWritten.load(std::memory_order_relaxed);
        if (std::FILE* outputFile = std::fopen(opts.output->c_str(), "w"))
        {
            const size_t lengthWrittenToFile = std::fwrite(results.outputBuffer, 1, finalBufferLength, outputFile);
            if (lengthWrittenToFile != finalBufferLength)
            {
                std::perror("Error writing to file");
            }

            std::fclose(outputFile);

            if (wantedBufferLength != finalBufferLength)
            {
                std::cout << "Output buffer too small! " << (wantedBufferLength - finalBufferLength) <<
                    " bytes were truncated" << std::endl;
            }
        }
        else
        {
            std::perror("Failed opening the output file for writing");
        }
        delete[] results.outputBuffer;
    }

    if (results.aboveObjective > 0 || results.error > 0 || results.timedOut > 0)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
