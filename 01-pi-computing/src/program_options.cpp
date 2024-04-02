#include <cstddef>

#include "program_options.hpp"

namespace parallel
{

std::pair<po::options_description, po::variables_map> set_program_options(int argc, char *argv[])
{
    po::options_description desc{"Allowed options"};

    desc.add_options()
        ("help", "produce help message")
        ("n-iterations", po::value<std::size_t>(), "set the number of iterations");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    return std::pair{desc, vm};
}

} // namespace parallel
