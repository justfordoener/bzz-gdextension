class_name BoardManager extends Node

var tiles = []
var tile_resource = preload("res://tile.tscn")

func _ready() -> void:
	var move_generator : MoveGenerator = MoveGenerator.new()
	var zwei = move_generator.print_type(move_generator)
	print(zwei)
	
	var new_tile = tile_resource.instantiate()
	new_tile.link_board_manager(self)
	new_tile.connect("tile_clicked", _on_tile_clicked)
	new_tile.position = Vector2(-10, -10)
	new_tile.tile_position = "1a"
	add_child(new_tile)
	tiles.append(new_tile)
	
func _on_tile_clicked(tile : Tile) -> void:
	print(tile.tile_position)
	pass
