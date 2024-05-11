#include <iostream>
#include <cmath>
#include <chrono>
#include <utility>
#include <optional>

#include <boost/program_options.hpp>

#include "sequential_integrator.hpp"

static std::optional<std::pair<double, double>> get_options(int argc, char *argv[])
{
    namespace po = boost::program_options;

    po::options_description desc{"Allowed options"};
    desc.add_options()
        ("help", "Produce help message")
        ("from", po::value<double>(), "Set the lower limit of integration")
        ("to", po::value<double>(), "Set the upper limit of integration");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return std::nullopt;
    }

    double a;
    if (vm.count("from"))
        a = vm["from"].as<double>();
    else
    {
        std::cout << "The lower limit of integration is not set. Abort" << std::endl;
        return std::nullopt;
    }

    double b;
    if (vm.count("to"))
        b = vm["to"].as<double>();
    else
    {
        std::cout << "The upper limit of integration is not set. Abort" << std::endl;
        return std::nullopt;
    }

    return std::pair{a, b};
}

int main(int argc, char *argv[])
{
    auto opts = get_options(argc, argv);
    if (!opts.has_value())
        return 0;

    auto [a, b] = opts.value();

    parallel::Sequential_Integrator integrator{[](double x){ return std::sin(1 / x); }, 1e-8};

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
