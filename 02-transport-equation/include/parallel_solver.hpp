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
    using Transport_Equation_Solver_Base::Scheme;

    Transport_Equation_PSolver(const boost::mpi::communicator &world, double a,
                               double t_1, double t_2, std::size_t N_t,
                               double x_1, double x_2, std::size_t N_x,
                               two_arg_func heterogeneity,
                               one_arg_func init_cond, one_arg_func boundary_cond)
        : Transport_Equation_Solver_Base{a, N_t, (t_2 - t_1) / (N_t - 1),
                                         N_x / world.size(), (x_2 - x_1) / (N_x - 1),
                                         heterogeneity}
    {
        if (init_cond(x_1) != boundary_cond(t_1))
            throw std::invalid_argument{"Initial and boundary condition are not coordinated"};

        const std::size_t x_size = grid_.x_size();
        const std::size_t t_size = grid_.t_size();

        if (const int w_size = world.size(); w_size == 1)
        {
            for (auto i = 0uz; i != x_size; ++i)
                grid_[0, i] = init_cond(x_1 + i * h_);

            for (auto i = 1uz; i != t_size; ++i)
                grid_[i, 0] = boundary_cond(t_1 + i * tau_);

            solve_sequential(Scheme::implicit_left_corner);
        }
        else
        {
            const int rank = world.rank();

            for (auto start_i = x_size * rank, i = 0uz; i != x_size; ++i)
                grid_[0, i] = init_cond((start_i + i) * h_);

            const double courant = a_ * tau_ / h_;

            if (rank == 0)
            {
                for (auto i = 1uz; i != t_size; ++i)
                    grid_[i, 0] = boundary_cond(t_1 + i * tau_);

                solve_left_border(world, courant);

                std::vector<double> full_grid;
                full_grid.reserve(t_size * x_size * w_size);

                boost::mpi::gather(world, &grid_[0, 0], t_size * x_size, full_grid, 0);
                grid_.swap(full_grid, t_size, x_size * w_size);
            }
            else
            {
                if (rank == w_size - 1)
                    solve_right_border(world, courant);
                else
                    solve_middle(world, courant);

                boost::mpi::gather(world, &grid_[0, 0], t_size * x_size, 0);
            }
        }
    }

private:

    void solve_left_border(const boost::mpi::communicator &world, double courant)
    {
        assert(world.rank() == 0);

        constexpr int tag = 0;
        constexpr int next_node_rank = 1;
        const std::size_t N_x = grid_.x_size();
        const std::size_t N_t = grid_.t_size();

        for (auto k = 0uz; k != N_t - 1; ++k)
        {
            for (auto m = 1uz; m != N_x; ++m)
                implicit_left_corner(courant, k, m);

            world.send(next_node_rank, tag, grid_[k + 1, N_x - 1]);
        }
    }

    void solve_middle(const boost::mpi::communicator &world, double courant)
    {
        assert(0 < world.rank() && world.rank() < world.size() - 1);

        constexpr int tag = 0;
        const int rank = world.rank();
        const std::size_t N_x = grid_.x_size();
        const std::size_t N_t = grid_.t_size();

        double leftmost;

        for (auto k = 0uz; k != N_t - 1; ++k)
        {
            world.recv(rank - 1, tag, leftmost);

            implicit_left_corner(courant, k, 0, leftmost);

            for (auto m = 1uz; m != N_x; ++m)
                implicit_left_corner(courant, k, m);

            world.send(rank + 1, tag, grid_[k + 1, N_x - 1]);
        }
    }

    void solve_right_border(const boost::mpi::communicator &world, double courant)
    {
        assert(world.rank() == world.size() - 1);

        constexpr int tag = 0;
        const int rank = world.rank();
        const std::size_t N_x = grid_.x_size();
        const std::size_t N_t = grid_.t_size();

        double leftmost;

        for (auto k = 0uz; k != N_t - 1; ++k)
        {
            world.recv(rank - 1, tag, leftmost);

            implicit_left_corner(courant, k, 0, leftmost);

            for (auto m = 1uz; m != N_x; ++m)
                implicit_left_corner(courant, k, m);
        }
    }
};

} // namespace parallel

#endif // INCLUDE_SOLVER_HPP
