#include <cmath>
#include <numbers>
#include <chrono>
#include <iostream>

#include <boost/program_options.hpp>

#include "sequential_solver.hpp"
#include "solution_visualization.hpp"

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;

    po::options_description desc{"Allowed options"};

    desc.add_options()
        ("help", "Produce help message")
        ("x-dots", po::value<std::size_t>(),
         "Set the number of points on X axis of the grid. "
         "The number of points on T axis of the grid is 2 times greater")
        ("plot", "Plot solution");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return 0;
    }

    std::size_t N_x;
    if (vm.count("x-dots"))
        N_x = vm["x-dots"].as<std::size_t>();
    else
    {
        std::cout << "The number of segments on X axis of the grid is not set. Abort" << std::endl;
        return 0;
    }

    auto start = std::chrono::high_resolution_clock::now();

    parallel::Transport_Equation_Solver solution
    {
        2.0 /* a */,
        0.0 /* t_1 */, 1.0 /* t_2 */, N_x * 2 - 1 /* N_t */,
        0.0 /* x_1 */, 1.0 /* x_2 */, N_x - 1/* N_x */,
        [](double t, double x){ return x + t; },
        [](double x){ return std::cos(std::numbers::pi * x); },
        [](double t){ return std::exp(-t); }
    };

    auto stop = std::chrono::high_resolution_clock::now();

    using mcs = std::chrono::microseconds;
    std::cout << "Sequential solving took: "
              << std::chrono::duration_cast<mcs>(stop - start).count() << " mcs" << std::endl;

    if (vm.count("plot"))
        plot_solution(solution, "x + t");

    return 0;
}
