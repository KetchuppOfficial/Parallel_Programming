#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <limits>
#include <chrono>

#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>

#include "pi_computation.hpp"
#include "program_options.hpp"

using boost::multiprecision::cpp_rational;
using boost::multiprecision::cpp_dec_float_50;

int main(int argc, char *argv[])
{
    boost::mpi::environment env{argc, argv};
    boost::mpi::communicator world;

    auto [desc, vm] = parallel::set_program_options(argc, argv,
                                                    "Set the number of iterations per process");

    int rank = world.rank();

    if (vm.count("help"))
    {
        if (rank == 0)
            std::cout << desc << std::endl;

        return 0;
    }

    std::size_t per_process;
    if (vm.count("n-iterations"))
        per_process = vm["n-iterations"].as<std::size_t>();
    else
    {
        if (rank == 0)
            std::cout << "The number of iterations not set. Abort" << std::endl;

        return 0;
    }

    auto start = std::chrono::high_resolution_clock::now();

    std::size_t from = rank * per_process;
    std::size_t to = from + per_process;

    cpp_rational pi_part = parallel::compute_part_of_pi_series(from, to);

    constexpr int tag = 0;

    int size = world.size();
    unsigned current_size = size;
    unsigned current_rank = rank;
    for (int shift = 1; current_size > 1; shift *= 2)
    {
        if (current_rank % 2)
        {
            world.send(rank - shift, tag, pi_part);
            return 0;
        }
        else if (current_rank < current_size - 1)
        {
            cpp_rational another_pi_part;
            world.recv(rank + shift, tag, another_pi_part);

            pi_part += another_pi_part;
        }

        std::div_t res = std::div(current_size, 2);
        current_size = res.rem ? 1 + res.quot : res.quot;
        current_rank /= 2;
    }

    auto finish = std::chrono::high_resolution_clock::now();

    constexpr auto precision = std::numeric_limits<cpp_dec_float_50>::max_digits10;
    std::cout << std::setprecision(precision) << parallel::ratio_to_float(pi_part) << std::endl;

    using ms = std::chrono::milliseconds;
    auto exec_time = std::chrono::duration_cast<ms>(finish - start).count();
    std::cout << "Parallel computing on " << size << " nodes took: " << exec_time << " ms"
              << std::endl;

    return 0;
}
