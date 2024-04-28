#include <cmath>
#include <numbers>
#include <chrono>
#include <iostream>

#include "sequential_solver.hpp"
#include "solution_visualization.hpp"

int main()
{
    auto start = std::chrono::high_resolution_clock::now();

    parallel::Transport_Equation_Solver solution
    {
        2.0 /* a */,
        0.0 /* t_1 */, 1.0 /* t_2 */, 1200 /* N_t */,
        0.0 /* x_1 */, 1.0 /* x_2 */, 599 /* N_x */,
        [](double t, double x){ return x + t; },
        [](double x){ return std::cos(std::numbers::pi * x); },
        [](double t){ return std::exp(-t); }
    };

    auto stop = std::chrono::high_resolution_clock::now();

    using mcs = std::chrono::microseconds;
    std::cout << "Sequential solving took: "
              << std::chrono::duration_cast<mcs>(stop - start).count() << " mcs" << std::endl;

    #if 0
    plot_solution(solution, "x + t");
    #endif

    return 0;
}
