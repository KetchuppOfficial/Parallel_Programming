#include <cstddef>
#include <chrono>
#include <iostream>
#include <stdexcept>

#include <boost/mpi/communicator.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/program_options.hpp>

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;

    boost::mpi::environment env{argc, argv};
    boost::mpi::communicator world;

    if (world.size() < 2)
        throw std::runtime_error{"The number of nodes must be at least 2"};

    po::options_description desc{"Allowed options"};

    desc.add_options()
        ("help", "Produce help message")
        ("n-messages", po::value<std::size_t>(),
         "Set the number of messages to send from node 0 to node 1");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help"))
    {
        if (world.rank() == 0)
            std::cout << desc << std::endl;

        return 0;
    }

    std::size_t N;
    if (vm.count("n-messages"))
        N = vm["n-messages"].as<std::size_t>();
    else
    {
        if (world.rank() == 0)
            std::cout << "The number of messages is not set. Abort" << std::endl;

        return 0;
    }

    constexpr int tag = 0;

    if (world.rank() == 0)
    {
        auto start = std::chrono::high_resolution_clock::now();

        for (auto i = 0uz; i != N; ++i)
            world.send(1, tag, std::vector{3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5});

        auto finish = std::chrono::high_resolution_clock::now();

        using mcs = std::chrono::microseconds;
        std::cout << "Sending " << N << " instance of std::vector took "
                  << std::chrono::duration_cast<mcs>(finish - start).count() << " mcs" << std::endl;
    }
    else if (world.rank() == 1)
    {
        for (auto i = 0uz; i != N; ++i)
        {
            std::vector<int> pi;
            world.recv(0, tag, pi);
        }
    }

    return 0;
}
