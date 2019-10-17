#pragma once
#include <cmath>

constexpr int max_name_len = 32;

enum class object_desc
{
	human,
	non_human,
	vehicle,
	static_object
};

inline const char* to_string(const object_desc e)
{
	switch (e)
	{
	case object_desc::human: return "human";
	case object_desc::non_human: return "non human";
	case object_desc::vehicle: return "vehicle";
	case object_desc::static_object: return "static object";
	default: return "unknown";
	}
}

enum class object_form
{
	cube,
	sphere,
	pyramid,
	cone
};

inline const char* to_string(const object_form e)
{
	switch (e)
	{
	case object_form::cube: return "cube";
	case object_form::sphere: return "sphere";
	case object_form::pyramid: return "pyramid";
	case object_form::cone: return "cone";
	default: return "unknown";
	}
}

struct coordinate
{
	int x;
	int y;

	int dist(const coordinate other) const
	{
		return round(sqrt(pow(x - other.x, 2) + pow(y - other.y, 2)));
	}
	
	bool operator < (const coordinate other) const
	{
		return memcmp(this, &other, sizeof(coordinate)) > 0;
		//TODO: Replace with something better.
	};
};

struct draw_packet
{
	int x;
	int y;
	int rgb;
};

// Message head
enum class msg_type
{
	join,		 // Client joining game at server
	leave,		 // Client leaving game
	change,		 // Information to clients
	event,		 // Information from clients to server
	text_message // Send text messages to one or all
};

// Included first in all messages
struct msg_head
{
	unsigned int length;	// Total length for whole message
	unsigned int seq_no;	// Sequence number
	unsigned int id;		// Client ID or 0;
	msg_type type;			// Type of message
};

// Message type Join (Client -> Server)
struct join_msg
{
	msg_head head;
	object_desc desc;
	object_form form;
	char name[max_name_len];
};

// Message type Leave (Client -> Server)
struct leave_msg
{
	msg_head head;
};

// Message type Change (Server -> Client)
enum class change_type
{
	new_player,
	player_leave,
	new_player_position
};

// Included first in all Change messages
struct change_msg
{
	msg_head head;
	change_type type;
};

// Variations of ChangeMsg
struct new_player_msg
{
	change_msg msg; //Change message header with new client id
	object_desc desc;
	object_form form;
	char name[max_name_len];
};

struct player_leave_msg
{
	change_msg msg; //Change message header with new client id
};

struct new_player_position_msg
{
	change_msg msg; //Change message header
	coordinate pos; //New object position
	coordinate dir; //New object direction
};

// Messages of type Event (Client -> Server)
enum class event_type
{
	move
};

// Included first in all Event messages
struct event_msg
{
	msg_head head;
	event_type type;
};

// Variations of EventMsg
struct move_event
{
	event_msg event;
	coordinate pos; //New object position
	coordinate dir; //New object direction
};

// Messages of type TextMessage
struct text_message_msg
{
	msg_head head;
	char text[1];
};
