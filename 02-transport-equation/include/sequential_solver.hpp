#ifndef INCLUDE_SEQUENTIAL_SOLVER_HPP
#define INCLUDE_SEQUENTIAL_SOLVER_HPP

#include <cstddef>
#include <stdexcept>

#include "solver_base.hpp"

namespace parallel
{

class Transport_Equation_Solver final : public Transport_Equation_Solver_Base
{
public:

    using Transport_Equation_Solver_Base::two_arg_func;
    using Transport_Equation_Solver_Base::one_arg_func;

    Transport_Equation_Solver(double a,
                              double t_1, double t_2, std::size_t N_t,
                              double x_1, double x_2, std::size_t N_x,
                              two_arg_func heterogeneity,
                              one_arg_func init_cond, one_arg_func boundary_cond)
        : Transport_Equation_Solver_Base{a,
                                         N_t, (t_2 - t_1) / (N_t - 1), N_x, (x_2 - x_1) / (N_x - 1),
                                         heterogeneity}
    {
        if (init_cond(x_1) != boundary_cond(t_1))
            throw std::invalid_argument{"Initial and boundary condition are not coordinated"};

        for (auto i = 0; i != grid_.x_size(); ++i)
            grid_[0, i] = init_cond(x_1 + i * h_);

        for (auto i = 1; i != grid_.t_size(); ++i)
            grid_[i, 0] = boundary_cond(t_1 + i * tau_);

        solve_sequential();
    }
};

} // namespace parallel

#endif // INCLUDE_SEQUENTIAL_SOLVER_HPP
