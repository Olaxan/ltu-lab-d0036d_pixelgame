#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <ctime>
#include <map>

#include "protocol.h"
#include "client.h"

constexpr unsigned int port_range_begin = 49152;
constexpr unsigned int port_range_end = 49160;
constexpr unsigned int max_clients = 64;
constexpr unsigned int max_move_dist = 2;
constexpr unsigned int buffer_size = 1024;

unsigned int next_id = 4;
std::map<unsigned int, client> players;
std::map<unsigned int, int> sockets;
std::map<coordinate, unsigned int> board;

int summary(const unsigned int id) noexcept
{
	
	const auto cli_it = players.find(id);
	if (cli_it == players.end())
		return -1;

	client* target = &cli_it->second;

	int byte_count = 0;
	for (auto it = players.begin(); it != players.end(); ++it)
	{
		client* player = &it->second;
		
		new_player_msg first
		{
			change_msg
			{
				msg_head {sizeof(new_player_msg), target->seq_no, id, msg_type::change },
				change_type::new_player
			}, player->desc, player->form
		};
		memcpy(first.name, player->name, max_name_len);
		target->seq_no++;
		
		new_player_position_msg second
		{
			change_msg
			{
				msg_head {sizeof(new_player_position_msg), target->seq_no, id, msg_type::change },
				change_type::new_player_position
			}, player->position
		};
		target->seq_no++;

		byte_count += send(sockets[id], &first, sizeof(new_player_msg), 0);
		byte_count += send(sockets[id], &second, sizeof(new_player_position_msg), 0);
	}

	return byte_count;
}

int broadcast(const unsigned int join_id, const change_type type) noexcept
{
	const auto cli_it = players.find(join_id);
	if (cli_it == players.end())
		return -1;

	client* player = &cli_it->second;

	int byte_count = 0;
	for (auto it = players.begin(); it != players.end(); ++it)
	{
		const int id = it->second.id;
		if (id == 0)
			continue;

		switch (type)
		{
		case change_type::new_player:
		{
			new_player_msg msg
			{
				change_msg
				{
					msg_head {sizeof(new_player_msg), it->second.seq_no++, join_id, msg_type::change },
					change_type::new_player
				}, player->desc, player->form
			};
			memcpy(msg.name, player->name, max_name_len);

			byte_count += send(sockets[id], &msg, sizeof(new_player_msg), 0);
			break;
		}
		case change_type::player_leave:
		{
			player_leave_msg msg
			{
				change_msg
				{
					msg_head {sizeof(player_leave_msg), it->second.seq_no++, join_id, msg_type::change },
					change_type::player_leave
				}
			};

			byte_count += send(sockets[id], &msg, sizeof(player_leave_msg), 0);
			break;
		}
		case change_type::new_player_position:
		{
			new_player_position_msg msg
			{
				change_msg
				{
					msg_head {sizeof(new_player_position_msg), it->second.seq_no++, join_id, msg_type::change },
					change_type::new_player_position
				}, player->position
			};

			byte_count += send(sockets[id], &msg, sizeof(new_player_position_msg), 0);
			break;
		}
		default: ;
		}
	}

	return -1;
}

coordinate get_first_free() noexcept
{
	coordinate c{ -100, -100 };

	for ( ; c.x < 100; c.x++)
	{
		for ( ; c.y < 100; c.y++)
		{
			auto it = board.find(c);
			if (it == board.end())
			{
				printf("Location %d, %d is free\n", c.x, c.y);
				return c;
			}
		}
	}

	printf("Warning - no free space on board \n");
	return c;
	//TODO: I guess some error handling if the board actually has 40 000 players simultaneously?
}

int move(const unsigned int id, const coordinate pos) noexcept
{
	auto it = players.find(id);
	
	if ((it != players.end()) && (board.find(pos) == board.end()))
	{
		client* player = &it->second;
		const int dist = player->position.dist(pos);
		
		if (dist <= max_move_dist && (pos.x >= -100 && pos.x <= 100 && pos.y >= -100 && pos.y <= 100))
			player->position = pos;
		else
			printf("Move invalid: %d, %d -> %d, %d (distance %d) \n", player->position.x, player->position.y, pos.x, pos.y, dist);

		return broadcast(it->first, change_type::new_player_position);
	}

	return -1;
}

