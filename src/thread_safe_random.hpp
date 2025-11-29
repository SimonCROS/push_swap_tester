//
// Created by Simon Cros on 1/18/25.
//

#ifndef THREAD_SAFE_RANDOM_HPP
#define THREAD_SAFE_RANDOM_HPP
#include <mutex>
#include <random>

class ThreadSafeRandom
{
public:
    using seed_type = std::random_device::result_type;
    using result_type = std::mt19937::result_type;

private:
    seed_type m_seed;
    std::mt19937 m_gen;
    std::mutex m_mutex;

public:
    ThreadSafeRandom() : m_seed(std::random_device{}()), m_gen(m_seed)
    {
    }

    explicit ThreadSafeRandom(const seed_type seed) : m_seed(seed), m_gen(m_seed)
    {
    }

    [[nodiscard]] auto seed() const -> seed_type { return m_seed; }

    [[nodiscard]] auto operator()() -> result_type
    {
        std::scoped_lock lock(m_mutex);
        return m_gen();
    }
};

#endif //THREAD_SAFE_RANDOM_HPP
