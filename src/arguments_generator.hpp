//
// Created by Simon Cros on 12/01/2025.
//

#ifndef RANDOM_LIST_ENGINE_HPP
#define RANDOM_LIST_ENGINE_HPP

#include <vector>
#include <random>
#include <stdexcept>
#include <iostream>
#include <unordered_set>

class ArgumentsGenerator;

class ArgumentsIterator
{
    friend class ArgumentsGenerator;

private:
    char* const m_begin;
    char* const m_end;
    char* m_pos;
    size_t m_step;

    ArgumentsIterator(char* begin, char* end, const size_t step) : m_begin(begin), m_end(end), m_pos(begin), m_step(step)
    {
    }

public:
    [[nodiscard]] auto begin() const -> const char* { return m_begin; }
    [[nodiscard]] auto end() const -> const char* { return m_end; }

    auto next() -> char*
    {
        if (m_pos >= m_end)
            return nullptr;

        char* tmp = m_pos;
        m_pos += m_step;
        return tmp;
    }
};

class ArgumentsGenerator
{
private:
    const size_t m_numbersCount;

    std::random_device m_rd;
    std::mt19937 m_gen;
    std::unordered_set<int> m_numbers;
    char* m_buffer;

public:
    static constexpr size_t lengthPerNumber = 12; // length + null terminator

    explicit ArgumentsGenerator(size_t numbersCount);

    ~ArgumentsGenerator();

    auto generate(int min, int max) -> ArgumentsIterator;
};

#endif //RANDOM_LIST_ENGINE_HPP
