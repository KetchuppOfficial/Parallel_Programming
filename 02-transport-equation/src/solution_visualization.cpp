#include <vector>
#include <format>
#include <cmath>

#include <matplot/matplot.h>

#include "solver_base.hpp"

namespace parallel
{

void plot_solution(const Transport_Equation_Solver_Base &solution, std::string_view heterogeneity)
{
    double T = (solution.t_size() - 1) * solution.t_step();
    double X = (solution.x_size() - 1) * solution.x_step();

    auto [x, y] = matplot::meshgrid(matplot::linspace(0.0, X, solution.x_size()),
                                    matplot::linspace(0.0, T, solution.t_size()));

    std::vector<std::vector<double>> z(solution.t_size());
    for (auto i = 0; i != solution.t_size(); ++i)
        for (auto j = 0; j != solution.x_size(); ++j)
            z[i].push_back(solution[i, j]);

    matplot::surf(x, y, z);

    matplot::sgtitle(std::format("du/dt {} {:.2f} * du/x = {}; t in [0; {:.2f}], x in [0; {:.2f}]",
                                 solution.parameter() > 0 ? '+' : '-',
                                 std::abs(solution.parameter()), heterogeneity, T, X));

    matplot::xlabel("x");
    matplot::ylabel("t");
    matplot::zlabel("u(t, x)");

    matplot::show();
}

} // namespace parallel
