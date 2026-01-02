class_name Bee extends Node

var tile_position : Tile
var right_end : int = 1200
var left_end : int = -1200

func move_to_tile(tile_destination: Tile) -> void:
	var tween := create_tween()

	var dir : Vector2 = tile_destination.position - self.position
	var target_rotation := dir.angle()
	print(target_rotation)
	
	if abs(int(rad_to_deg(target_rotation))) % 30 > 3 and abs(int(rad_to_deg(target_rotation))) % 30 < 27: # portal
		print("I take a portal")
		print(abs(int(target_rotation)))
		var middle_target = Vector2(left_end, self.position.y)
		if target_rotation > 0: # up means going out to the right
			middle_target = Vector2(right_end, self.position.y)
			
		target_rotation = (middle_target - self.position).angle()
		tween.parallel().tween_property(
			self,
			"rotation",
			target_rotation,
			0.07
		)
		tween.parallel().tween_property(
			self,
			"position",
			middle_target,
			0.2
		)
		await tween.finished
		dir = tile_destination.position - self.position
		target_rotation = dir.angle()
		self.position = Vector2(-self.position.x, tile_destination.position.y)
		await move_to_tile(tile_destination)
		return

	tween.parallel().tween_property(
		self,
		"rotation",
		target_rotation,
		0.15
	)

	tween.parallel().tween_property(
		self,
		"position",
		tile_destination.position,
		0.4
	)

	await tween.finished

	tile_position = tile_destination
	tile_destination.bee = self
