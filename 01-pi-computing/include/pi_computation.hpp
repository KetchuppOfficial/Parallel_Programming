#ifndef INCLUDE_PI_COMPUTATION_HPP
#define INCLUDE_PI_COMPUTATION_HPP

#include <cstddef>

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>

namespace parallel
{

boost::multiprecision::cpp_dec_float_50 compute_pi(std::size_t n_iterations);
boost::multiprecision::cpp_rational compute_part_of_pi_series(std::size_t from, std::size_t to);
boost::multiprecision::cpp_dec_float_50 ratio_to_float(boost::multiprecision::cpp_rational r);

} // namespace parallel

#endif // INCLUDE_PI_COMPUTATION_HPP
