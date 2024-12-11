#include <cstddef>
#include <omp.h>
#include <string>
#include <cmath>
#include <chrono>

#include <fmt/base.h>

#include <CLI/CLI.hpp>

#include "common.hpp"
#include "simple_matrix.hpp"

namespace
{

using ms = std::chrono::milliseconds;

// D = (0, 0) ==> no dependency
auto reference(std::size_t n_rows, std::size_t n_cols)
{
    auto a = parallel::init_a(n_rows, n_cols);

    auto start = std::chrono::high_resolution_clock::now();

    std::size_t i;
    #pragma omp parallel for
    for (i = 0uz; i != n_rows; ++i)
        for (auto j = 0uz; j != n_cols; ++j)
            a[i, j] = std::sin(2 * a[i, j]);

    auto finish = std::chrono::high_resolution_clock::now();

    return std::pair{a, std::chrono::duration_cast<ms>(finish - start).count()};
}

/*
 * (0, 2): a[0, 2] = f(a[3, 0])
 * (0, 3): a[0, 3] = f(a[3, 1])
 * (0, 4): a[0, 4] = f(a[3, 2]) <--- a[3, 2] : sink
 *              ...
 * (1, 2): a[1, 2] = f(a[4, 0])
 * (1, 3): a[1, 3] = f(a[4, 1])
 * (1, 3): a[1, 4] = f(a[4, 2])
 *              ...
 * (2, 2): a[2, 2] = f(a[5, 0])
 * (2, 3): a[2, 3] = f(a[5, 1])
 * (2, 4): a[2, 4] = f(a[5, 2])
 *              ...
 * (3, 2): a[3, 2] = f(a[5, 0]) <--- a[3, 2] : source
 * (3, 3): a[3, 3] = f(a[5, 1])
 * (3, 4): a[3, 4] = f(a[5, 2])
 *              ...
 *
 * D = (0, 4) - (3, 2) = (-3, 2) ==> antidependency
 *
 * Outer loop can be parallelized with step of 3 iterations.
 * Inner loop can be parallelized without constraints.
 */
auto task2(std::size_t n_rows, std::size_t n_cols)
{
    auto a = parallel::init_a(n_rows, n_cols);

    auto start = std::chrono::high_resolution_clock::now();

    std::size_t i;
    #ifdef TRY_NESTED // disable by default because it's a pessimization
    #pragma omp parallel for num_threads(3) schedule(static, 1)
    #endif // TRY_NESTED
    for (i = 0uz; i != n_rows - 3; ++i)
    {
        std::size_t j;
        #pragma omp parallel for
        for (j = 2uz; j != n_cols; ++j)
            a[i, j] = std::sin(0.1 * a[i + 3, j - 2]);
    }

    auto finish = std::chrono::high_resolution_clock::now();

    return std::pair{a, std::chrono::duration_cast<ms>(finish - start).count()};
}

/*
 * The first loop doesn't contain dependencies.
 *
 * The second loop:
 * (5, 0) : a[5, 0] = f(a[5, 0])
 *          b[5, 0] = g(a[0, 2])
 * (5, 1) : a[5, 1] = f(a[5, 1])
 *          b[5, 1] = g(a[0, 3])
 * (5, 2) : a[5, 2] = f(a[5, 2]) <--- a[5, 2] : source
 *          b[5, 2] = g(a[0, 4])
 *              ...
 * (10, 0) : a[10, 0] = f(a[10, 0])
 *           b[10, 0] = g(a[5, 2]) <--- a[5, 2] : sink
 * (10, 1) : a[10, 1] = f(a[10, 1])
 *           b[10, 0] = g(a[5, 3])
 * (10, 2) : a[10, 2] = f(a[10, 2])
 *           b[10, 2] = g(a[5, 4])
 *              ...
 *
 * D = (10, 0) - (5, 2) = (5, -2) ==> true dependency
 *
 * Outer loop can be parallelized with step of 5 iterations.
 * Inner loop can be parallelized without constraints.
 */
auto task3(std::size_t n_rows, std::size_t n_cols)
{
    auto a = parallel::init_a(n_rows, n_cols);
    auto b = parallel::SimpleMatrix(n_rows, n_cols);

    auto start = std::chrono::high_resolution_clock::now();

    std::size_t i;
    #pragma omp parallel for
    for (i = 0uz; i != 5uz; ++i)
        for (auto j = 0uz; j != n_cols; ++j)
            a[i, j] = std::sin(0.005 * a[i, j]);

    #ifdef TRY_NESTED
    #pragma omp parallel for num_threads(5) schedule(static, 1)
    #endif // TRY_NESTED
    for (i = 5uz; i != n_rows; ++i)
    {
        std::size_t j;
        #pragma omp parallel for
        for (j = 0uz; j != n_cols - 2; ++j)
        {
            a[i, j] = std::sin(0.005 * a[i, j]);
            b[i, j] = 1.5 * a[i - 5, j + 2];
        }

        a[i, n_cols - 2] = std::sin(0.005 * a[i, n_cols - 2]);
        a[i, n_cols - 1] = std::sin(0.005 * a[i, n_cols - 1]);
    }

    auto finish = std::chrono::high_resolution_clock::now();

    return std::pair{b, std::chrono::duration_cast<ms>(finish - start).count()};
}

} // unnamed namespace

int main(int argc, char **argv) try
{
    CLI::App app{"Parallel application for studying loop parallelization with OpenMP"};

    std::string out_name;
    app.add_option("-o,--output", out_name, "Path to a .txt file with results");

    bool time = false;
    app.add_flag("-t,--time", time, "Measure studied loop execution time");

    std::size_t n_rows;
    app.add_option("--n-rows", n_rows, "The number of rows in 2D array")
        ->default_val(5000);

    std::size_t n_cols;
    app.add_option("--n-cols", n_cols, "The number of columns in 2D array")
        ->default_val(5000);

    std::string mode;
    app.add_option("-m,--mode", mode, "Mode of execution")
        ->check(CLI::IsMember({"reference", "task2", "task3"}))
        ->default_val("reference");

    CLI11_PARSE(app, argc, argv);

    auto [data, time_result] = [n_rows, n_cols, &mode]
    {
        if (mode == "reference")
            return reference(n_rows, n_cols);
        else if (mode == "task2")
            return task2(n_rows, n_cols);
        else
            return task3(n_rows, n_cols);
    }();

    if (time)
        fmt::println("Loop execution took {} ms", time_result);

    if (!out_name.empty())
        parallel::print_result(out_name, data);

    return 0;
}
catch (const std::exception &e)
{
    fmt::println(stderr, "Caught an instance of {}.\nwhat(): {}", typeid(e).name(), e.what());
    return 1;
}
catch (...)
{
    fmt::println(stderr, "Caught an unknown exception");
    return 1;
}
