class_name BoardManager extends Node

@export
var tile_height = 100
var tile_width = 110
var tiles = []
var tile_resource = preload("res://tile.tscn")
var bee_resource = preload("res://bee.tscn")
var available_moves : Array[Tile] = []
var clicked_tile = null
var clicked_tile_to : Array[Tile]
var move_generator: MoveGenerator
const num_ranks = 9
const num_files = 7
const max_tiles_per_file = 7
const coordinate_offset = 64
const initial_bee_positions : Array[int] = [11, 12, 13, 14, 50, 51, 52, 53]

func _ready() -> void:
	move_generator = MoveGenerator.new()
	var tile_indices : PackedInt64Array = move_generator.get_tile_indices()
	for index in tile_indices:
		var new_tile = tile_resource.instantiate()
		new_tile.link_board_manager(self)
		new_tile.connect("tile_clicked", _on_tile_clicked)
		new_tile.position = index_to_coordinates(index)
		new_tile.tile_index = index
		add_child(new_tile)
		tiles.append(new_tile)
			
	var available_moves_int = move_generator.available_moves()
	available_moves = numerical_moves_to_tiles(available_moves_int)
	# spawn bees
	for idx in initial_bee_positions:
		var tile : Tile = get_tile_by_position(idx)
		if tile.bee == null:
			var bee = bee_resource.instantiate()
			tile.bee = bee
			bee.position = tile.position
			add_child(bee)

func index_to_coordinates(index : int) -> Vector2:
	index -= 1
	var x = index % 7 * tile_width + (tile_width / 2 * index / 7) 
	var y = index / 7 * tile_height
	return Vector2(x, y)

func get_tile_by_position(pos: int) -> Tile:
	for tile in tiles:
		if tile.tile_index == pos:
			return tile
	return null

func numerical_moves_to_tiles(moves: Array[int]) -> Array[Tile]:
	var result: Array[Tile] = []

	for i in range(0, moves.size(), 2):
		var from_tile := get_tile_by_position(moves[i])
		var to_tile := get_tile_by_position(moves[i + 1])

		if from_tile == null or to_tile == null:
			push_error("Invalid move: %s -> %s" % [moves[i], moves[i + 1]])
			continue

		result.append(from_tile)
		result.append(to_tile)

	return result
		
		
func _on_tile_clicked(tile : Tile) -> void:
	print(tile.tile_index)
	if (clicked_tile == null):
		for i in range(0, available_moves.size(), 2):
			if available_moves[i] == tile:
				clicked_tile_to.append(available_moves[i + 1])
				available_moves[i + 1].highlight()
		if (clicked_tile_to.size() != 0):
			clicked_tile = tile
			clicked_tile.activate()
			print("selected tile")
		else:
			print("no bee on this tile")
	else:
		if tile in clicked_tile_to:
			await clicked_tile.send_bee(tile)
			print("valid move")
			_on_valid_move(clicked_tile, tile)
		else:
			print("invalid move. unselected tile")
		var reclick = false
		for to_tile in clicked_tile_to:
			to_tile.unhighlight()
			for i in range(0, available_moves.size(), 2):
				if available_moves[i] == tile:
					reclick = true
					break
			
		clicked_tile_to = []
		clicked_tile.deactivate()
		clicked_tile = null
		if reclick:
			_on_tile_clicked(tile)
	pass

func _on_valid_move(from : Tile, to : Tile) -> void:
	available_moves = []
	var user_move : PackedInt64Array = [from.tile_index, to.tile_index]
	var game_result = move_generator.send_user_move(user_move)[0]
	if (game_result != 0):
		terminate_game(game_result)
		return
	var bot_move : PackedInt64Array = move_generator.request_bot_move()
	game_result = bot_move[2]
	bot_move.remove_at(2)
	var move_tile : Array[Tile] = numerical_moves_to_tiles(bot_move)
	move_tile[0].send_bee(move_tile[1])
	if (game_result != 0):
		terminate_game(game_result)
		return
	available_moves = numerical_moves_to_tiles(move_generator.available_moves())
	
func terminate_game(game_result) -> void:
	if (game_result == 1):
		print("White wins")
	elif (game_result == 2):
		print("Black wins")
	elif (game_result == 3):
		print("Draw")
	else:
		printerr("unknown result type: ", game_result)
	
