#ifndef INCLUDE_GRID_HPP
#define INCLUDE_GRID_HPP

#include <cstddef>
#include <vector>

namespace parallel
{

class Grid final
{
public:

    Grid(std::size_t N_t, std::size_t N_x) : storage_(N_t * N_x), N_t_{N_t}, N_x_{N_x} {}

    std::size_t t_size() const noexcept { return N_t_; }
    std::size_t x_size() const noexcept { return N_x_; }

    const std::vector<double> &storage() const { return storage_; }

    const double &operator[](std::size_t k, std::size_t m) const { return storage_[m * N_t_ + k]; }
    double &operator[](std::size_t k, std::size_t m) { return storage_[m * N_t_ + k]; }

    void swap(std::vector<double> &rhs, std::size_t N_t, std::size_t N_x)
    {
        storage_.swap(rhs);
        N_t_ = N_t;
        N_x_ = N_x;
    }

private:

    std::vector<double> storage_;
    std::size_t N_t_;
    std::size_t N_x_;
};

} // namespace parallel

#endif // INCLUDE_GRID_HPP
