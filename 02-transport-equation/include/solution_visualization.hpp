#ifndef INCLUDE_SOLUTION_VISUALIZATION
#define INCLUDE_SOLUTION_VISUALIZATION

#include <string_view>
#include <functional>

#include "solver_base.hpp"

namespace parallel
{

void plot_solution(const Transport_Equation_Solver_Base &solution, std::string_view heterogeneity,
                   std::function<double(double, double)> analytical_solution);

} // namespace parallel

#endif // INCLUDE_SOLUTION_VISUALIZATION
