#include <iostream>
#include <optional>
#include <cstddef>
#include <print>
#include <exception>
#include <chrono>

#include <omp.h>
#include <boost/program_options.hpp>

namespace
{

std::optional<std::pair<std::size_t, bool>> get_options(int argc, char **argv)
{
    namespace po = boost::program_options;

    po::options_description desc{"Allowed options"};

    std::size_t n_threads;
    bool parallel = false;
    desc.add_options()
        ("help", "Produce help message")
        ("N", po::value<std::size_t>(&n_threads)->required(), "The number of summands")
        ("parallel", po::bool_switch(&parallel), "Run parallel version of the program");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help"))
    {
        std::cout << desc;
        return std::nullopt;
    }

    po::notify(vm);

    return std::pair{n_threads, parallel};
}

} // unnamed namespace

int main(int argc, char **argv) try
{
    const auto maybe_pair = get_options(argc, argv);
    if (!maybe_pair.has_value())
        return 0;

    const auto &[N, parallel] = *maybe_pair;

    double result = 0.0;

    auto start = std::chrono::high_resolution_clock::now();
    if (parallel)
    {
        std::size_t i;
        #pragma omp parallel for reduction(+:result)
        for (i = N; i != 0; --i)
            result += 1.0 / i;
    }
    else
    {
        for (auto i = N; i != 0; --i)
            result += 1.0 / i;
    }
    auto end = std::chrono::high_resolution_clock::now();

    using mcs = std::chrono::microseconds;
    std::println("result: {}\ncalculation took {} microseconds",
                 result, std::chrono::duration_cast<mcs>(end - start).count());

    return 0;
}
catch (const std::exception &e)
{
    std::println(std::cerr, "Error: {}.", e.what());
    return 1;
}
catch (...)
{
    std::println(std::cerr, "Error: unknown exception caught.");
    return 1;
}
