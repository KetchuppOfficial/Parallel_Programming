#include <iostream>
#include <format>
#include <cmath>
#include <numbers>

#include <matplot/matplot.h>

#include "solver.hpp"

static void plot_solution(const parallel::Transport_Equation_Solver &solution)
{
    double T = (solution.t_size() - 1) * solution.t_step();
    double X = (solution.x_size() - 1) * solution.x_step();

    auto [x, y] = matplot::meshgrid(matplot::linspace(0.0, T, solution.t_size()),
                                    matplot::linspace(0.0, X, solution.x_size()));

    std::vector<std::vector<double>> z(solution.x_size(), std::vector<double>(solution.t_size()));
    for (auto i = 0; i != solution.x_size(); ++i)
        for (auto j = 0; j != solution.t_size(); ++j)
            z[i][j] = solution[j, i];

    matplot::surf(x, y, z);

    matplot::sgtitle(std::format("du/dt + {:.2f} * du/x = {}; t in [0; {:.2f}], x in [0; {:.2f}]",
                                 solution.parameter(), "x + t", T, X));

    matplot::xlabel("t");
    matplot::ylabel("x");
    matplot::zlabel("u(t, x)");

    matplot::show();
}

int main()
{
    parallel::Transport_Equation_Solver solution
    {
        2.0 /* a */,
        1.0 /* T */, 300 /* N_t */,
        1.0 /* X */, 100 /* N_x */,
        [](double t, double x){ return x + t; },
        [](double x){ return std::cos(std::numbers::pi * x); },
        [](double t){ return std::exp(-t);}
    };

    plot_solution(solution);

    return 0;
}
