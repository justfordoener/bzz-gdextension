extends Node

func _ready() -> void:
	var move_generator : MoveGenerator = MoveGenerator.new()
	var zwei = move_generator.print_type(move_generator)
	print(zwei)
