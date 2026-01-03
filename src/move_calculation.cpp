#include "move_calculation.h"

const std::vector<int64_t> NEIGHBOR_OFFSETS = {1, 6, 7};
const Bitboard MAP_MASK = 0b0000111000111100111110111111111111111111101111100111100011100000ULL;
const Gamestate START_POSITION = {
    0b0000000000100000000000000000000000000000000000000000000000000000ULL,
    0b0000000000010000000000000000000000000000000000000000000000000000ULL,
    0b0000000000001000000000000000000000000000000000000000000000000000ULL,
    0b0000000000000100000000000000000000000000000000000000000000000000ULL,
    0b0000000000000000000000000000000000000000000000000100000000000000ULL,
    0b0000000000000000000000000000000000000000000000000010000000000000ULL,
    0b0000000000000000000000000000000000000000000000000001000000000000ULL,
    0b0000000000000000000000000000000000000000000000000000100000000000ULL};
const int64_t BEES_PER_PLAYER = 4;
const int64_t HEXAGON = 6;
const Gamestate EMPTY_GAMESTATE = {0, 0, 0, 0, 0, 0, 0, 0};
const Bitboard PORTAL_TILE_1 =      0b0000000000000000000000000001000000000000000000000000000000000000ULL;
const Bitboard PORTAL_TILE_2 =      0b0000000000000000000000000000100000000000000000000000000000000000ULL;
const Bitboard PORTAL_TILE_3 =      0b0000000000000000000000000000000000100000000000000000000000000000ULL;
const Bitboard PORTAL_TILE_4 =      0b0000000000000000000000000000000000010000000000000000000000000000ULL;

