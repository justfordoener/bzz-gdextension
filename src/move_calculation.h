#pragma once
#include <vector>
#include <cstdint>
#include <bitset>
#include <string>
#include <iostream>

using Bitboard = int64_t;
using Gamestate = std::vector<Bitboard>;

extern const std::vector<int64_t> NEIGHBOR_OFFSETS;
extern const Bitboard MAP_MASK;
extern const Gamestate START_POSITION;
extern const int64_t BEES_PER_PLAYER;
extern const int64_t HEXAGON;
extern const Gamestate EMPTY_GAMESTATE;
extern const Bitboard PORTAL_RED;
extern const Bitboard PORTAL_BLUE;

void print_gamestate(Gamestate gamestate, Bitboard highlighted_tile = 0, bool censored = true);
int64_t count_neighbors(Bitboard position, Bitboard tile);
Bitboard generate_moves(Bitboard position, Bitboard tile);
int64_t ctz(Bitboard x);
int64_t count_ones(Bitboard x);
Bitboard position_from_gamestate(Gamestate gamestate);
std::vector<Gamestate> generate_next_gamestates(Gamestate gamestate, int64_t turn_counter);
std::vector<int64_t> legal_moves(Gamestate gamestate, int64_t turn_counter);
int64_t number_legal_moves(Gamestate gamestate, int64_t turn_counter);
