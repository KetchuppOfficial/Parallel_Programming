#ifndef INCLUDE_MATRIX_HPP
#define INCLUDE_MATRIX_HPP

#include <vector>
#include <iterator>
#include <algorithm>
#include <type_traits>
#include <stdexcept>
#include <utility>

#ifdef _OPENMP
#include <omp.h>
#endif // _OPENMP

namespace parallel
{

template<typename T>
class Matrix final : private std::vector<T>
{
    using vector = std::vector<T>;
    using vector::reserve;
    using vector::operator[];

public:

    using typename vector::size_type;
    using typename vector::value_type;
    using vector::size;

    Matrix(std::size_t n_rows, std::size_t n_cols)
        : vector(n_rows * n_cols), n_rows_{n_rows}, n_cols_{n_cols} {}

    template<std::input_iterator It>
    Matrix(std::size_t n_rows, std::size_t n_cols, It first, It last)
        : Matrix(n_rows, n_cols)
    {
        for (size_type i = 0; first != last && i != size(); ++first, ++i)
            (*this)[i] = *first;
    }

    Matrix(const Matrix &rhs) = default;
    Matrix &operator=(const Matrix &rhs) = default;

    Matrix(Matrix &&rhs) noexcept
        : vector(std::move(rhs)),
          n_rows_{std::exchange(rhs.n_rows_, 0)},
          n_cols_{std::exchange(rhs.n_cols_, 0)} {}

    Matrix &operator=(Matrix &&rhs) noexcept
    {
        vector::operator=(std::move(rhs));
        n_rows_ = rhs.n_rows_;
        n_cols_ = rhs.n_cols_;
    }

    void swap(Matrix &rhs) noexcept(std::is_nothrow_swappable_v<vector>)
    {
        vector::swap(rhs);
        std::swap(n_rows_, rhs.n_rows_);
        std::swap(n_cols_, rhs.n_cols_);
    }

    size_type n_rows() const noexcept { return n_rows_; }
    size_type n_cols() const noexcept { return n_cols_; }
    bool is_square() const noexcept { return n_rows_ == n_cols_; }

    template<typename Self>
    auto &&operator[](this Self &&self, size_type i, size_type j) noexcept
    {
        return self[i * self.n_cols() + j];
    }

    template<typename Self>
    auto &&at(this Self &&self, size_type i, size_type j)
    {
        if (i >= self.n_rows() || j >= self.n_cols())
            throw std::out_of_range{"Attempt to access matrix element that is out of range"};
        return self[i, j];
    }

    Matrix &transpose() &
    {
        if (is_square())
        {
            for (auto i = 0; i != n_rows_; ++i)
                for (auto j = i + 1; j != n_cols_; ++j)
                    std::swap((*this)[i, j], (*this)[j, i]);
        }
        else
        {
            Matrix transposed{n_cols_, n_rows_};

            for (auto i = 0; i != n_rows_; ++i)
                for (auto j = 0; j != n_cols_; ++j)
                    transposed[j, i] = (*this)[i, j];

            swap(transposed);
        }

        return *this;
    }

private:

    size_type n_rows_;
    size_type n_cols_;
};

template<std::input_iterator It>
Matrix(std::size_t n_rows, std::size_t n_cols, It first, It last)
    -> Matrix<typename std::iterator_traits<It>::value_type>;

template<typename T>
Matrix<T> naive_product(const Matrix<T> &lhs, const Matrix<T> &rhs)
{
    using size_type = typename Matrix<T>::size_type;

    if (lhs.n_cols() != rhs.n_rows())
        throw std::invalid_argument("Product of matrices of given sizes if undefined");

    Matrix<T> product{lhs.n_rows(), rhs.n_cols()};
    const size_type dim = lhs.n_cols();

    size_type i;
    #pragma omp parallel for
    for (i = 0; i != lhs.n_rows(); ++i)
    {
        size_type j;
        #pragma omp parallel for
        for (j = 0; j != rhs.n_cols(); ++j)
        {
            for (size_type k = 0; k != dim; ++k)
                product[i, j] += lhs[i, k] * rhs[k, j];
        }
    }

    return product;
}

template<typename T>
Matrix<T> transpose_product(const Matrix<T> &lhs, Matrix<T> rhs)
{
    using size_type = typename Matrix<T>::size_type;

    if (lhs.n_cols() != rhs.n_rows())
        throw std::invalid_argument("Product of matrices of given sizes if undefined");

    Matrix<T> product{lhs.n_rows(), rhs.n_cols()};
    const size_type dim = lhs.n_cols();

    rhs.transpose();

    size_type i;
    #pragma omp parallel for
    for (i = 0; i != lhs.n_rows(); ++i)
    {
        size_type j;
        #pragma omp parallel for
        for (size_type j = 0; j != rhs.n_cols(); ++j)
        {
            for (size_type k = 0; k != dim; ++k)
                product[i, j] += lhs[i, k] * rhs[j, k];
        }
    }

    return product;
}

} // namespace parallel

#endif // INCLUDE_MATRIX_HPP
