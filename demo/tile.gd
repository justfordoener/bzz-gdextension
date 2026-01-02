class_name Tile
extends Node2D

var board_manager: BoardManager
var tile_index: int
var bee : Bee

signal tile_clicked(tile: Tile)

@onready var visual := $TextureButton  # Sprite2D / ColorRect / TextureRect

var is_active := true
var is_highlighted := false

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

func deactivate() -> void:
	is_active = false
	visual.modulate = Color.WHITE

func activate() -> void:
	is_active = true
	visual.modulate = Color.BLACK

func highlight() -> void:
	is_highlighted = true
	visual.modulate = Color(0.6, 1.0, 0.6) # green tint
	
func unhighlight() -> void:
	is_highlighted = true
	visual.modulate = Color.WHITE

func send_bee(target_tile: Tile) -> void:
	bee.move_to_tile(target_tile)
	bee = null
	print("Moving bee from %s to %s" % [tile_index, target_tile.tile_index])
