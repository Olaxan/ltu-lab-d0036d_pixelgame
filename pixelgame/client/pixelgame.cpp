// pixelgame.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <string>

#define ASIO_STANDALONE
#include "asio/asio.hpp"

#include "client_mgr.h"
#include "userinput/uinput.h"

asio::io_service io_service;
asio::ip::tcp::resolver resolver(io_service);

constexpr auto service_name = "pixelgame";
constexpr auto canvas_addr = "::1";
constexpr auto canvas_port = "4445";

int main()
{
	bool running = true;
	asio::ip::tcp::socket socket(io_service);
	
	while (running)
	{
		efiilj::UserInput<std::string> address_query("Enter server name/ip:", "> ");
		efiilj::UserInput<std::string> service_query("Enter port number/service name: ", "> ");

		if (address_query.Show())
		{
			if (address_query.Value() == "exit")
				running = false;
			else
			{
				if (service_query.Show())
				{
					std::cout << "Connecting to " << address_query.Value() << "..." << std::endl;

					try
					{
						auto endpoint_iterator = resolver.resolve(address_query.Value(), service_query.Value());
						asio::connect(socket, endpoint_iterator);
					}
					catch (asio::system_error& e)
					{
						std::cout << "Failed to connect: " << e.what() << std::endl;
						continue;
					}

					std::cout << (socket.is_open() ? "Connection established!" : "Could not connect - unknown error") << std::endl;

					efiilj::UserInput<std::string> name_query("Enter player name: ", "> ");
					efiilj::UserInput<int> form_query("Enter player form (0 = cube, 1 = sphere, 2 = pyramid, 3 = cone): ",
						"> ", 0, 3);

					if (name_query.Show() && form_query.Show())
					{
						std::string name = name_query.Value();
						auto form = static_cast<object_form>(form_query.Value());
						
						asio::error_code err;
						client_mgr manager(socket, canvas_addr, canvas_port);
						
						auto player = client(name, form, object_desc::human);
						
						if (manager.join(player, err))
						{
							std::cout << "Joined successfully!" << std::endl;
							if (manager.start())
								return 0;
						}
						else
						{
							std::cout << "Failed to join match: " << err.message() << std::endl;
						}
					}
				}
			}
		}
	}

	socket.close();
	return 0;
}