void print_gamestate(Gamestate gamestate, Bitboard highlighted_tile, bool censored)
{
    std::vector<std::string> preamble = {"", " ", "  ", "   ", "    ", "     ", "      ", "       ", "        "};
    for (int64_t i = 0; i < 9; ++i)
    {
        std::cout << preamble[i];
        for (int64_t j = 0; j < 7; ++j)
        {
            int64_t index = 63 - (i * 7 + j); // start from the top (MSB)

            if (!(MAP_MASK & (1ULL << index)) && censored)
                std::cout << "  ";
            else if (highlighted_tile & (1ULL << index))
                std::cout << "O ";
            else if (gamestate[0] & (1ULL << index) || gamestate[1] & (1ULL << index) || gamestate[2] & (1ULL << index) || gamestate[3] & (1ULL << index))
                std::cout << "w ";
            else if (gamestate[4] & (1ULL << index) || gamestate[5] & (1ULL << index) || gamestate[6] & (1ULL << index) || gamestate[7] & (1ULL << index))
                std::cout << "b ";
            else
                std::cout << ". ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

int64_t count_neighbors(Bitboard position, Bitboard tile)
{
    Bitboard surrounding_tiles = (tile << NEIGHBOR_OFFSETS[0]) | (tile << NEIGHBOR_OFFSETS[1]) | (tile << NEIGHBOR_OFFSETS[2]) |
                                 (tile >> NEIGHBOR_OFFSETS[0]) | (tile >> NEIGHBOR_OFFSETS[1]) | (tile >> NEIGHBOR_OFFSETS[2]);
    Bitboard neighbors = surrounding_tiles & position;
    if (tile & PORTAL_TILE_1)
    {
        neighbors -= PORTAL_TILE_2 & neighbors;
    }
    else if (tile & PORTAL_TILE_2)
    {
        neighbors -= (PORTAL_TILE_1 | PORTAL_TILE_3) & neighbors;
    }
    else if (tile & PORTAL_TILE_3)
    {
        neighbors -= (PORTAL_TILE_2 | PORTAL_TILE_4) & neighbors;
    }
    else if (tile & PORTAL_TILE_4)
    {
        neighbors -= PORTAL_TILE_2 & neighbors;
    }
    return std::bitset<64>(neighbors).count();
}

Bitboard generate_moves(Bitboard position, Bitboard tile)
{
    Bitboard free_tiles = ~position & MAP_MASK;

    int64_t num_neighbors = count_neighbors(position, tile);
    if (num_neighbors == 0)
        return 0;

    Bitboard possible_moves = 0;
    for (long unsigned int i = 0; i < NEIGHBOR_OFFSETS.size(); ++i)
    {
        Bitboard ray = tile;
        Bitboard ray_rev = tile;
        for (int64_t j = 0; j < num_neighbors; ++j)
        {
            ray = (ray << NEIGHBOR_OFFSETS[i]) & free_tiles;
            ray_rev = (ray_rev >> NEIGHBOR_OFFSETS[i]) & free_tiles;
        }
        possible_moves |= ray | ray_rev;
    }
    return possible_moves;
}

int64_t ctz(Bitboard x)
{
    if (x == 0)
        return 64;
    int64_t count = 0;
    while ((x & 1) == 0)
    {
        x >>= 1;
        count++;
    }
    return count;
}

int64_t count_ones(Bitboard x) 
{
    int64_t count;
    for (count=0; x; count++)
        x &= x - 1;
    return count;
}

Bitboard position_from_gamestate(Gamestate gamestate)
{
    Bitboard position = 0;
    for (Bitboard bee : gamestate)
    {
        position |= bee;
    }
    return position;
}

std::vector<Gamestate> generate_next_gamestates(Gamestate gamestate, int64_t turn_counter)
{
    Bitboard position = position_from_gamestate(gamestate);
    Bitboard free_tiles = ~position & MAP_MASK;

    std::vector<Gamestate> moves(BEES_PER_PLAYER * HEXAGON, EMPTY_GAMESTATE);
    size_t move_index = 0;
    int64_t start_bee_index = BEES_PER_PLAYER * (turn_counter % 2); // turn 0 is white
    for (int64_t i = start_bee_index; i < start_bee_index + BEES_PER_PLAYER; ++i)
    {
        Bitboard bee = gamestate[i];
        int64_t neighbors = count_neighbors(position, bee);
        if (neighbors == 0)
            continue;

        for (long unsigned int j = 0; j < NEIGHBOR_OFFSETS.size(); ++j)
        {
            Bitboard ray = bee;
            Bitboard ray_rev = bee;
            for (int64_t k = 0; k < neighbors; ++k)
            {
                ray = (ray << NEIGHBOR_OFFSETS[j]) & (free_tiles);
                ray_rev = (ray_rev >> NEIGHBOR_OFFSETS[j]) & (free_tiles);
            }

            Gamestate new_state = gamestate;
            new_state[i] = ray;
            moves[move_index] = new_state;
            move_index += (ray != 0);

            Gamestate new_state_rev = gamestate;
            new_state_rev[i] = ray_rev;
            moves[move_index] = new_state_rev;
            move_index += (ray_rev != 0);
        }
    }
    moves.resize(move_index);
    return moves;
}

std::vector<int64_t> legal_moves(Gamestate gamestate, int64_t turn_counter) // returns (from, to) indices of legal moves
{
    Bitboard position = position_from_gamestate(gamestate);

    std::vector<int64_t> indexed_moves;
    indexed_moves.resize(2 * 2 * BEES_PER_PLAYER * HEXAGON); // two indices for each move(from, to), for two players, for each bee, in each direction

    int64_t move_idx = 0;
    int64_t start_bee_index = BEES_PER_PLAYER * (turn_counter % 2); // turn 0 is white
    for (int64_t i = start_bee_index; i < start_bee_index + BEES_PER_PLAYER; ++i)
    {
        Bitboard bee = gamestate[i];
        int64_t from = ctz(bee);
        Bitboard bitboard_moves = generate_moves(position, bee);
        while (bitboard_moves)
        {
            Bitboard next_move = bitboard_moves & -bitboard_moves;
            int64_t to = ctz(next_move);
            bitboard_moves &= bitboard_moves - 1;

            indexed_moves[move_idx] = from;
            move_idx++;
            indexed_moves[move_idx] = to;
            move_idx++;
        }
    }
    indexed_moves.resize(move_idx);
    return indexed_moves;
}

int64_t number_legal_moves(Gamestate gamestate, int64_t turn_counter) 
{
    Bitboard position = position_from_gamestate(gamestate);

    std::vector<std::pair<int64_t, int64_t>> indexed_moves;
    int64_t start_bee_index = BEES_PER_PLAYER * (turn_counter % 2); // turn 0 is white
    int64_t legal_moves_counter = 0;
    for (int64_t i = start_bee_index; i < start_bee_index + BEES_PER_PLAYER; ++i) 
    {
        Bitboard bee = gamestate[i];
        //int64_t from = ctz(bee);
        Bitboard bitboard_moves = generate_moves(position, bee);
        legal_moves_counter += count_ones(bitboard_moves);
    }
    return legal_moves_counter;
}