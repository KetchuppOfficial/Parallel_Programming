#ifndef INCLUDE_PROGRAM_OPTIONS_HPP
#define INCLUDE_PROGRAM_OPTIONS_HPP

#include <utility>

#include <boost/program_options.hpp>

namespace parallel
{

namespace po = boost::program_options;

std::pair<po::options_description, po::variables_map> set_program_options(int argc, char *argv[]);

} // namespace parallel

#endif // INCLUDE_PROGRAM_OPTIONS_HPP
