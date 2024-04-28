#ifndef INCLUDE_SOLVER_BASE
#define INCLUDE_SOLVER_BASE

#include <cstddef>
#include <functional>

#include "grid.hpp"

namespace parallel
{

/*
 * Solves equation:
 * du/dt + a * du/dx = f(t, x), where u = u(t, x), x in (0; X), t in (0; T), a in R
 * u(0, x) = phi(x), x in [0; X]
 * u(t, 0) = psi(t), t in [0; T]
 */
class Transport_Equation_Solver_Base
{
protected:

    using two_arg_func = std::function<double(double, double)>;
    using one_arg_func = std::function<double(double)>;

public:

    Transport_Equation_Solver_Base(double a,
                                   std::size_t N_t, double t_step, std::size_t N_x, double x_step,
                                   two_arg_func heterogeneity)
        : grid_{N_t + 1, N_x + 1}, a_{a}, tau_{t_step}, h_{x_step}, f_{heterogeneity} {}

    const double &operator[](std::size_t k, std::size_t m) const { return grid_[k, m]; }

    std::size_t x_size() const noexcept { return grid_.x_size(); }
    std::size_t t_size() const noexcept { return grid_.t_size(); }

    double t_step() const noexcept { return tau_; }
    double x_step() const noexcept { return h_; }

    double parameter() const noexcept { return a_; }

protected:

    ~Transport_Equation_Solver_Base() = default;

    /*
     * to use on the right border
     *      +
     *      |
     *   +--+
     */
    void explicit_left_corner(std::size_t k, std::size_t m)
    {
        grid_[k + 1, m] = grid_[k, m] + (a_ * tau_ / h_) * (grid_[k, m - 1] - grid_[k, m])
                                      + tau_ * f_(k * tau_, m * h_);
    }

    /*
     * to compute the second layer
     *      +
     *      |
     *   +--+--+
     */
    void explicit_four_points(std::size_t k, std::size_t m)
    {
        explicit_four_points(k, m, grid_[k, m - 1], grid_[k, m + 1]);
    }

    void explicit_four_points(std::size_t k, std::size_t m, double left, double right)
    {
        grid_[k + 1, m] =
            grid_[k, m] + (a_ * tau_ / (2 * h_)) * (left - right)
                        + (a_ * tau_ * tau_ / (2 * h_ * h_)) * (right - 2 * grid_[k, m] + left)
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
    void cross(std::size_t k, std::size_t m)
    {
        cross(k, m, grid_[k, m - 1], grid_[k, m + 1]);
    }

    void cross(std::size_t k, std::size_t m, double left, double right)
    {
        grid_[k + 1, m] = grid_[k - 1, m] + (a_ * tau_ / h_) * (left - right)
                                          + 2 * tau_ * f_(k * tau_, m * h_);
    }

    Grid grid_;
    double a_;
    double tau_;
    double h_;
    two_arg_func f_;
};

} // namespace parallel

#endif // INCLUDE_SOLVER_BASE
