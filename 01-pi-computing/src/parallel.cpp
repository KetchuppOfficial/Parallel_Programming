#include <cstddef>
#include <iostream>
#include <iomanip>
#include <limits>
#include <chrono>

#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>

#include "pi-computation.hpp"
#include "program_options.hpp"

using boost::multiprecision::cpp_rational;
using boost::multiprecision::cpp_dec_float_50;

int main(int argc, char *argv[])
{
    #ifndef BOOST_MPI_HAS_NOARG_INITIALIZATION
    static_assert(false, "Program requires command line arguments for boost::program_options");
    #endif

    auto [desc, vm] = parallel::set_program_options(argc, argv);

    boost::mpi::environment env;
    boost::mpi::communicator world;

    int rank = world.rank();

    if (vm.count("help"))
    {
        if (rank == 0)
            std::cout << desc << std::endl;

        return 0;
    }

    std::size_t per_process = 1000;
    if (vm.count("n-iterations"))
        per_process = vm["n-iterations"].as<std::size_t>();
    else
    {
        if (rank == 0)
            std::cout << "The number of iterations not set. Abort" << std::endl;

        return 0;
    }

    int size = world.size();

    auto start = std::chrono::high_resolution_clock::now();

    cpp_rational pi_part
        = parallel::compute_part_of_pi_series(rank * per_process, (rank + 1) * per_process);

    for (int denom = 2; denom <= size; denom *= 2)
    {
        if (rank % denom)
        {
            world.send(rank - denom / 2, 0, pi_part);
            return 0;
        }
        else
        {
            cpp_rational another_pi_part;
            world.recv(rank + denom / 2, 0, another_pi_part);

            pi_part += another_pi_part;
        }
    }

    auto finish = std::chrono::high_resolution_clock::now();

    using ms = std::chrono::milliseconds;
    auto exec_time = std::chrono::duration_cast<ms>(finish - start).count();

    constexpr auto precision = std::numeric_limits<cpp_dec_float_50>::max_digits10;
    std::cout << std::setprecision(precision) << parallel::ratio_to_float(pi_part) << std::endl;

    std::cout << "Parallel computing on " << size << " nodes took: " << exec_time << "ms"
              << std::endl;

    return 0;
}
