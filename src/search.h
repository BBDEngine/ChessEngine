#pragma once

#include "board.h"
#include "move.h"
#include "tt.h"
#include "util.h"
#include <array>
#include <cstdint>
#include <ctime>

#include <filesystem>

// Setup for searching thread
namespace BBD::Engine
{

constexpr int MAX_DEPTH = 100;

inline void init(const std::string &weitghts_path = "./drill/nnue_v1-100/quantised.bin")
{
    BBD::attacks::init();
    BBD::Zobrist::init();
    BBD::NNUE::NNUENetwork::load_from_file(weitghts_path);
}

class SearchLimiter
{
  public:
    struct SearchMode
    {
        static constexpr int DEPTH_SEARCH = 1;
        static constexpr int TIME_SEARCH = 2;
    };

  private:
    int depth;
    std::time_t move_time;
    int mode;

  public:
    void set_depth(int _depth)
    {
        depth = _depth;
        mode = SearchMode::DEPTH_SEARCH;
    }

    const int get_depth() const
    {
        return depth;
    }

    void set_time(std::time_t _move_time)
    {
        move_time = _move_time - 10;
        mode = SearchMode::TIME_SEARCH;
    }

    const std::time_t get_move_time() const
    {
        return move_time;
    }

    const int get_mode() const
    {
        return mode;
    }
};

class SearchThread
{
  private:
    Board board;
    Move thread_best_move, root_best_move;
    SearchLimiter limiter;
    std::array<std::array<std::array<int, 64>, 64>, 2> history;
    std::array<std::array<Move, 2>, MAX_DEPTH> killers;

    TranspositionTable tt;

    time_t start_time;

    uint64_t nodes;

  public:
    void order_moves(MoveList &moves, int nr_moves, const Move tt_move, int ply);

    Score quiescence(Score alpha, Score beta, int ply);

    template <bool root_node> Score negamax(Score alpha, Score beta, int depth, int ply);

    Move search(Board &board, SearchLimiter &limiter);

    uint64_t get_nodes()
    {
        return nodes;
    }

    void start_clock()
    {
        start_time = get_time_since_start();
    }
};

} // namespace BBD::Engine