//
// Created by Simon Cros on 12/01/2025.
//

// ReSharper disable CppPossiblyErroneousEmptyStatements

#include <iostream>
#include <atomic>
#include <thread>
#include <format>
#include <limits>

#include "executor.hpp"
#include "arguments_generator.hpp"

std::atomic_uint worst{0};
std::atomic_uint best{0};
std::atomic_uint deviation{0};
std::atomic_uint done{0};
std::atomic_uint remaining{10000};

auto worker() -> void
{
    Executor executor;
    ArgumentsGenerator generator(100);

    while (true)
    {
        unsigned int current = remaining.load(std::memory_order_relaxed);
        while (current > 0 && !remaining.compare_exchange_weak(current, current - 1, std::memory_order_relaxed));

        if (current == 0)
            break;

        auto& buffer = executor.execute("/Users/simoncros/Projects/push_swap/push_swap", generator.generate(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()));
        std::cout << std::ranges::count(buffer, '\n') << std::endl;
    }
}

auto main(int argc, char* argv[]) -> int
{
    std::thread threads[5];
    for (auto& i : threads)
        i = std::thread(worker);
    for (auto& i : threads)
        i.join();
}
