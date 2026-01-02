#include "move_generation.h"

void MoveGenerator::_bind_methods() {
	Ref<MoveGenerator> mg = memnew(MoveGenerator);
	godot::ClassDB::bind_method(D_METHOD("available_moves"), &MoveGenerator::available_moves);
	godot::ClassDB::bind_method(D_METHOD("send_user_move", "move"), &MoveGenerator::send_user_move);
	godot::ClassDB::bind_method(D_METHOD("request_bot_move"), &MoveGenerator::request_bot_move);
	godot::ClassDB::bind_method(D_METHOD("get_tile_indices"), &MoveGenerator::get_tile_indices);
}

PackedInt64Array vector_to_godot_array(std::vector<int64_t> arr) {
	PackedInt64Array new_arr;
	for (int64_t x : arr)
	{
		new_arr.push_back(x);
	}
	return new_arr;
}

PackedInt64Array MoveGenerator::get_tile_indices()
{
	int64_t index = 1;
	Bitboard mask = MAP_MASK;
	PackedInt64Array tile_indices;
	while (mask)
	{
		mask >>= 1;
		if (mask & 1)
			tile_indices.push_back(index);
		index++;
	}
	return tile_indices;
}

PackedInt64Array MoveGenerator::available_moves() {
	return vector_to_godot_array(legal_moves(gamestate, turn_counter));
}

PackedInt64Array MoveGenerator::send_user_move(PackedInt64Array move)
{
	Bitboard from_move = 1;
	Bitboard to_move = 1;
	for (int i = 0; i < move[0]; ++i)
		from_move <<= 1;
	for (int i = 0; i < move[1]; ++i)
		to_move <<= 1;
	for (int i = 0; i < gamestate.size(); ++i)
		if (gamestate[i] == from_move)
			gamestate[i] = to_move;
			std::cout << "gamestate updated";

	std::vector<int64_t> game_result;
	game_result.push_back(game_terminated(gamestate, turn_counter, game_history));
	
	game_history.push_back(gamestate);
	turn_counter++;
	return vector_to_godot_array(game_result);
}

PackedInt64Array MoveGenerator::request_bot_move()
{
	Gamestate new_gamestate = make_turn(gamestate, turn_counter);

	Bitboard from_move;
	Bitboard to_move;
	for (int i = 0; i < gamestate.size(); ++i)
	{
		if (new_gamestate[i] != gamestate[i])
		{
			from_move = gamestate[i];
			to_move = new_gamestate[i];
		}
	}
	std::vector<int64_t> move = {ctz(from_move), ctz(to_move)};

	gamestate = new_gamestate;
	move.push_back(game_terminated(gamestate, turn_counter, game_history));
	
	game_history.push_back(gamestate);
	turn_counter++;
	return vector_to_godot_array(move);
}