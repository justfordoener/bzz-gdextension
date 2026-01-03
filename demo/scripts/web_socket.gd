class_name WebSocketManager extends Node

@export var websocket_url := "wss://0i5scnug9h.execute-api.eu-north-1.amazonaws.com/production/"
@onready var user_interface = $"../UserInterface"
@onready var socket := WebSocketPeer.new()
var current_state := WebSocketPeer.STATE_CLOSED
var template_message = {
	"body" = "None"
}

func _ready() -> void:
	user_interface.websocket = self
	print("Connecting to WebSocket...")
	socket.connect_to_url(websocket_url)

func _process(_delta: float) -> void:
	if socket.get_ready_state() == WebSocketPeer.STATE_CLOSED:
		return

	socket.poll()

	if socket.get_ready_state() != current_state:
		current_state = socket.get_ready_state()
		match current_state:
			WebSocketPeer.STATE_CONNECTING:
				print("WebSocket state: CONNECTING")
			WebSocketPeer.STATE_OPEN:
				print("WebSocket state: OPEN")
				send_message(template_message, "get_connection_id")
			WebSocketPeer.STATE_CLOSING:
				print("WebSocket state: CLOSING")
			WebSocketPeer.STATE_CLOSED:
				print("WebSocket state: CLOSED")

	if current_state == WebSocketPeer.STATE_OPEN:
		while socket.get_available_packet_count() > 0:
			var raw := socket.get_packet().get_string_from_utf8()
			var json := JSON.new()
			if json.parse(raw) == OK:
				print("Parsed JSON:", json.data)
				_process_message(json.data)
			else:
				print("RAW message received:", raw)

func _process_message(message) -> void:
	if message is not Dictionary:
		Data.connection_id = message
	elif message.has("message"):
		if message.message == "Internal server error":
			printerr(message)
	elif message.mode == "lobby_query":
		Data.lobby_ids = message.lobby_ids
	elif message.mode == "connect":
		Data.connection_id = message.connection_id
	
func send_message(message : Dictionary, route : String):
	var payload = {
		"action": "none",
		"message": message
	}
	payload.action = route
	if socket.get_ready_state() == WebSocketPeer.STATE_OPEN:
		var error = socket.send_text(JSON.stringify(payload))
		print("sent message: " + JSON.stringify(payload, "\t"))
		if error != OK:
			print("error: " + str(error))
	else:
		print("couldn't send message, websocket state is: " + str(socket.get_ready_state()))
