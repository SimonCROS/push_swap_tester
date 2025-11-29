//
// Created by scros on 11/29/25.
//

#include <atomic>
#include <mutex>
#include <thread>
#include <charconv>
#include <cstring>

#include "worker.hpp"
#include "executor.hpp"
#include "arguments_generator.hpp"

extern std::atomic_uint remaining;
extern std::atomic_bool stopWorkers;
extern std::atomic_bool killed;
extern std::atomic_uint64_t outputBufferPos;
extern std::atomic_uint64_t outputBufferWritten;

extern std::mutex results_access;
extern results_t results;

constexpr char timeOutPrefix[] = "Timed out ";
constexpr char errorPrefix[] = "Exit with status ";
constexpr char successPrefix[] = "Succeeded ";

auto worker(const program_opts& opts, const program_params& params,
            const std::reference_wrapper<ThreadSafeRandom> random) -> void
{
    Executor executor;
    ArgumentsGenerator generator(params.numbers);

    while (!stopWorkers.load(std::memory_order_relaxed))
    {
        unsigned int current = remaining.load(std::memory_order_relaxed);
        while (current > 0 && !remaining.compare_exchange_weak(current, current - 1, std::memory_order_relaxed))
        {
        }

        if (current == 0)
            break;

        const auto result = executor.execute(params.program, opts, generator.generate(random));

        // If execution gets killed by a signal, result.status may not be 0, so results should be ignored as they can be invalid
        if (killed.load(std::memory_order_relaxed))
        {
            break;
        }

        bool isSuccess = true;
        {
            std::scoped_lock lock(results_access);

            ++results.finished;
            if (result.timedOut)
            {
                ++results.timedOut;
                isSuccess = false;
            }
            else if (result.status != 0)
            {
                ++results.error;
                isSuccess = false;
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

        if (!isSuccess && results.outputBufferSize > 0)
        {
            size_t writeSize = 0;
            const char* prefix = nullptr;
            size_t prefixLength = 0;
            size_t additionalLength = 0;

            if (results.timedOut)
            {
                prefixLength = sizeof(timeOutPrefix) - 1;
                prefix = timeOutPrefix;
            }
            else if (result.status != 0)
            {
                prefixLength = sizeof(errorPrefix) - 1;
                prefix = errorPrefix;
                additionalLength = 4; // to write exit status (max 255) and ' '
            }
            else
            {
                prefixLength = sizeof(successPrefix) - 1;
                prefix = successPrefix;
            }
            writeSize += prefixLength;
            writeSize += additionalLength;
            writeSize += generator.bufferSize();
            writeSize += 1; // newline

            const uint64_t writeIndex = outputBufferPos.fetch_add(writeSize, std::memory_order_relaxed);
            if (writeIndex + writeSize < results.outputBufferSize)
            {
                const char* printableArgs = generator.convert_last_buffer_to_printable();
                char * writePos = results.outputBuffer + writeIndex;

                std::memcpy(writePos, prefix, prefixLength);
                writePos += prefixLength;

                if (!result.timedOut && result.status != 0) // check if not timed out, this can change the status code
                {
                    char *numberEnd = std::to_chars(writePos, writePos + 4, result.status % 256).ptr;
                    writePos += 4;
                    std::fill(numberEnd, writePos, ' ');
                }

                std::memcpy(writePos, printableArgs, generator.bufferSize());
                writePos += generator.bufferSize();

                *writePos = '\n';

                outputBufferWritten.fetch_add(writeSize, std::memory_order_relaxed);
            }
        }
    }
}
