#include <print>
#include <cstddef>

#include <omp.h>

int main()
{
    std::size_t x = 1;
    std::size_t i;
    #pragma omp parallel for ordered
    for (i = 0; i != omp_get_num_threads(); ++i)
    {
        #pragma omp ordered
        {
            std::println("thread {}, x = {}", omp_get_thread_num(), x);
            x *= 2;
        }
    }

    return 0;
}
