#ifndef INCLUDE_SOLVER_HPP
#define INCLUDE_SOLVER_HPP

#include <cstddef>
#include <functional>
#include <stdexcept>

#include "grid.hpp"

namespace parallel
{

/*
 * Solves equation:
 * du/dt + a * du/dx = f(t, x), where u = u(t, x), x in (0; X), t in (0; T), a in R
 * u(0, x) = phi(x), x in [0; X]
 * u(t, 0) = psi(t), t in [0; T]
 */
class Transport_Equation_Solver final
{
public:

    using two_arg_func = std::function<double(double, double)>;
    using one_arg_func = std::function<double(double)>;

    Transport_Equation_Solver(double a, double T, std::size_t N_t, double X, std::size_t N_x,
                              two_arg_func heterogeneity,
                              one_arg_func init_cond, one_arg_func boundary_cond)
        : grid_{N_t + 1, N_x + 1}, a_{a}, tau_{T / N_t}, h_{X / N_x}, f_{heterogeneity}
    {
        if (T < 0)
            throw std::invalid_argument{"Value of parameter T must be positive"};
        else if (X < 0)
            throw std::invalid_argument{"Value of parameter X must be positive"};
        else if (N_t < 2)
            throw std::invalid_argument{"The number of segments of the T axis must be at least 2"};
        else if (N_x < 2)
            throw std::invalid_argument{"The number of segments on the X axis must be at least 2"};
        else if (init_cond(0) != boundary_cond(0))
            throw std::invalid_argument{"Initial and boundary condition are not coordinated"};

        for (auto i = 0; i != grid_.x_size(); ++i)
            grid_[0, i] = init_cond(i * h_);

        for (auto i = 1; i != grid_.t_size(); ++i)
            grid_[i, 0] = boundary_cond(i * tau_);

        solve(a);
    }

    double operator[](std::size_t k, std::size_t m) const { return grid_[k, m]; }

    std::size_t x_size() const noexcept { return grid_.x_size(); }
    std::size_t t_size() const noexcept { return grid_.t_size(); }

    double t_step() const noexcept { return tau_; }
    double x_step() const noexcept { return h_; }

    double parameter() const noexcept { return a_; }

private:

    void solve(double a)
    {
        for (auto m = 1; m != grid_.x_size() - 1; ++m)
            explicit_four_points(0, m, a);

        explicit_left_corner(0, grid_.x_size() - 1, a);

        for (auto k = 1; k != grid_.t_size() - 1; ++k)
        {
            for (auto m = 1; m != grid_.x_size() - 1; ++m)
                cross(k, m, a);

            explicit_left_corner(k, grid_.x_size() - 1, a);
        }
    }

    /*
     * to use on the right border
     *      +
     *      |
     *   +--+
     */
    void explicit_left_corner(std::size_t k, std::size_t m, double a)
    {
        grid_[k + 1, m] = grid_[k, m] + (a * tau_ / h_) * (grid_[k, m - 1] - grid_[k, m])
                                      + tau_ * f_(k * tau_, m * h_);
    }

    /*
     * to compute the second layer
     *      +
     *      |
     *   +--+--+
     */
    void explicit_four_points(std::size_t k, std::size_t m, double a)
    {
        grid_[k + 1, m] =
            grid_[k, m] + (a * tau_ / (2 * h_)) * (grid_[k, m - 1] - grid_[k, m + 1])
                        + (a * tau_ * tau_ / (2 * h_ * h_))
                            * (grid_[k, m + 1] - 2 * grid_[k, m] + grid_[k, m - 1])
                        + tau_ * f_(k * tau_, m * h_);
    }

    /*
     * to use in ordinary cases
     *      +
     *      |
     *   +--+--+
     *      |
     *      +
     */
    void cross(std::size_t k, std::size_t m, double a)
    {
        grid_[k + 1, m] = grid_[k - 1, m] + (a * tau_ / h_) * (grid_[k, m - 1] - grid_[k, m + 1])
                                          + 2 * tau_ * f_(k * tau_, m * h_);
    }

    Grid grid_;
    const double a_;
    const double tau_;
    const double h_;
    two_arg_func f_;
};

} // namespace parallel

#endif // INCLUDE_SOLVER_HPP
