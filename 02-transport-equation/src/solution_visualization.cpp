#include <vector>
#include <format>
#include <cmath>

#include <matplot/matplot.h>

#include "solver_base.hpp"

namespace parallel
{

void plot_solution(const Transport_Equation_Solver_Base &solution, std::string_view heterogeneity,
                   std::function<double(double, double)> analytical_solution)
{
    double T = (solution.t_size() - 1) * solution.t_step();
    double X = (solution.x_size() - 1) * solution.x_step();

    auto [x, t] = matplot::meshgrid(matplot::linspace(0.0, X, solution.x_size()),
                                    matplot::linspace(0.0, T, solution.t_size()));

    std::vector<std::vector<double>> u(solution.t_size());
    auto u_numerical = u, delta_u = u;

    for (auto k = 0; k != solution.t_size(); ++k)
    {
        for (auto m = 0; m != solution.x_size(); ++m)
        {
            double numerical_value = solution[k, m];
            double value = analytical_solution(k * solution.t_step(), m * solution.x_step());

            u[k].push_back(value);
            u_numerical[k].push_back(numerical_value);
            delta_u[k].push_back(std::abs(value - numerical_value));
        }
    }

    matplot::sgtitle(std::format("du/dt {} {:.2f} * du/x = {}; t in [0; {:.2f}], x in [0; {:.2f}]",
                                 solution.parameter() > 0 ? '+' : '-',
                                 std::abs(solution.parameter()), heterogeneity, T, X));

    matplot::subplot(1, 3, 0);
    matplot::surf(x, t, u_numerical);
    matplot::title("Numerical solution");
    matplot::xlabel("x");
    matplot::ylabel("t");
    matplot::zlabel("u(t, x)");

    matplot::subplot(1, 3, 1);
    matplot::surf(x, t, u);
    matplot::title("Analytical solution");
    matplot::xlabel("x");
    matplot::ylabel("t");
    matplot::zlabel("u(t, x)");

    matplot::subplot(1, 3, 2);
    matplot::surf(x, t, delta_u);
    matplot::title("Residual");
    matplot::xlabel("x");
    matplot::ylabel("t");
    matplot::zlabel("u(t, x)");

    matplot::show();
}

} // namespace parallel
