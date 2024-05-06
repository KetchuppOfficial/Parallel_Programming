#include <iostream>
#include <cmath>
#include <chrono>

#include "sequential_integrator.hpp"

int main()
{
    parallel::Sequential_Integrator integrator{[](double x){ return std::sin(1 / x); }, 1e-8};

    constexpr double a = 0.0001, b = 0.1;

    auto start = std::chrono::high_resolution_clock::now();
    double I = integrator.integrate(a, b);
    auto finish = std::chrono::high_resolution_clock::now();

    using ms = std::chrono::milliseconds;

    std::cout << "Non-recursive computation took "
              << std::chrono::duration_cast<ms>(finish - start).count() << " ms\n"
              << "    I = " << I << std::endl;

    start = std::chrono::high_resolution_clock::now();
    I = integrator.integrate(a, b, parallel::Sequential_Integrator::recursive{});
    finish = std::chrono::high_resolution_clock::now();

    std::cout << "Recursive computation took "
              << std::chrono::duration_cast<ms>(finish - start).count() << " ms\n"
              << "    I = " << I << std::endl;

    return 0;
}
