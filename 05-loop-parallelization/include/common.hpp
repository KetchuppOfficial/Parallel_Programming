#ifndef INCLUDE_COMMON_HPP
#define INCLUDE_COMMON_HPP

#include <cstddef>
#include <string>
#include <stdexcept>
#include <fstream>

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fmt/ostream.h>

#include "simple_matrix.hpp"

namespace parallel
{

inline auto init_a(std::size_t n_rows, std::size_t n_cols)
{
    parallel::SimpleMatrix a(n_rows, n_cols);

    for (auto i = 0uz; i != n_rows; ++i)
        for (auto j = 0uz; j != n_cols; ++j)
            a[i, j] = 10 * i + j;

    return a;
}

inline void print_result(const std::string &out_name, const parallel::SimpleMatrix &m)
{
    std::ofstream out{out_name};
    if (!out.is_open())
        throw std::runtime_error{fmt::format("could not open file {}", out_name)};

    for (auto i = 0uz; i != m.n_rows(); ++i)
        fmt::println(out, "{}", fmt::join(&m[i, 0], &m[i + 1, 0], ", "));
}

} // namespace parallel

#endif // INCLUDE_COMMON_HPP
