#ifndef INCLUDE_ANALYTICAL_SOLUTION_HPP
#define INCLUDE_ANALYTICAL_SOLUTION_HPP

#include <cmath>
#include <numbers>

namespace parallel
{

inline double analytical_solution(double t, double x)
{
    double u = (x + 2 * t) * (5 * x + 2 * t) / 32;

    if (x >= 2 * t)
        return u + std::cos(std::numbers::pi * (x - 2 * t)) - 5 * (x - 2 * t) * (x - 2 * t) / 32;
    else
        return u + std::exp(0.5 * x - t) - (x - 2 * t) * (x - 2 * t) / 32;
}

} // namespace parallel

#endif // INCLUDE_ANALYTICAL_SOLUTION_HPP
