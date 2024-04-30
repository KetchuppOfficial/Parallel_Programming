#include <cstddef>
#include <iostream>
#include <iomanip>
#include <limits>
#include <chrono>

#include "pi_computation.hpp"
#include "program_options.hpp"

int main(int argc, char *argv[])
{
    auto [desc, vm] = parallel::set_program_options(argc, argv, "Set the number of iterations");

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
    auto exec_time = std::chrono::duration_cast<ms>(finish - start).count();
    std::cout << "Sequential computing took: " << exec_time << " ms" << std::endl;

    return 0;
}
