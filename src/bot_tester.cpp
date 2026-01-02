#include <algorithm>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>
#include <bitset>
#include "bot_tester.h"

using namespace std;
using Bitboard = int64_t;
using Gamestate = vector<Bitboard>;

const int64_t SEARCH_DEPTH = 10;

bool equal_gamestates(const Gamestate &a, const Gamestate &b)
{
    for (int64_t i = 0; i < a.size(); ++i)
    {
        if (a[i] != b[i])
            return false;
    }
    return true;
}

int64_t game_terminated(Gamestate state, int64_t turn_counter, vector<Gamestate> history)
{
    Bitboard white_bees = state[0] | state[1] | state[2] | state[3];
    Bitboard black_bees = state[4] | state[5] | state[6] | state[7];
    int64_t white_mobility = number_legal_moves(state, 0);
    int64_t black_mobility = number_legal_moves(state, 1);
    if ((white_bees & 0b0000000000000000000000000000000000000000000000000000000011100000ULL) || (!black_mobility && (turn_counter % 2 == 1)))
        return 1; // white wins
    if ((black_bees & 0b0000111000000000000000000000000000000000000000000000000000000000ULL) || (!white_mobility && (turn_counter % 2 == 0)))
        return 2; // black wins

    int64_t state_counter = 0;
    for (int64_t i = 0; i < history.size(); ++i)
        state_counter += (int64_t)equal_gamestates(state, history[i]);
    if (state_counter == 2)
        return 3; // draw
    
    return 0;
}

Gamestate make_turn(Gamestate gamestate, int64_t turn_counter, int64_t depth)
{
    vector<Gamestate> next_gamestates = generate_next_gamestates(gamestate, turn_counter);

    int64_t best_eval = -WIN_VAL;
    if (turn_counter % 2 == 1)
        best_eval = WIN_VAL;

    Gamestate best_next_state;
    for (Gamestate state : next_gamestates)
    {
        int64_t evaluation = minmax_ab(state, depth, turn_counter, -WIN_VAL, WIN_VAL);
        // int64_t evaluation = eval(state, turn);
        if (turn_counter % 2 == 0)
        {
            if (evaluation >= best_eval)
            {
                best_eval = evaluation;
                best_next_state = state;
            }
        }
        else
        {
            if (evaluation <= best_eval)
            {
                best_eval = evaluation;
                best_next_state = state;
            }
        }
    }

    return best_next_state;
}



int64_t also_not_main()
{
    Gamestate gamestate = START_POSITION;
    vector<Gamestate> game_history = {gamestate};
    print_gamestate(gamestate);

    int64_t game_result = 0;
    int64_t turn_counter = 0;
    while (true)
    {
        gamestate = make_turn(gamestate, turn_counter, 10);
        game_history.push_back(gamestate);
        print_gamestate(gamestate);

        game_result = game_terminated(gamestate, turn_counter, game_history);
        if (game_result)
        {
            break;
        }
        turn_counter++;
    }

    if (game_result == 1)
        cout << "White wins\n";
    else if (game_result == 2)
        cout << "Black wins\n";
    else
        cout << "Draw\n";

    return 0;    
}
