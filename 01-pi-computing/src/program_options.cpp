#include <cstddef>

#include "program_options.hpp"

namespace parallel
{

auto set_program_options(int argc, char *argv[], std::string_view n_iter_desc)
    -> std::pair<po::options_description, po::variables_map>
{
    po::options_description desc{"Allowed options"};

    desc.add_options()
        ("help", "Produce help message")
        ("n-iterations", po::value<std::size_t>(), n_iter_desc.data());

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    return std::pair{desc, vm};
}

} // namespace parallel
