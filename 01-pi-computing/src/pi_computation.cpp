#include "pi_computation.hpp"

namespace parallel
{

using boost::multiprecision::cpp_rational;
using boost::multiprecision::cpp_int;
using boost::multiprecision::cpp_dec_float_50;

cpp_rational compute_part_of_pi_series(std::size_t from, std::size_t to)
{
    cpp_rational pi;

    for (std::size_t k = from; k != to; ++k)
    {
        cpp_rational r1{4, 8 * k + 1};
        cpp_rational r2{-2, 8 * k + 4};
        cpp_rational r3{-1, 8 * k + 5};
        cpp_rational r4{-1, 8 * k + 6};

        cpp_int denom = 1;
        denom <<= 4 * k;

        pi += (r1 + r2 + r3 + r4) / denom;
    }

    return pi;
}

cpp_dec_float_50 ratio_to_float(cpp_rational r)
{
    cpp_dec_float_50 num{boost::multiprecision::numerator(r)};
    cpp_dec_float_50 denom{boost::multiprecision::denominator(r)};

    return num / denom;
}

cpp_dec_float_50 compute_pi(std::size_t n_iterations)
{
    return ratio_to_float(compute_part_of_pi_series(0, n_iterations));
}

} // namespace parallel
