#include <cstddef>
#include <iostream>
#include <iomanip>
#include <limits>
#include <chrono>

#include <boost/program_options.hpp>

#include "pi-computation.hpp"

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    po::options_description desc{"Allowed options"};

    desc.add_options()
        ("help", "produce help message")
        ("n-iterations", po::value<std::size_t>(), "set the number of iterations");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return 0;
    }

    std::size_t n_iterations;
    if (vm.count("n-iterations"))
        n_iterations = vm["n-iterations"].as<std::size_t>();
    else
    {
        std::cout << "The number of iterations not set. Abort" << std::endl;
        return 1;
    }

    auto start = std::chrono::high_resolution_clock::now();

    auto pi = parallel::compute_pi(n_iterations);

    auto finish = std::chrono::high_resolution_clock::now();

    constexpr auto max_precision = std::numeric_limits<decltype(pi)>::max_digits10;
    std::cout << std::setprecision(max_precision) << pi << std::endl;

    using ms = std::chrono::milliseconds;
    std::cout << "Computation time: " << std::chrono::duration_cast<ms>(finish - start).count()
              << " ms" << std::endl;

    return 0;
}
