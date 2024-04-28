#ifndef INCLUDE_SOLUTION_VISUALIZATION
#define INCLUDE_SOLUTION_VISUALIZATION

#include <string_view>

#include "solver_base.hpp"

namespace parallel
{

void plot_solution(const Transport_Equation_Solver_Base &solution, std::string_view heterogeneity);

} // namespace parallel

#endif // INCLUDE_SOLUTION_VISUALIZATION
