#include "search.h"
#include "eval.h"
#include <cassert>
#include <fstream>
#include <iostream>

namespace BBD::Engine
{

inline void print_board(const Board &board)
{
    for (int rank = 7; rank >= 0; rank--)
    {
        std::cout << rank + 1 << "  ";
        for (int file = 0; file < 8; file++)
        {
            Piece piece = board.at(rank * 8 + file);
            if (piece != Pieces::NO_PIECE)
            {
                std::cout << piece.to_char() << ' ';
            }
            else
            {
                std::cout << ". ";
            }
        }
        std::cout << '\n';
    }
    std::cout << "\n   a b c d e f g h\n\n";
}

void SearchThread::order_moves(MoveList &moves, int nr_moves)
{
    std::array<int, 256> scores;

    // Try to rank captures higher
    for (int i = 0; i < nr_moves; i++)
    {
        Move move = moves[i];
        if (board.is_capture(move))
            scores[i] = 100000 * board.at(move.to());
        else
            scores[i] = history[board.player_color()][move.from()][move.to()];
    }

    // Simple sort
    for (int i = 0; i < nr_moves; i++)
    {
        for (int j = i + 1; j < nr_moves; j++)
        {
            if (scores[j] > scores[i])
            {
                std::swap(scores[j], scores[i]);
                std::swap(moves[j], moves[i]);
            }
        }
    }
}

Score SearchThread::quiescence(Score alpha, Score beta)
{
    if (board.threefold_check())
    {
        return 0; // draw
    }

    nodes++;

    if (limiter.get_mode() == SearchLimiter::SearchMode::TIME_SEARCH)
    {
        if (nodes && nodes % (1 << 12))
        {
            if (get_time_since_start() - start_time > limiter.get_move_time())
                throw "Timeout";
        }
    }

    Score eval = board_evaluation(board) * (board.player_color() == Colors::WHITE ? 1 : -1);
    Score best = eval;

    if (best >= beta)
        return best;
    alpha = std::max(alpha, best);

    MoveList moves;
    int nr_moves = board.gen_legal_moves<CAPTURE_MOVES>(moves);

    order_moves(moves, nr_moves);

    for (int i = 0; i < nr_moves; i++)
    {
        Move move = moves[i];

        if (!board.is_legal(move))
            continue;

        assert(board.is_capture(move));

        board.make_move(move);
        Score score = -quiescence(-beta, -alpha);
        board.undo_move(move);

        if (score > best)
        {
            best = score;
            if (score > alpha)
            {
                alpha = score;
                if (alpha >= beta)
                    break;
            }
        }
    }
    return best;
}

template <bool root_node> Score SearchThread::negamax(Score alpha, Score beta, int depth, int ply)
{
    Score alpha_original = alpha;
    Move best_move;

    if (!root_node && board.threefold_check())
    {
        return 0; // draw
    }
    if (depth == 0)
        return quiescence(alpha, beta);

    nodes++;

    if (limiter.get_mode() == SearchLimiter::SearchMode::TIME_SEARCH)
    {
        if (nodes && nodes % (1 << 12))
        {
            if (get_time_since_start() - start_time > limiter.get_move_time())
                throw "Timeout";
        }
    }

    // Transposition table probe

    uint64_t posKey = board.get_cur_hash();
    {
        Score ttScore;
        TTBound ttBound;
        Move ttMove;

        if (!root_node && tt.probe(posKey, depth, ttScore, ttBound, ttMove))
        {
            if (ttBound == TTBound::EXACT)
                return ttScore;
            if (ttBound == TTBound::LOWER && ttScore > alpha)
                alpha = ttScore;
            else if (ttBound == TTBound::UPPER && ttScore < beta)
                beta = ttScore;

            if (alpha >= beta)
                return ttScore;
        }
    }

    // Reverse futility pruning

    Score eval = board_evaluation(board) * (board.player_color() == Colors::WHITE ? 1 : -1);

    if (!root_node && !board.checkers() && depth <= 3)
    {
        int margin = 200 * depth;
        if (eval >= beta + margin)
        {
            return eval;
        }
    }

    // Principal variation search

    MoveList moves;
    int nr_moves = board.gen_legal_moves<ALL_MOVES>(moves);

    order_moves(moves, nr_moves);

    Score best = -INF;
    int played = 0;

    for (int i = 0; i < nr_moves; i++)
    {
        Move move = moves[i];
        if (!board.is_legal(move))
            continue;

        board.make_move(move);
        played++;

        Score score;

        if (played > 1)
        {
            score = -negamax<false>(-alpha - 1, -alpha, depth - 1, ply + 1);

            if (score > alpha && score < beta)
                score = -negamax<false>(-beta, -alpha, depth - 1, ply + 1);
        }
        else
        {
            score = -negamax<false>(-beta, -alpha, depth - 1, ply + 1);
        }

        board.undo_move(move);

        if (score > best)
        {
            best = score;
            best_move = move;

            if (score > alpha)
            {
                if constexpr (root_node)
                {
                    root_best_move = move;
                }
                alpha = score;

                if (alpha >= beta)
                {
                    history[board.player_color()][move.from()][move.to()] += depth * depth;
                    break;
                }
            }
        }
    }

    // Checkmate / stalemate detection

    if (played == 0)
        return board.checkers() ? -INF + ply : 0;

    // Store in transposition table

    TTBound bound_type;
    if (best <= alpha_original)
        bound_type = TTBound::UPPER;
    else if (best >= beta)
        bound_type = TTBound::LOWER;
    else
        bound_type = TTBound::EXACT;

    tt.store(posKey, depth, best, bound_type, best_move);

    return best;
}

Move SearchThread::search(Board &_board, SearchLimiter &_limiter)
{
    auto search_start_time = get_time_since_start();
    nodes = 0;
    board = _board, limiter = _limiter;

    tt.clear();

    // Fill history with 0 at the beginning

    for (auto &t : history)
    {
        for (auto &p : t)
            p.fill(0);
    }

    Score score;
    auto depth = 1;
    auto running = true;
    int limit_depth = limiter.get_mode() == SearchLimiter::SearchMode::DEPTH_SEARCH ? limiter.get_depth() : 100;

    start_clock();
    while (running && depth <= limit_depth) // limit how much we can search
    {
        try
        {
            score = negamax<true>(-INF, INF, depth, 0);
            std::cout << "info score " << score << " depth " << depth << " nodes " << nodes << " time "
                      << get_time_since_start() - search_start_time << std::endl;
            depth++;
            thread_best_move = root_best_move; // only take into account full search results, for now
        }
        catch (...)
        {
            running = false;
        }
    }
    std::cout << "bestmove " << thread_best_move.to_string() << std::endl;

    return thread_best_move;
}

} // namespace BBD::Engine