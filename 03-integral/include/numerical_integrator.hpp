#ifndef INCLUDE_NUMERICAL_INTEGRATOR
#define INCLUDE_NUMERICAL_INTEGRATOR

#include <functional>
#include <cassert>
#include <numeric>
#include <cmath>
#include <stack>
#include <tuple>
#include <vector>

namespace parallel
{

class Numerical_Integrator final
{
public:

    struct recursive {};

    Numerical_Integrator(std::function<double(double)> f, double epsilon)
        : f_{f}, epsilon_{epsilon} {}

    double integrate(double a, double b) const
    {
        double f_a = f_(a), f_b = f_(b);

        return (a < b) ?  integrate_with_stack(a, f_a, b, f_b)
                       : -integrate_with_stack(b, f_b, a, f_b);
    }

    double integrate(double a, double b, recursive tag) const
    {
        double f_a = f_(a), f_b = f_(b);

        return (a < b) ?  integrate_recursive(a, f_a, b, f_b)
                       : -integrate_recursive(b, f_b, a, f_a);
    }

private:

    using segment = std::tuple<double,  // a
                               double,  // f_a
                               double,  // b
                               double,  // f_b
                               double>; // I_ab

    double integrate_with_stack(double a, double f_a, double b, double f_b) const
    {
        assert(a < b);

        double I = 0;
        double I_ab = std::midpoint(f_a, f_b) * (b - a);

        std::stack<segment, std::vector<segment>> stack;
        while (true)
        {
            double c = std::midpoint(a, b);
            double f_c = f_(c);

            double I_ac = std::midpoint(f_a, f_c) * (c - a);
            double I_cb = std::midpoint(f_c, f_b) * (b - c);
            double I_acb = I_ac + I_cb;

            if (std::abs(I_ab - I_acb) > epsilon_ * std::abs(I_acb))
            {
                stack.emplace(a, c, f_a, f_c, I_ac);
                a = c;
                f_a = f_c;
                I_ab = I_cb;
            }
            else
            {
                I += I_acb;

                if (stack.empty())
                    break;

                std::tie(a, b, f_a, f_b, I_ab) = stack.top();
                stack.pop();
            }
        }

        return I;
    }

    double integrate_recursive(double a, double f_a, double b, double f_b) const
    {
        assert(a < b);

        double c = std::midpoint(a, b);
        double f_c = f_(c);

        double I_ab = std::midpoint(f_a, f_b) * (b - a);
        double I_ac = std::midpoint(f_a, f_c) * (c - a);
        double I_cb = std::midpoint(f_c, f_b) * (b - c);
        double I_acb = I_ac + I_cb;

        if (std::abs(I_ab - I_acb) > epsilon_ * std::abs(I_acb))
            return integrate_recursive(a, f_a, c, f_c) + integrate_recursive(c, f_c, b, f_b);
        else
            return I_acb;
    }

    std::function<double(double)> f_;
    double epsilon_;
};

} // namespace parallel

#endif // INCLUDE_NUMERICAL_INTEGRATOR
