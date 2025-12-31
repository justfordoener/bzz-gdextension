#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/variant/array.hpp"
#include "move_calculation.h"
#include <vector>

using namespace godot;

class MoveGenerator : public RefCounted {
	GDCLASS(MoveGenerator, RefCounted)

private:
	Gamestate gamestate = START_POSITION;
	std::vector<Gamestate> game_history = {gamestate};
	int game_result = 0;
	int turn_counter = 0;

protected:
	static void _bind_methods();

public:
	MoveGenerator() = default;
	~MoveGenerator() override = default;

	PackedInt64Array print_type(const Variant &p_variant) const;
};
