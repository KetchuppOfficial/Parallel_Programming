#ifndef INCLUDE_PROGRAM_OPTIONS_HPP
#define INCLUDE_PROGRAM_OPTIONS_HPP

#include <utility>
#include <string_view>

#include <boost/program_options.hpp>

namespace parallel
{

namespace po = boost::program_options;

auto set_program_options(int argc, char *argv[], std::string_view n_iter_desc)
    -> std::pair<po::options_description, po::variables_map>;

} // namespace parallel

#endif // INCLUDE_PROGRAM_OPTIONS_HPP
