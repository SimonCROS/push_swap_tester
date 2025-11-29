//
// Created by scros on 11/29/25.
//

#include <atomic>
#include <mutex>
#include <thread>

#include "worker.hpp"
#include "executor.hpp"
#include "arguments_generator.hpp"

extern std::atomic_uint remaining;
extern std::atomic_bool stopWorkers;
extern std::atomic_bool killed;

extern std::mutex results_access;
extern results_t results;

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
