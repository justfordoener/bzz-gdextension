class_name Bee extends Node

var tile_position

func move_to_tile(tile_destination : Tile) -> void:
	tile_position = tile_destination
	self.position = tile_destination.position
	tile_destination.bee = self
