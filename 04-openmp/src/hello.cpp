#include <print>

#include <omp.h>

int main()
{
    #pragma omp parallel
    {
        std::println("Hello from thread {} of {}", omp_get_thread_num(), omp_get_num_threads());
    }

    return 0;
}
