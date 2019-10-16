#pragma once
#include "protocol.h"
#include <string>

struct client
{
	client(): position(), form(), desc() { }

	client(std::string& name, const object_form form, const object_desc desc)
		: position(), form(form), desc(desc)
	{
		name.copy(this->name, name.size());
	}

	explicit client(new_player_msg& msg)
	: position(), form(msg.form), desc(msg.desc)
	{
		std::memcpy(name, msg.name, max_name_len);
	}

	explicit client(join_msg& msg)
		: position(), form(msg.form), desc(msg.desc)
	{
		std::memcpy(name, msg.name, max_name_len);
	}

	coordinate position;
	object_form form;
	object_desc desc;
	char name[max_name_len] {};

	int get_rgb() const
	{
		switch (form)
		{
		case object_form::cube: return 16711680;
		case object_form::sphere: return 65280;
		case object_form::pyramid: return 16776960;
		case object_form::cone: return 255;
		default: return 16777215;
		}
	}

	bool operator < (const client other) const
	{
		return memcmp(this, &other, sizeof(client)) > 0;
	};
};
