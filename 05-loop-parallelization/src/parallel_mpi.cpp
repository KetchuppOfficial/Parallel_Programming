#include <cstddef>
#include <numeric>
#include <stdexcept>
#include <string>
#include <cmath>
#include <chrono>
#include <utility>
#include <vector>
#include <algorithm>
#include <iterator>
#include <exception>

#include <fmt/base.h>

#include <CLI/CLI.hpp>

#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/collectives.hpp>

#include "common.hpp"
#include "simple_matrix.hpp"

namespace
{

namespace mpi = boost::mpi;

auto balance_tasks(const mpi::communicator &world, std::size_t n_tasks)
    -> std::pair<std::size_t, std::size_t>
{
    if (n_tasks % world.size() == 0)
    {
        const auto count = n_tasks / world.size();
        return std::pair{count * world.rank(), count * (world.rank() + 1)};
    }

    std::vector<std::size_t> tasks(world.size());
    for (auto i = 0uz; n_tasks != 0; --n_tasks, ++i)
        ++tasks[i % world.size()];

    const auto from = std::accumulate(tasks.begin(), std::next(tasks.begin(), world.rank()), 0uz);

    return std::pair{from, from + tasks[world.rank()]};
}

using ms = std::chrono::milliseconds;

constexpr int kRoot = 0;
constexpr int kTag = 42;

auto reference(const mpi::communicator &world, std::size_t n_rows, std::size_t n_cols)
{
    auto [row_from, row_to] = balance_tasks(world, n_rows);

    const auto width = row_to - row_from;
    parallel::SimpleMatrix a{width, n_cols};

    for (auto i = row_from; i != row_to; ++i)
        for (auto j = 0uz; j != n_cols; ++j)
            a[i - row_from, j] = 10 * i + j;

    auto start = std::chrono::high_resolution_clock::now();

    for (auto i = 0uz; i != width; ++i)
        for (auto j = 0uz; j != n_cols; ++j)
            a[i, j] = std::sin(2 * a[i, j]);

    if (world.rank() == kRoot)
    {
        std::vector<double> result(n_rows * n_cols);
        mpi::gather(world, &a[0, 0], a.size(), result.data(), kRoot);

        auto finish = std::chrono::high_resolution_clock::now();
        return std::pair{std::move(result), std::chrono::duration_cast<ms>(finish - start).count()};
    }

    mpi::gather(world, &a[0, 0], a.size(), kRoot);

    auto finish = std::chrono::high_resolution_clock::now();
    return std::pair{std::vector<double>{}, std::chrono::duration_cast<ms>(finish - start).count()};
}

/*
 * (3, 0): a[3, 0] = f(a[0, 2])
 * (3, 1): a[3, 1] = f(a[0, 3])
 * (3, 2): a[3, 2] = f(a[0, 4]) <--- a[3, 2]: source
 *              ...
 * (6, 0): a[6, 0] = f(a[3, 2]) <--- a[3, 2]: sink
 * (6, 1): a[6, 1] = f(a[3, 3])
 * (6, 2): a[6, 2] = f(a[3, 4])
 *              ...
 *
 * D = (6, 0) - (3, 2) = (3, -2) ==> true dependency
 *
 * Outer loop can be parallelized with step of 3 iterations.
 * Inner loop can be parallelized without constraints.
 */
auto task1(const mpi::communicator &world, std::size_t n_rows, std::size_t n_cols)
{
    if (world.size() != 3)
        throw std::runtime_error{"Only 3 processes are supported for task 1"};

    std::size_t n_lines = [n_rows, &world]
    {
        auto x = n_rows / world.size();
        if (auto rem = n_rows % world.size(); rem == 0)
            return x;
        else if (rem == 1)
            return (world.rank() == 0) ? x + 1 : x;
        else
            return (world.rank() == 2) ? x : x + 1;
    }();

    parallel::SimpleMatrix a{n_lines, n_cols};
    for (auto i = 0; i != n_lines; ++i)
        for (auto j = 0uz; j != n_cols; ++j)
            a[i, j] = 10 * (i * 3 + world.rank()) + j;

    auto start = std::chrono::high_resolution_clock::now();

    for (auto i = 1uz; i != n_lines; ++i)
        for (auto j = 0uz; j != n_cols - 2; ++j)
            a[i, j] = std::sin(3 * a[i - 1, j + 2]);

    if (world.rank() == kRoot)
    {
        std::vector<double> from_1, from_2;

        world.recv(kRoot + 1, kTag, from_1);
        world.recv(kRoot + 2, kTag, from_2);

        std::vector<double> result(n_rows * n_cols);
        auto min_lines = from_2.size() / n_cols;
        for (auto i = 0; i != min_lines; ++i)
        {
            auto out = result.data() + i * 3 * n_cols;

            auto begin_0 = a.buffer().data() + i * n_cols;
            std::copy_n(begin_0, n_cols, out);

            auto begin_1 = from_1.data() + i * n_cols;
            std::copy_n(begin_1, n_cols, out + n_cols);

            auto begin_2 = from_2.data() + i * n_cols;
            std::copy_n(begin_2, n_cols, out + 2 * n_cols);
        }

        if (a.size() != from_2.size())
        {
            auto begin = a.buffer().data() + min_lines * n_cols;
            auto out = result.data() + min_lines * 3 * n_cols;
            std::copy_n(begin, n_cols, out);
        }

        if (from_1.size() != from_2.size())
        {
            auto begin = from_1.data() + min_lines * n_cols;
            auto out = result.data() + min_lines * 3 * n_cols + n_cols;
            std::copy_n(begin, n_cols, out);
        }

        auto finish = std::chrono::high_resolution_clock::now();
        return std::pair{std::move(result), std::chrono::duration_cast<ms>(finish - start).count()};
    }

    world.send(kRoot, kTag, a.buffer());
    auto finish = std::chrono::high_resolution_clock::now();

    return std::pair{std::vector<double>{}, std::chrono::duration_cast<ms>(finish - start).count()};
}

} // unnamed namespace

