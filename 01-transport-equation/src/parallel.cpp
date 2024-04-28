#include <cmath>
#include <numbers>
#include <chrono>
#include <iostream>

#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>

#include "parallel_solver.hpp"
#include "solution_visualization.hpp"

int main()
{
    boost::mpi::environment env;
    boost::mpi::communicator world;

    auto start = std::chrono::high_resolution_clock::now();

    std::size_t N_x = world.size() * 100;

    parallel::Transport_Equation_PSolver solution
    {
        world, 2.0 /* a */,
        0.0 /* t_1 */, 1.0 /* T */, N_x * 2 /* N_t */,
        0.0 /* x_1 */, 1.0 /* X */, N_x - 1 /* N_x */,
        [](double t, double x){ return x + t; },
        [](double x){ return std::cos(std::numbers::pi * x); },
        [](double t){ return std::exp(-t); }
    };

    auto stop = std::chrono::high_resolution_clock::now();

    if (world.rank() == 0)
    {
        using mcs = std::chrono::microseconds;
        std::cout << "Parallel solving on " << world.size() << " nodes took: "
                  << std::chrono::duration_cast<mcs>(stop - start).count() << " mcs" << std::endl;
        #if 0
        plot_solution(solution, "x + t");
        #endif
    }

    return 0;
}