int main()
{
	int opt = true;
	int master_socket, addrlen, new_socket, client_socket[max_clients], i, valread, sd;
	struct sockaddr_in address {};

	// Receive data buffer
	char buffer[buffer_size + 1];

	fd_set readfds;

	// Initialize sockets
	for (i = 0; i < max_clients; i++)
	{
		client_socket[i] = 0;
	}

	// Setup socket
	if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("Failed to create socket");
		exit(EXIT_FAILURE);
	}

	// Setup socket options - reuse address
	if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&opt), sizeof(opt)) < 0)
	{
		perror("Failed to setup socket options");
		exit(EXIT_FAILURE);
	}

	// Setup address options
	int port = port_range_begin;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	// Attempt to bind socket - test all sockets in range until successful, or fail
	while (bind(master_socket, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) < 0)
	{
		if (port > port_range_end)
		{
			perror("Failed to bind port");
			exit(EXIT_FAILURE);
		}
		
		address.sin_port = htons(port++);
	}

	printf("Listener on port %d \n", port);
 
	if (listen(master_socket, 3) < 0)
	{
		perror("Failed to listen on port");
		exit(EXIT_FAILURE);
	}
 
	addrlen = sizeof(address);
	puts("Waiting for connections...");

	while (true)
	{

		// Clear socket set and add master socket
		FD_ZERO(&readfds); 
		FD_SET(master_socket, &readfds);
		int max_sd = master_socket;

		// Add client sockets to set
		for (i = 0; i < max_clients; i++)
		{

			sd = client_socket[i];

			if (sd > 0)
				FD_SET(sd, &readfds);
 
			if (sd > max_sd)
				max_sd = sd;
		}

		// Wait for socket activity - no timeout
		const int activity = select(max_sd + 1, &readfds, nullptr, nullptr, nullptr);

		if ((activity < 0) && (errno != EINTR))
		{
			perror("Selector error");
		}

		// Respond to activity on master socket
		if (FD_ISSET(master_socket, &readfds))
		{
			// Attempt to accept new connection
			if ((new_socket = accept(master_socket, reinterpret_cast<struct sockaddr*>(&address), reinterpret_cast<socklen_t*>(&addrlen))) < 0)
			{
				perror("Accept error");
				exit(EXIT_FAILURE);
			}

			printf("New connection - socket fd %d, ip %s, port %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

			for (i = 0; i < max_clients; i++)
			{
				if (client_socket[i] == 0)
				{
					client_socket[i] = new_socket;
					printf("Socket number %d \n", i);

					break;
				}
			}
		}

		// Main reception loop
		for (i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];

			if (FD_ISSET(sd, &readfds))
			{

				if ((valread = read(sd, buffer, buffer_size)) == 0)
				{
					getpeername(sd, reinterpret_cast<struct sockaddr*>(&address), 
						reinterpret_cast<socklen_t*>(&addrlen));
					printf("Host disconnected, ip %s, port %d \n",
						inet_ntoa(address.sin_addr), ntohs(address.sin_port));

					close(sd);
					client_socket[i] = 0;
				}
				else if (valread > -1)
				{
					//printf("Message from %s (%d bytes) \n", inet_ntoa(address.sin_addr), valread);

					msg_head head{};
					std::memcpy(&head, buffer, valread);

					switch (head.type)
					{
						case msg_type::join:
						{
							join_msg msg{};
							std::memcpy(&msg, buffer, sizeof(join_msg));
							printf("Player %s joined, desc %d, form %d \n", msg.name, msg.desc, msg.form);

							msg_head response{ sizeof(msg_head), 1, next_id, msg_type::join };
							if (send(sd, &response, sizeof(msg_head), 0) == sizeof(msg_head))
							{
								coordinate cord = get_first_free();
								players[next_id] = client(msg, next_id, cord);
								sockets[next_id] = sd;
								board[cord] = next_id;
								summary(next_id);
								broadcast(next_id, change_type::new_player);
								broadcast(next_id, change_type::new_player_position);
								next_id++;
							}
							break;
						}
						case msg_type::leave:
						{
							leave_msg msg{};
							std::memcpy(&msg, buffer, sizeof(leave_msg));
							auto it = players.find(msg.head.id);
							if (it != players.end())
							{
								const int id = it->second.id;
								printf("Player %s (id %d) disconnected \n", it->second.name, id);
								players.erase(id);
								sockets.erase(id);
								board.erase(it->second.position);
								broadcast(id, change_type::player_leave);
							}
							break;
						}
						case msg_type::event:
						{
							move_event msg{};
							std::memcpy(&msg, buffer, sizeof(move_event));
							move(msg.event.head.id, msg.pos);
						}
						case msg_type::text_message:
						{
							break;
						}
						default: ;
					}

					buffer[valread] = '\0';
				}
			}
		}
	}

	return 0;
}