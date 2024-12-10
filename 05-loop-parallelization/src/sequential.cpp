#include <cstddef>
#include <stdexcept>
#include <string>
#include <cmath>
#include <fstream>
#include <chrono>

#include <fmt/base.h>
#include <fmt/ranges.h>
#include <fmt/ostream.h>

#include <CLI/CLI.hpp>

#include "simple_matrix.hpp"

namespace
{

auto init_a(std::size_t n_rows, std::size_t n_cols)
{
    parallel::SimpleMatrix a(n_rows, n_cols);

    for (auto i = 0uz; i != n_rows; ++i)
        for (auto j = 0uz; j != n_cols; ++j)
            a[i, j] = 10 * i + j;

    return a;
}

using ms = std::chrono::milliseconds;

auto reference(std::size_t n_rows, std::size_t n_cols)
{
    auto a = init_a(n_rows, n_cols);

    auto start = std::chrono::high_resolution_clock::now();

    for (auto i = 0uz; i != n_rows; ++i)
        for (auto j = 0uz; j != n_cols; ++j)
            a[i, j] = std::sin(2 * a[i, j]);

    auto finish = std::chrono::high_resolution_clock::now();

    return std::pair{a, std::chrono::duration_cast<ms>(finish - start).count()};
}

auto task1(std::size_t n_rows, std::size_t n_cols)
{
    auto a = init_a(n_rows, n_cols);

    auto start = std::chrono::high_resolution_clock::now();

    for (auto i = 3uz; i != n_rows; ++i)
        for (auto j = 0uz; j != n_cols - 2; ++j)
            a[i, j] = std::sin(3 * a[i - 3, j + 2]);

    auto finish = std::chrono::high_resolution_clock::now();

    return std::pair{a, std::chrono::duration_cast<ms>(finish - start).count()};
}

auto task2(std::size_t n_rows, std::size_t n_cols)
{
    auto a = init_a(n_rows, n_cols);

    auto start = std::chrono::high_resolution_clock::now();

    for (auto i = 0uz; i != n_rows - 3; ++i)
        for (auto j = 2uz; j != n_cols; ++j)
            a[i, j] = std::sin(0.1 * a[i + 3, j - 2]);

    auto finish = std::chrono::high_resolution_clock::now();

    return std::pair{a, std::chrono::duration_cast<ms>(finish - start).count()};
}

auto task3(std::size_t n_rows, std::size_t n_cols)
{
    auto a = init_a(n_rows, n_cols);
    auto b = parallel::SimpleMatrix(n_rows, n_cols);

    auto start = std::chrono::high_resolution_clock::now();

    for (auto i = 0uz; i != n_rows; ++i)
        for (auto j = 0uz; j != n_cols; ++j)
            a[i, j] = std::sin(0.005 * a[i, j]);

    for (auto i = 5uz; i != n_rows; ++i)
        for (auto j = 0; j != n_cols - 2; ++j)
            b[i, j] = 1.5 * a[i - 5, j + 2];

    auto finish = std::chrono::high_resolution_clock::now();

    return std::pair{b, std::chrono::duration_cast<ms>(finish - start).count()};
}

void print_result(const std::string &out_name, const parallel::SimpleMatrix &m)
{
    std::ofstream out{out_name};
    if (!out.is_open())
        throw std::runtime_error{fmt::format("could not open file {}", out_name)};

    for (auto i = 0uz; i != m.n_rows(); ++i)
        fmt::println(out, "{}", fmt::join(&m[i, 0], &m[i + 1, 0], ", "));
}

} // unnamed namespace

int main(int argc, char **argv) try
{
    CLI::App app{"Application for studying loop parallelization"};

    std::string out_name;
    app.add_option("-o,--output", out_name, "Path to a .txt file with results")
        ->check(CLI::ExistingFile);

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
        ->check(CLI::IsMember({"reference", "task1", "task2", "task3"}))
        ->default_val("reference");

    CLI11_PARSE(app, argc, argv);

    auto [data, time_result] = [n_rows, n_cols, &mode]
    {
        if (mode == "reference")
            return reference(n_rows, n_cols);
        else if (mode == "task1")
            return task1(n_rows, n_cols);
        else if (mode == "task2")
            return task2(n_rows, n_cols);
        else
            return task3(n_rows, n_cols);
    }();

    if (time)
        fmt::println("Loop execution took {} ms", time_result);

    if (!out_name.empty())
        print_result(out_name, data);

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
