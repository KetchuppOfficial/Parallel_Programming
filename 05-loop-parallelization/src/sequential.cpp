#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#include <format>
#include <chrono>

#include <fmt/base.h>
#include <fmt/ranges.h>
#include <fmt/ostream.h>

#include <CLI/CLI.hpp>

namespace
{

constexpr std::size_t kXSize = 5000;
constexpr std::size_t kYSize = 5000;

auto init_a()
{
    std::vector<double> data(kXSize * kYSize);

    for (auto i = 0uz; i != kXSize; ++i)
        for (auto j = 0uz; j != kYSize; ++j)
            data[i * kYSize + j] = 10 * i + j;

    return data;
}

auto init_b() { return std::vector<double>(kXSize * kYSize); }

using ms = std::chrono::milliseconds;

auto reference()
{
    auto a = init_a();

    auto start = std::chrono::high_resolution_clock::now();

    for (auto i = 0uz; i != kXSize; ++i)
        for (auto j = 0uz; j != kYSize; ++j)
            a[i * kYSize + j] = std::sin(2 * a[i * kYSize + j]);

    auto finish = std::chrono::high_resolution_clock::now();

    return std::pair{a, std::chrono::duration_cast<ms>(finish - start).count()};
}

auto task1()
{
    auto a = init_a();

    auto start = std::chrono::high_resolution_clock::now();

    for (auto i = 3uz; i != kXSize; ++i)
        for (auto j = 0uz; j != kYSize - 2; ++j)
            a[i * kYSize + j] = std::sin(3 * a[(i - 3) * kYSize + (j + 2)]);

    auto finish = std::chrono::high_resolution_clock::now();

    return std::pair{a, std::chrono::duration_cast<ms>(finish - start).count()};
}

auto task2()
{
    auto a = init_a();

    auto start = std::chrono::high_resolution_clock::now();

    for (auto i = 0uz; i != kXSize - 3; ++i)
        for (auto j = 2uz; j != kYSize; ++j)
            a[i * kYSize + j] = std::sin(0.1 * a[(i + 3) * kYSize + (j - 2)]);

    auto finish = std::chrono::high_resolution_clock::now();

    return std::pair{a, std::chrono::duration_cast<ms>(finish - start).count()};
}

auto task3()
{
    auto a = init_a();
    auto b = init_b();

    auto start = std::chrono::high_resolution_clock::now();

    for (auto i = 0uz; i != kXSize; ++i)
        for (auto j = 0uz; j != kYSize; ++j)
            a[i * kYSize + j] = std::sin(0.005 * a[i * kYSize + j]);

    for (auto i = 5uz; i != kXSize; ++i)
        for (auto j = 0; j != kYSize - 2; ++j)
            b[i * kYSize + j] = 1.5 * a[(i - 5) * kYSize + (j + 2)];

    auto finish = std::chrono::high_resolution_clock::now();

    return std::pair{b, std::chrono::duration_cast<ms>(finish - start).count()};
}

void print_result(const std::string &out_name, const double *data)
{
    std::ofstream out{out_name};
    if (!out.is_open())
        throw std::runtime_error{std::format("could not open file {}", out_name)};

    for (auto i = 0uz; i != kXSize; ++i)
    {
        for (auto j = 0uz; j != kYSize; ++j)
        {
            auto *first = &data[i * kYSize];
            auto *last = &data[i * (kYSize + 1)];
            fmt::println(out, "{}", fmt::join(first, last, ", "));
        }
    }
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

    std::string mode;
    app.add_option("-m,--mode", mode, "Mode of execution")
        ->check(CLI::IsMember({"reference", "task1", "task2", "task3"}))
        ->default_val("reference");

    CLI11_PARSE(app, argc, argv);

    auto [data, time_result] = [&]
    {
        if (mode == "reference")
            return reference();
        else if (mode == "task1")
            return task1();
        else if (mode == "task2")
            return task2();
        else
            return task3();
    }();

    if (time)
        fmt::println("Loop execution took {} ms", time_result);

    if (!out_name.empty())
        print_result(out_name, data.data());

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
