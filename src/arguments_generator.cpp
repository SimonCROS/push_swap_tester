//
// Created by Simon Cros on 12/01/2025.
//

#include "arguments_generator.hpp"

#include <charconv>
#include <algorithm>

#include "thread_safe_random.hpp"

ArgumentsGenerator::ArgumentsGenerator(const size_t numbersCount) : m_numbersCount(numbersCount), m_numbers(&m_pool)
{
    if (numbersCount > 1000000)
        throw std::invalid_argument("The maximum number count is 1'000'000");

    m_buffer = new char[m_numbersCount * lengthPerNumber];
    m_numbers.reserve(m_numbersCount);
}

ArgumentsGenerator::~ArgumentsGenerator()
{
    delete[] m_buffer;
}

auto ArgumentsGenerator::generate(ThreadSafeRandom& random) -> ArgumentsIterator
{
    constexpr int min = std::numeric_limits<int>::min();
    constexpr int max = std::numeric_limits<int>::max();

    std::fill_n(m_buffer, m_numbersCount * lengthPerNumber, 0);

    std::mt19937 generator(random());
    std::uniform_int_distribution<int> dice(min, max); // Uniform distribution in range [min, max]

    size_t maxIter = m_numbersCount * 2;
    m_numbers.clear();
    while (m_numbers.size() < m_numbersCount && maxIter > 0)
    {
        --maxIter;

        const int number = dice(generator);

        // Try to insert the number. If insertion succeeds, add it to the buffer
        if (m_numbers.insert(number).second)
        {
            const size_t buffer_offset = (m_numbers.size() - 1) * lengthPerNumber;
            std::to_chars(m_buffer + buffer_offset, m_buffer + buffer_offset + lengthPerNumber, number);
        }
    }

    return {m_buffer, m_buffer + (m_numbersCount * lengthPerNumber), lengthPerNumber};
}
