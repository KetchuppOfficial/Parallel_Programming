#include <print>
#include <format>
#include <vector>
#include <utility>
#include <numeric>
#include <ranges>

#include <omp.h>

namespace
{

int get_absolute_tid(const std::vector<int> &n_threads, int local_tid)
{
    return std::accumulate(n_threads.begin(), std::prev(n_threads.end()), local_tid);
}

auto get_nested_info()
{
    const int current_level = omp_get_level();

    std::vector<int> n_threads(current_level + 1);
    n_threads[0] = 1;

    for (int l = 1; l != current_level + 1; ++l)
        n_threads[l] = n_threads[l - 1] * omp_get_team_size(l);

    int tid_on_current_level = 0;
    for (int l = 1; l != current_level; ++l)
    {
        tid_on_current_level +=
            n_threads[current_level] * omp_get_ancestor_thread_num(l) / n_threads[l];
    }
    tid_on_current_level += omp_get_thread_num();

    return std::pair{std::move(n_threads), tid_on_current_level};
}

void print_info()
{
    #pragma omp critical
    {
        auto [n_threads, local_tid] = get_nested_info();
        std::println("Level {}: thread {:2} of {:2}; absolute tid: {:2}",
                     omp_get_level(), local_tid, n_threads.back(),
                     get_absolute_tid(n_threads, local_tid));
    }
}

} // unnamed namespace

int main()
{
    #pragma omp parallel num_threads(2)
    {
        print_info();
        #pragma omp parallel num_threads(3)
        {
            print_info();
            #pragma omp parallel num_threads(4)
            {
                print_info();
            }
        }
    }

    return 0;
}
