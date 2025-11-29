//
// Created by Simon Cros on 12/01/2025.
//

#ifndef RANDOM_LIST_ENGINE_HPP
#define RANDOM_LIST_ENGINE_HPP

#include <unordered_set>
#include <memory_resource>

#include "arguments_iterator.hpp"

class ArgumentsGenerator;
class ThreadSafeRandom;

class ArgumentsGenerator
{
private:
    const size_t m_numbersCount;

    std::pmr::unsynchronized_pool_resource m_pool;
    std::pmr::unordered_set<int> m_numbers;
    char* m_buffer;

public:
    static constexpr size_t lengthPerNumber = 12; // length + null terminator

    explicit ArgumentsGenerator(size_t numbersCount);
    ~ArgumentsGenerator();

    auto generate(ThreadSafeRandom& random) -> ArgumentsIterator;
};

#endif //RANDOM_LIST_ENGINE_HPP
