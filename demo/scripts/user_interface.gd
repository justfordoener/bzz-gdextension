extends Control

var websocket : WebSocketManager
var template_message = {
	"lobby" = ""
}

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass

func _on_multiplayer_button_pressed() -> void:
	websocket.send_message(template_message, "query_lobbies")


func _on_host_button_pressed() -> void:
	websocket.send_message(template_message, "host")


func _on_join_button_pressed() -> void:
	template_message.lobby = Data.lobby_id
	websocket.send_message(template_message, "join")
