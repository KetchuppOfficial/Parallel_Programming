#include <cmath>
#include <numbers>
#include <chrono>
#include <iostream>
#include <optional>
#include <tuple>

#include <boost/program_options.hpp>

#include "sequential_solver.hpp"
#include "solution_visualization.hpp"
#include "analytical_solution.hpp"

static auto get_options(int argc, char *argv[])
    -> std::optional<std::tuple<std::size_t, std::size_t, bool>>
{
    namespace po = boost::program_options;

    po::options_description desc{"Allowed options"};
    desc.add_options()
        ("help", "Produce help message")
        ("t-dots", po::value<std::size_t>(), "Set the number of points on T axis of the grid")
        ("x-dots", po::value<std::size_t>(), "Set the number of points on X axis of the grid")
        ("plot", "Plot solution");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return std::nullopt;
    }

    std::size_t N_t;
    if (vm.count("t-dots"))
        N_t = vm["t-dots"].as<std::size_t>();
    else
    {
        std::cout << "The number of points on T axis is not set. Abort" << std::endl;
        return std::nullopt;
    }

    std::size_t N_x;
    if (vm.count("x-dots"))
        N_x = vm["x-dots"].as<std::size_t>();
    else
    {
        std::cout << "The number of points on X axis is not set. Abort" << std::endl;
        return std::nullopt;
    }

    bool plot = vm.count("plot");

    return std::tuple{N_t, N_x, plot};
}

int main(int argc, char *argv[])
{
    auto opts = get_options(argc, argv);
    if (!opts.has_value())
        return 0;

    auto [N_t, N_x, plot] = opts.value();

    auto start = std::chrono::high_resolution_clock::now();

    parallel::Transport_Equation_Solver solution
    {
        2.0 /* a */,
        0.0 /* t_1 */, 1.0 /* t_2 */, N_t /* N_t */,
        0.0 /* x_1 */, 1.0 /* x_2 */, N_x /* N_x */,
        [](double t, double x){ return x + t; },
        [](double x){ return std::cos(std::numbers::pi * x); },
        [](double t){ return std::exp(-t); },
        parallel::Transport_Equation_Solver_Base::Scheme::explicit_three_points
    };

    auto stop = std::chrono::high_resolution_clock::now();

    using mcs = std::chrono::microseconds;
    std::cout << "Sequential solving took: "
              << std::chrono::duration_cast<mcs>(stop - start).count() << " mcs" << std::endl;

    if (plot)
        parallel::plot_solution(solution, "x + t", parallel::analytical_solution);

    return 0;
}
