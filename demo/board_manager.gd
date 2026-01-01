class_name BoardManager extends Node

@export
var tile_size = 64
var tiles = []
var tile_resource = preload("res://tile.tscn")
var bee_resource = preload("res://bee.tscn")
var available_moves = []
var clicked_tile = null
var clicked_tile_to : Array[Tile]
const num_ranks = 7
const num_files = 9
const max_tiles_per_file = 7
const coordinate_offset = 64
const start_positions = ["a1", "b1", "c1", "d1", "d7", "e7", "f7", "g7"]

func _ready() -> void:
	var move_generator: MoveGenerator = MoveGenerator.new()

	for x in range(num_ranks): # 0-8
		for y in range(num_files): # a-g
			if (x > y + 2 or y > x + 4):
				continue
			var new_tile = tile_resource.instantiate()
			new_tile.link_board_manager(self)
			new_tile.connect("tile_clicked", _on_tile_clicked)
			new_tile.position = Vector2(x * tile_size + (num_files - y) * tile_size / 2, - y * tile_size - coordinate_offset)

			var file := char("a".unicode_at(0) + x)
			var rank := str(y)

			new_tile.tile_position = file + rank
			add_child(new_tile)
			tiles.append(new_tile)
			
	available_moves = move_generator.available_moves(move_generator)
	available_moves = numerical_moves_to_tiles(available_moves)
	print("number of available moves: ", available_moves.size())
	
	# spawn bees
	for pos in start_positions:
		var bee = bee_resource.instantiate()
		print(pos)
		var tile = get_tile_by_position(pos)
		tile.bee = bee
		bee.position = tile.position
		

func index_to_tile_position(index: int) -> String:
	var rank = str(int(float(num_ranks - index) / 7 + 1.0))
	var file = char("a".unicode_at(0) + (index % 7) - 1)
	return file + rank

func get_tile_by_position(pos: String) -> Tile:
	for tile in tiles:
		if tile.tile_position == pos:
			return tile
	return null

func numerical_moves_to_tiles(moves: Array[int]) -> Array[Tile]:
	var result: Array[Tile] = []

	for i in range(0, moves.size(), 2):
		var from_index := moves[i]
		var to_index := moves[i + 1]

		var from_pos := index_to_tile_position(from_index)
		var to_pos := index_to_tile_position(to_index)

		var from_tile := get_tile_by_position(from_pos)
		var to_tile := get_tile_by_position(to_pos)

		if from_tile == null or to_tile == null:
			push_error("Invalid move: %s -> %s" % [from_pos, to_pos])
			continue

		result.append(from_tile)
		result.append(to_tile)

	return result
		
	
func _on_tile_clicked(tile : Tile) -> void:
	print(tile.tile_position)
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
		# TODO: check if move goes through portal
		if tile in clicked_tile_to:
			clicked_tile.send_bee(tile)
			print("valid move")
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
