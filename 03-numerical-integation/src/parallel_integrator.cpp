#include <tuple>
#include <stack>
#include <vector>
#include <semaphore>
#include <numeric>
#include <thread>
#include <future>
#include <cassert>
#include <algorithm>
#include <utility>
#include <cstddef>
#include <functional>
#include <ranges>

#include "parallel_integrator.hpp"

namespace parallel
{

using segment = std::tuple<double,  // a
                           double,  // f_a
                           double,  // b
                           double,  // f_b
                           double>; // I_ab

std::stack<segment, std::vector<segment>> global_stack;

static std::binary_semaphore stack_sem{1};
static std::binary_semaphore job_present_sem{1};
static std::size_t n_active_threads = 0;

static constexpr std::size_t max_segments = 10;

double Parallel_Integrator::integrate(double a, double b, std::size_t n_threads) const
{
    double f_a = f_(a), f_b = f_(b);

    if (a == b)
        return 0.0;
    else if (a < b)
        global_stack.emplace(a, f_a, b, f_b, std::midpoint(f_a, f_b) * (b - a));
    else
        global_stack.emplace(b, f_b, a, f_a, std::midpoint(f_a, f_b) * (a - b));

    std::vector<std::thread> jobs;
    jobs.reserve(n_threads);

    std::vector<std::future<double>> futures;
    futures.reserve(n_threads);

    for (auto i : std::views::iota(0uz, n_threads))
    {
        std::packaged_task<double()> task{std::bind(&Parallel_Integrator::integration_job,
                                                    this, n_threads)};
        futures.emplace_back(task.get_future());
        jobs.emplace_back(std::move(task));
    }

    double I = std::accumulate(futures.begin(), futures.end(), 0.0, [](double num, auto &future)
    {
        return num + future.get();
    });

    for (auto &job : jobs)
        job.join();

    return (a < b) ? I : -I;
}

double Parallel_Integrator::integration_job(std::size_t n_threads) const
{
    for (double I = 0.0; ;)
    {
        job_present_sem.acquire();
        stack_sem.acquire();

        auto [a, f_a, b, f_b, I_ab] = global_stack.top();
        global_stack.pop();

        if (!global_stack.empty())
            job_present_sem.release();

        if (a < b)
        {
            n_active_threads++;
            stack_sem.release();
        }
        else
        {
            stack_sem.release();
            return I;
        }

        double partial_I = integrate_segment(a, f_a, b, f_b, I_ab);

        stack_sem.acquire();

        n_active_threads--;
        if (n_active_threads == 0 && global_stack.empty())
        {
            for (auto i : std::views::iota(0uz, n_threads))
                global_stack.emplace(1.0, NAN, 0.0, NAN, NAN);

            job_present_sem.release();
        }

        stack_sem.release();

        I += partial_I;
    }
}

double Parallel_Integrator::integrate_segment(double a, double f_a,
                                              double b, double f_b, double I_ab) const
{
    assert(a < b);

    double I = 0;

    std::stack<segment, std::vector<segment>> stack;
    while (true)
    {
        double c = std::midpoint(a, b);
        double f_c = f_(c);

        double I_ac = std::midpoint(f_a, f_c) * (c - a);
        double I_cb = std::midpoint(f_c, f_b) * (b - c);
        double I_acb = I_ac + I_cb;

        if (not_good_approximation_yet(I_ab, I_acb))
        {
            stack.emplace(a, f_a, c, f_c, I_ac);
            a = c;
            f_a = f_c;
            I_ab = I_cb;
        }
        else
        {
            I += I_acb;

            if (stack.empty())
                break;

            std::tie(a, f_a, b, f_b, I_ab) = stack.top();
            stack.pop();
        }

        if (stack.size() > max_segments && global_stack.empty())
        {
            stack_sem.acquire();

            if (global_stack.empty())
                job_present_sem.release();

            while (stack.size() > 1)
            {
                global_stack.emplace(stack.top());
                stack.pop();
            }

            stack_sem.release();
        }
    }

    return I;
}

} // namespace parallel
