class_name Tile extends Node2D

var board_manager : BoardManager
var tile_position : String
signal tile_clicked(tile: Tile)

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.

func link_board_manager(bm: BoardManager):
	board_manager = bm

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass

func _on_pressed() -> void:
	emit_signal("tile_clicked", self)
	pass
