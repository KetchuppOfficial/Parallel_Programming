#ifndef INCLUDE_PARALLEL_SOLVER_HPP
#define INCLUDE_PARALLEL_SOLVER_HPP

#include <cstddef>
#include <cstdlib>
#include <cassert>

#include <boost/mpi/communicator.hpp>
#include <boost/mpi/collectives.hpp>

#include "solver_base.hpp"

namespace parallel
{

class Transport_Equation_PSolver final : public Transport_Equation_Solver_Base
{
public:

    using Transport_Equation_Solver_Base::two_arg_func;
    using Transport_Equation_Solver_Base::one_arg_func;

    Transport_Equation_PSolver(const boost::mpi::communicator &world, double a,
                               double t_1, double t_2, std::size_t N_t,
                               double x_1, double x_2, std::size_t N_x,
                               two_arg_func heterogeneity,
                               one_arg_func init_cond, one_arg_func boundary_cond)
        : Transport_Equation_Solver_Base{a, N_t, (t_2 - t_1) / N_t,
                                         (N_x + 1) / world.size() - 1, (x_2 - x_1) / N_x,
                                         heterogeneity}
    {
        for (auto start_i = grid_.x_size() * world.rank(), i = 0uz; i != grid_.x_size(); ++i)
            grid_[0, i] = init_cond((start_i + i) * h_);

        if (const int rank = world.rank(); rank == 0)
        {
            for (auto i = 1uz; i != grid_.t_size(); ++i)
                grid_[i, 0] = boundary_cond(t_1 + i * tau_);

            solve_left_border(world);
        }
        else if (rank == world.size() - 1)
            solve_right_border(world);
        else
            solve_middle(world);

        reduce(world);
    }

private:

    void solve_left_border(const boost::mpi::communicator &world)
    {
        assert(world.rank() == 0);

        constexpr int tag = 0;
        constexpr int next_node_rank = 1;
        const std::size_t N_x = grid_.x_size();

        world.send(next_node_rank, tag, grid_[0, N_x - 1]);

        double rightmost;
        world.recv(next_node_rank, tag, rightmost);

        for (auto m = 1uz; m != N_x - 1; ++m)
            explicit_four_points(0, m);

        explicit_four_points(0, N_x - 1, grid_[0, N_x - 2], rightmost);

        for (auto k = 1uz; k != grid_.t_size() - 1; ++k)
        {
            world.send(next_node_rank, tag, grid_[k, N_x - 1]);
            world.recv(next_node_rank, tag, rightmost);

            for (auto m = 1; m != N_x - 1; ++m)
                cross(k, m);

            cross(k, N_x - 1, grid_[k, N_x - 2], rightmost);
        }
    }

    void solve_middle(const boost::mpi::communicator &world)
    {
        assert(0 < world.rank() && world.rank() < world.size() - 1);

        constexpr int tag = 0;
        const int rank = world.rank();
        const std::size_t N_x = grid_.x_size();

        world.send(rank + 1, tag, grid_[0, N_x - 1]);
        world.send(rank - 1, tag, grid_[0, 0]);

        double leftmost, rightmost;
        world.recv(rank - 1, tag, leftmost);
        world.recv(rank + 1, tag, rightmost);

        explicit_four_points(0, 0, leftmost, grid_[0, 1]);

        for (auto m = 1uz; m != N_x - 1; ++m)
            explicit_four_points(0, m);

        explicit_four_points(0, N_x - 1, grid_[0, N_x - 2], rightmost);

        for (auto k = 1uz; k != grid_.t_size() - 1; ++k)
        {
            world.send(rank + 1, tag, grid_[k, N_x - 1]);
            world.send(rank - 1, tag, grid_[k, 0]);

            world.recv(rank - 1, tag, leftmost);
            world.recv(rank + 1, tag, rightmost);

            cross(k, 0, leftmost, grid_[k, 1]);

            for (auto m = 1; m != N_x - 1; ++m)
                cross(k, m);

            cross(k, N_x - 1, grid_[k, N_x - 2], rightmost);
        }
    }

    void solve_right_border(const boost::mpi::communicator &world)
    {
        assert(world.rank() == world.size() - 1);

        constexpr int tag = 0;
        const int rank = world.rank();
        const std::size_t N_x = grid_.x_size();

        world.send(rank - 1, tag, grid_[0, 0]);

        double leftmost;
        world.recv(rank - 1, tag, leftmost);

        explicit_four_points(0, 0, leftmost, grid_[0, 1]);

        for (auto m = 1uz; m != N_x - 1; ++m)
            explicit_four_points(0, m);

        explicit_left_corner(0, N_x - 1);

        for (auto k = 1uz; k != grid_.t_size() - 1; ++k)
        {
            world.send(rank - 1, tag, grid_[k, 0]);
            world.recv(rank - 1, tag, leftmost);

            cross(k, 0, leftmost, grid_[k, 1]);

            for (auto m = 1; m != N_x - 1; ++m)
                cross(k, m);

            explicit_left_corner(k, N_x - 1);
        }
    }

    void reduce(const boost::mpi::communicator &world)
    {
        constexpr int tag = 0;
        int rank = world.rank();
        unsigned current_size = world.size();
        unsigned current_rank = rank;

        for (int shift = 1; current_size > 1; shift *= 2)
        {
            if (current_rank % 2)
            {
                world.send(rank - shift, tag, grid_.storage());
                return;
            }
            else if (current_rank < current_size - 1)
            {
                std::vector<double> another_grid;
                world.recv(rank + shift, tag, another_grid);

                std::size_t t_size = grid_.t_size();
                std::size_t lhs_x_size = grid_.x_size();
                std::size_t rhs_x_size = another_grid.size() / t_size;

                Grid output{grid_.t_size(), lhs_x_size + rhs_x_size};

                for (auto k = 0uz; k != t_size; ++k)
                {
                    for (auto m = 0uz; m != lhs_x_size; ++m)
                        output[k, m] = grid_[k, m];
                    for (auto m = 0uz; m != rhs_x_size; ++m)
                        output[k, m + lhs_x_size] = another_grid[k * rhs_x_size + m];
                }

                grid_ = std::move(output);
            }

            std::div_t res = std::div(current_size, 2);
            current_size = res.rem ? 1 + res.quot : res.quot;
            current_rank /= 2;
        }
    }
};

} // namespace parallel

#endif // INCLUDE_SOLVER_HPP
