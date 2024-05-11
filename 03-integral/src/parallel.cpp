#include <iostream>
#include <cmath>
#include <chrono>
#include <format>

#include "parallel_integrator.hpp"

int main()
{
    parallel::Parallel_Integrator integrator{[](double x){ return std::sin(1 / x); }, 1e-8};

    constexpr double a = 0.0001, b = 0.1;
    constexpr std::size_t n_threads = 10;

    auto start = std::chrono::high_resolution_clock::now();
    double I = integrator.integrate(a, b, n_threads);
    auto finish = std::chrono::high_resolution_clock::now();

    using ms = std::chrono::milliseconds;

    std::cout << std::format("Computation on {} threads took ", n_threads)
              << std::chrono::duration_cast<ms>(finish - start).count() << " ms\n"
              << "    I = " << I << std::endl;

    return 0;
}
