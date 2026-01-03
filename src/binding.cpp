#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "bot_tester.h"

namespace py = pybind11;

PYBIND11_MODULE(bot_tester, m)
{
    // --- 2. Define your functions ---
    m.def("get_start_position", []() {
        return get_start_position();
    });

    m.def("legal_moves", [](std::vector<int64_t> gamestate, int64_t turn_counter) {
        return legal_moves(gamestate, turn_counter);
    });

    m.def("count_neighbors", [](std::vector<int64_t> gamestate, int64_t bee) {
        int64_t position = 0;
        for (int64_t bee : gamestate)
            position |= bee;
        return count_neighbors(position, bee);
    });

    m.def("game_terminated", [](std::vector<int64_t> gamestate, int64_t turn_counter, std::vector<std::vector<int64_t>> game_history) {
        return game_terminated(gamestate, turn_counter, game_history);
    });

    m.def("generate_next_gamestates", [](std::vector<int64_t> gamestate, int64_t turn_counter) {
        return generate_next_gamestates(gamestate, turn_counter);
    });
}