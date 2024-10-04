#include <random>
#include <chrono>
#include <iostream>
#include <print>
#include <optional>
#include <stdexcept>
#include <string>
#include <format>
#include <type_traits>

#include <boost/program_options.hpp>

#include "matrix.hpp"

namespace
{

template<typename T>
std::pair<parallel::Matrix<T>, parallel::Matrix<T>> get_matrices(std::size_t size)
{
    std::random_device rd;
    std::mt19937_64 gen{rd()};

    using distribution_type = std::conditional_t<std::is_floating_point_v<T>,
        std::uniform_real_distribution<T>, std::uniform_int_distribution<T>>;

    distribution_type elem{T{-10}, T{10}};
    auto generator = [&]{ return elem(gen); };

    std::vector<T> v(size * size);

    std::ranges::generate(v, generator);
    parallel::Matrix m_1(size, size, v.begin(), v.end());

    std::ranges::generate(v, generator);
    parallel::Matrix m_2(size, size, v.begin(), v.end());

    return std::pair{std::move(m_1), std::move(m_2)};
}

enum class MultiplicationAlgorithm
{
    naive,
    transpose_second,
    block
};

std::optional<std::pair<std::size_t, MultiplicationAlgorithm>> get_options(int argc, char **argv)
{
    namespace po = boost::program_options;

    po::options_description desc{"Square matrix multiplication benchmark.\n\nAllowed options"};

    std::size_t side;
    std::string algo;
    desc.add_options()
        ("help,h", "Produce help message")
        ("side,s", po::value<std::size_t>(&side)->required(), "The size of both matrices' side")
        ("algorithm,a", po::value<std::string>(&algo)->default_value("naive"),
         "Algorithm chosen for multiplication. Possible options:\n"
         "- naive: multiplication by definition\n"
         "- transpose-second: the same as \"naive\" but "
         "transposes the second matrix before multiplication\n"
         "- block: multiply matrix by blocks");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help"))
    {
        std::cout << desc;
        return std::nullopt;
    }

    po::notify(vm);

    if (algo == "naive")
        return std::pair{side, MultiplicationAlgorithm::naive};
    else if (algo == "transpose-second")
        return std::pair{side, MultiplicationAlgorithm::transpose_second};
    else if (algo == "block")
        return std::pair{side, MultiplicationAlgorithm::block};

    throw std::invalid_argument{std::format("unknown multiplication algorithm \"{}\"", algo)};
}

} // unnamed namespace

int main(int argc, char **argv) try
{
    auto maybe_opts = get_options(argc, argv);
    if (!maybe_opts.has_value())
        return 0;

    auto &[side, algo] = *maybe_opts;

    auto [A, B] = get_matrices<double>(side);

    auto start = std::chrono::high_resolution_clock::now();
    switch (algo)
    {
        case MultiplicationAlgorithm::naive:
        {
            [[maybe_unused]] auto C = parallel::naive_product(A, B);
            break;
        }
        case MultiplicationAlgorithm::transpose_second:
        {
            [[maybe_unused]] auto C = parallel::transpose_product(A, B);
            break;
        }
        case MultiplicationAlgorithm::block:
        {
            [[maybe_unused]] auto C = parallel::drepper_product(A, B);
            break;
        }
        default:
            std::unreachable();
    }
    auto finish = std::chrono::high_resolution_clock::now();

    using ms = std::chrono::milliseconds;
    std::println("Matrix multiplication took: {} ms",
                 std::chrono::duration_cast<ms>(finish - start).count());

    return 0;
}
catch (const std::exception &e)
{
    std::println(std::cerr, "Error: {}", e.what());
    return 1;
}
catch (...)
{
    std::println(std::cerr, "Unknown exception.");
    return 1;
}
