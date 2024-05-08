#include <cmath>
#include <numbers>
#include <chrono>
#include <iostream>

#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/program_options.hpp>

#include "parallel_solver.hpp"
#include "solution_visualization.hpp"

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;

    boost::mpi::environment env{argc, argv};
    boost::mpi::communicator world;

    po::options_description desc{"Allowed options"};

    desc.add_options()
        ("help", "Produce help message")
        ("t-dots", po::value<std::size_t>(), "Set the number of points on T axis of the grid.")
        ("x-dots-per-process", po::value<std::size_t>(),
         "Set the number of points on X axis of the grid for each process")
        ("plot", "Plot solution");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        if (world.rank() == 0)
            std::cout << desc << std::endl;

        return 0;
    }

    std::size_t N_t;
    if (vm.count("t-dots"))
        N_t = vm["t-dots"].as<std::size_t>();
    else
    {
        if (world.rank() == 0)
            std::cout << "The number of points on T axis is not set. Abort" << std::endl;

        return 0;
    }

    std::size_t N_x;
    if (vm.count("x-dots-per-process"))
        N_x = world.size() * vm["x-dots-per-process"].as<std::size_t>();
    else
    {
        if (world.rank() == 0)
            std::cout << "The number of points on X axis is not set. Abort" << std::endl;

        return 0;
    }

    auto start = std::chrono::high_resolution_clock::now();

    parallel::Transport_Equation_PSolver solution
    {
        world, 2.0 /* a */,
        0.0 /* t_1 */, 1.0 /* T */, N_t /* N_t */,
        0.0 /* x_1 */, 1.0 /* X */, N_x /* N_x */,
        [](double t, double x){ return x + t; },
        [](double x){ return std::cos(std::numbers::pi * x); },
        [](double t){ return std::exp(-t); }
    };

    auto stop = std::chrono::high_resolution_clock::now();

    if (world.rank() == 0)
    {
        using mcs = std::chrono::microseconds;
        std::cout << "Parallel solving on " << world.size()
                  << ((world.size() > 1) ? " nodes" : " node") << " took: "
                  << std::chrono::duration_cast<mcs>(stop - start).count() << " mcs" << std::endl;

        if (vm.count("plot"))
            plot_solution(solution, "x + t");
    }

    return 0;
}
