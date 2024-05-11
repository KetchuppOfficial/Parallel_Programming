#ifndef INCLUDE_PARALLEL_INTEGRATOR_HPP
#define INCLUDE_PARALLEL_INTEGRATOR_HPP

#include <functional>
#include <cstddef>
#include <cmath>

namespace parallel
{

class Parallel_Integrator final
{
public:

    Parallel_Integrator(std::function<double(double)> f, double epsilon)
        : f_{f}, epsilon_{epsilon} {}

    double integrate(double a, double b, std::size_t n_threads) const;

private:

    double integration_job(std::size_t n_threads) const;
    double integrate_segment(double a, double f_a, double b, double f_b, double I_ab) const;

    bool not_good_approximation_yet(double I_ab, double I_acb) const
    {
        return std::abs(I_ab - I_acb) > epsilon_ * std::abs(I_acb);
    }

    std::function<double(double)> f_;
    double epsilon_;
};

} // namespace parallel

#endif // INCLUDE_PARALLEL_INTEGRATOR_HPP
