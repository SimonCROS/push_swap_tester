//
// Created by scros on 11/29/25.
//

#ifndef ARGUMENTS_ITERATOR_HPP
#define ARGUMENTS_ITERATOR_HPP

class ArgumentsIterator
{
    friend class ArgumentsGenerator;

private:
    char* const m_begin;
    char* const m_end;
    char* m_pos;
    size_t m_step;

    ArgumentsIterator(char* begin, char* end, const size_t step) : m_begin(begin), m_end(end), m_pos(begin),
                                                                   m_step(step)
    {
    }

public:
    [[nodiscard]] auto begin() const -> const char* { return m_begin; }
    [[nodiscard]] auto end() const -> const char* { return m_end; }
    auto reset() -> void { m_pos = m_begin; }

    auto next() -> char*
    {
        if (m_pos >= m_end)
            return nullptr;

        char* tmp = m_pos;
        m_pos += m_step;
        return tmp;
    }
};

#endif //ARGUMENTS_ITERATOR_HPP