int main(int argc, char **argv) try
{
    mpi::environment env{argc, argv};
    mpi::communicator world;

    CLI::App app{"Parallel application for studying loop parallelization with MPI"};

    std::string out_name;
    app.add_option("-o,--output", out_name, "Path to a .txt file with results");

    bool print_time = false;
    app.add_flag("-t,--time", print_time, "Measure studied loop execution time");

    std::size_t n_rows;
    app.add_option("--n-rows", n_rows, "The number of rows in 2D array")
        ->default_val(5000);

    std::size_t n_cols;
    app.add_option("--n-cols", n_cols, "The number of columns in 2D array")
        ->default_val(5000);

    std::string mode;
    app.add_option("-m,--mode", mode, "Mode of execution")
        ->check(CLI::IsMember({"reference", "task1"}))
        ->default_val("reference");

    try
    {
        app.parse(argc, argv);
    }
    catch (const CLI::CallForHelp &h)
    {
        if (world.rank() == kRoot)
            fmt::println("{}", app.help());
        return h.get_exit_code();
    }
    catch (const CLI::ParseError &e)
    {
        return app.exit(e);
    }

    if (n_rows <= world.rank())
        return 0;

    auto [data, execution_time] = [n_rows, n_cols, &world, &mode]
    {
        if (mode == "reference")
            return reference(world, n_rows, n_cols);
        else
            return task1(world, n_rows, n_cols);
    }();

    if (world.rank() == kRoot)
    {
        if (print_time)
        {
            std::vector<ms::rep> time_arr(world.size());
            mpi::gather(world, execution_time, time_arr.data(), kRoot);
            fmt::println("Loop execution took {} ms", *std::ranges::max_element(time_arr));
        }
        if (!out_name.empty())
        {
            parallel::SimpleMatrix m{std::move(data), n_rows, n_cols};
            parallel::print_result(out_name, m);
        }
    } else if (print_time)
        mpi::gather(world, execution_time, kRoot);

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
