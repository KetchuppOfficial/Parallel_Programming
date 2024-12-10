#ifndef INCLUDE_SIMPLE_MATRIX_HPP
#define INCLUDE_SIMPLE_MATRIX_HPP

#include <cstddef>
#include <vector>

namespace parallel
{

class SimpleMatrix final
{
public:

    SimpleMatrix(std::size_t n_rows, std::size_t n_cols)
        : data_(n_rows * n_cols), n_rows_{n_rows}, n_cols_{n_cols} {}

    template<typename Self>
    auto &&operator[](this Self &&self, std::size_t i, std::size_t j) noexcept
    {
        return self.data_[i * self.n_cols_ + j];
    }

    std::size_t n_rows() const noexcept { return n_rows_; }
    std::size_t n_cols() const noexcept { return n_cols_; }

private:

    std::vector<double> data_;
    std::size_t n_rows_;
    std::size_t n_cols_;
};

} // parallel

#endif // INCLUDE_SIMPLE_MATRIX_HPP
