#ifndef MOVE_CALCULATION_H
#define MOVE_CALCULATION_H

#include "move_calculation.h"

// Type aliases
using Bitboard = int64_t;
using Gamestate = std::vector<Bitboard>;

// Global variables
extern const int64_t DEPTH;
extern const int64_t WIN_VAL;
extern const Bitboard SIXTY_FOUR_IS_BAD;

// Evaluation / helper functions
int64_t ct_ranks_inv(Bitboard x);   // white distance to goal
int64_t ct_ranks(int64_t x);        // black distance to goal

int64_t eval(
    Gamestate state,
    int64_t turn_counter
);

// Minimax with alpha-beta pruning
int64_t minmax_ab(
    Gamestate state,
    int64_t depth,
    int64_t turn_counter,
    int64_t alpha,
    int64_t beta
);

#endif // MOVE_CALCULATION_H
