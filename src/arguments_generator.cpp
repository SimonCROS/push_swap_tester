//
// Created by Simon Cros on 12/01/2025.
//

#include "arguments_generator.hpp"

#include <charconv>
#include <iostream>
#include <algorithm>

#include "thread_safe_random.hpp"

ArgumentsGenerator::ArgumentsGenerator(ThreadSafeRandom& random, const size_t numbersCount) : m_generator(random()), m_numbersCount(numbersCount)
{
    if (numbersCount > 1000000)
        throw std::invalid_argument("The maximum number count is 1'000'000");

    m_buffer = new char[m_numbersCount * lengthPerNumber];
    populate_unique_numbers();
}

ArgumentsGenerator::~ArgumentsGenerator()
{
    delete[] m_buffer;
}

auto ArgumentsGenerator::generate() -> ArgumentsIterator
{
    std::fill_n(m_buffer, m_numbersCount * lengthPerNumber, 0);

    std::shuffle(std::begin(m_numbers), std::end(m_numbers), m_generator);
    for (int i = 0; i < m_numbersCount; ++i)
    {
        const size_t buffer_offset = i * lengthPerNumber;
        std::to_chars(m_buffer + buffer_offset, m_buffer + buffer_offset + lengthPerNumber, m_numbers[i]);
    }

    return {m_buffer, m_buffer + (m_numbersCount * lengthPerNumber), lengthPerNumber};
}

auto ArgumentsGenerator::populate_unique_numbers() -> void
{
    constexpr int min = std::numeric_limits<int>::min();
    constexpr int max = std::numeric_limits<int>::max();

    m_numbers.reserve(m_numbersCount);
    m_numbers.clear();

    std::unordered_set<int> uniqueNumbers;
    uniqueNumbers.reserve(m_numbersCount);

    std::uniform_int_distribution<int> dice(min, max); // Uniform distribution in range [min, max]
    while (m_numbers.size() < m_numbersCount)
    {
        const int number = dice(m_generator);

        // Try to insert the number. If insertion succeeded, add it to the list
        if (uniqueNumbers.insert(number).second)
        {
            m_numbers.push_back(number);
        }
    }
}
