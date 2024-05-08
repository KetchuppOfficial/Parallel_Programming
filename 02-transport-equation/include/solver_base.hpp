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
        : grid_{N_t, N_x}, a_{a}, tau_{t_step}, h_{x_step}, f_{heterogeneity}
    {
        if (t_step < 0)
            throw std::invalid_argument{"Left time boundary must be less then right boundary"};
        else if (x_step < 0)
            throw std::invalid_argument{"Left space boundary must be less then right boundary"};
        else if (N_t < 2)
            throw std::invalid_argument{"The number of segments on the T axis must be at least 2"};
        else if (N_x < 2)
            throw std::invalid_argument{"The number of segments on the X axis must be at least 2"};
    }

    const double &operator[](std::size_t k, std::size_t m) const { return grid_[k, m]; }

    std::size_t x_size() const noexcept { return grid_.x_size(); }
    std::size_t t_size() const noexcept { return grid_.t_size(); }

    double t_step() const noexcept { return tau_; }
    double x_step() const noexcept { return h_; }

    double parameter() const noexcept { return a_; }

protected:

    ~Transport_Equation_Solver_Base() = default;

    void solve_sequential()
    {
        if (a_ < 0 && -a_ * tau_ < h_)
            throw std::runtime_error{"The scheme is unstable for given parameters"};

        for (auto m = 1uz; m != grid_.x_size(); ++m)
            for (auto k = 0uz; k != grid_.t_size() - 1; ++k)
                implicit_left_corner(k, m);
    }

    /*
     *   +--+
     *      |
     *      +
     */
    void implicit_left_corner(std::size_t k, std::size_t m)
    {
        return implicit_left_corner(k, m, grid_[k + 1, m - 1]);
    }

    void implicit_left_corner(std::size_t k, std::size_t m, double leftmost)
    {
        grid_[k + 1, m] = (h_ * grid_[k, m] + tau_ * a_ * leftmost
                                            + tau_ * h_ * f_(k * tau_, m * h_)) / (h_ + tau_ * a_);
    }

    Grid grid_;
    double a_;
    double tau_;
    double h_;
    two_arg_func f_;
};

} // namespace parallel

#endif // INCLUDE_SOLVER_BASE
