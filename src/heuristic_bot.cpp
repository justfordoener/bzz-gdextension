#include <algorithm>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>
#include <bitset>
#include "heuristic_bot.h"

using namespace std;
using Bitboard = int64_t;
using Gamestate = vector<Bitboard>;

const int64_t DEPTH = 10;
const int64_t WIN_VAL = 1000;
const Bitboard SIXTY_FOUR_IS_BAD = 0b1111111111111111111111111111111111111111111111111111111111111110ULL;

// for white distance to goal
int64_t ct_ranks_inv(Bitboard x)
{
    int64_t count = 0;
    while (x & SIXTY_FOUR_IS_BAD)
    {
        x >>= 7;
        count++;
    }
    return count;
}

// for black distance to goal
int64_t ct_ranks(int64_t x)
{
    int64_t count = 0;
    while (x)
    {
        x <<= 7;
        count++;
    }
    return count;
}

int64_t eval(Gamestate state, int64_t turn_counter)
{
    Bitboard white_bees = state[0] | state[1] | state[2] | state[3];
    Bitboard black_bees = state[4] | state[5] | state[6] | state[7];
    int64_t white_mobility = number_legal_moves(state, 0);
    int64_t black_mobility = number_legal_moves(state, 1);
    if ((white_bees & 0b0000000000000000000000000000000000000000000000000000000011100000ULL) || (!black_mobility && (turn_counter % 2 == 1)))
        return WIN_VAL; // white wins
    if ((black_bees & 0b0000111000000000000000000000000000000000000000000000000000000000ULL) || (!white_mobility && (turn_counter % 2 == 0)))
        return -WIN_VAL; // black wins
    int64_t white_distance = ct_ranks_inv(state[0]) + ct_ranks_inv(state[1]) + ct_ranks_inv(state[2]) + ct_ranks_inv(state[3]);
    int64_t black_distance = ct_ranks(state[4]) + ct_ranks(state[5]) + ct_ranks(state[6]) + ct_ranks(state[7]);

    return (white_mobility - black_mobility) + (black_distance - white_distance);
}

int64_t minmax_ab(Gamestate state, int64_t depth, int64_t turn_counter, int64_t alpha, int64_t beta)
{
    int64_t evaluation = eval(state, turn_counter);
    if (!depth || abs(evaluation) >= WIN_VAL)
    {
        return evaluation;
    }

    vector<Gamestate> resulting_positions = generate_next_gamestates(state, turn_counter);
    int64_t turn_indicator = (turn_counter % 2) * 2 - 1;
    int64_t best_value = turn_indicator * WIN_VAL; // white: -WIN_VAL, black: WIN_VAL
    for (const Gamestate& gs : resulting_positions)
    {
        int64_t value = -minmax_ab(gs, depth - 1, turn_counter + 1, alpha, beta);

        if (turn_indicator == -1)
        {
            // white: maximize
            best_value = max(best_value, value);
            alpha = max(alpha, best_value);
        }
        else 
        {
            // white: minimize
            best_value = min(best_value, value);
            beta = min(beta, best_value);
        }
        if (alpha >= beta)
            break;
    }
    return best_value;
}
