//
// Created by scros on 11/29/25.
//

#ifndef WORKER_HPP
#define WORKER_HPP

#include "complexity.hpp"

auto worker(const program_opts& opts, const program_params& params,
            std::reference_wrapper<ThreadSafeRandom> random) -> void;

#endif //WORKER_HPP
