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

int main()
{
	int opt = true;
	int master_socket, addrlen, new_socket, client_socket[max_clients], i, valread, sd;
	struct sockaddr_in address {};

	unsigned int id = 4;
	std::map<unsigned int, client> players;

	char message[] = "Server version 0.1";

	// Receive data buffer
	char buffer[1025];

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

	// Attempt to bind socket - repeat 'i' times if not successful
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

			printf("New connection - socket fd %d, ip: %s, port: %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
			
			if (send(new_socket, &message, sizeof message, 0) != sizeof message)
				perror("Transmission error");
			else 
				puts("Welcome message sent successfully");

			for (i = 0; i < max_clients; i++)
			{
				if (client_socket[i] == 0)
				{
					client_socket[i] = new_socket;
					printf("Adding to list of sockets as %d\n", i);

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

				if ((valread = read(sd, buffer, 1024)) == 0)
				{
					getpeername(sd, reinterpret_cast<struct sockaddr*>(&address), 
						reinterpret_cast<socklen_t*>(&addrlen));
					printf("Host disconnected , ip %s , port %d \n",
						inet_ntoa(address.sin_addr), ntohs(address.sin_port));

					close(sd);
					client_socket[i] = 0;
				}
				else
				{
					printf("Message from %s \n", inet_ntoa(address.sin_addr));
					buffer[valread] = '\0';
					send(sd, buffer, strlen(buffer), 0);
				}
			}
		}
	}

	return 0;
}