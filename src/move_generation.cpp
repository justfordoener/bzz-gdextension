#include "move_generation.h"

void MoveGenerator::_bind_methods() {
	Ref<MoveGenerator> mg = memnew(MoveGenerator);
	godot::ClassDB::bind_method(D_METHOD("print_type", "variant"), &MoveGenerator::print_type);
}

PackedInt64Array vector_to_godot_array(std::vector<int64_t> arr) {
	PackedInt64Array new_arr;
	for (int64_t x : arr)
	{
		new_arr.push_back(x);
	}
	return new_arr;
}

PackedInt64Array MoveGenerator::print_type(const Variant &p_variant) const {
	print_line(vformat("Type: %d", p_variant.get_type()), 2);

	return vector_to_godot_array(legal_moves(gamestate, turn_counter));
}

