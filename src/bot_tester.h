#ifndef HEURISTIC_BOT_H
#define HEURISTIC_BOT_H

#include "heuristic_bot.h"

// Type aliases
using Bitboard = int64_t;
using Gamestate = std::vector<Bitboard>;

// Global variables
extern const int64_t SEARCH_DEPTH;

// Function declarations
bool equal_gamestates(const Gamestate &a, const Gamestate &b);

int64_t game_terminated(
    Gamestate state,
    int64_t turn_counter,
    std::vector<Gamestate> history
);

Gamestate make_turn(
    Gamestate gamestate,
    int64_t turn_counter,
    int64_t depth = SEARCH_DEPTH
);

int64_t also_not_main();

#endif // HEURISTIC_BOT_H
