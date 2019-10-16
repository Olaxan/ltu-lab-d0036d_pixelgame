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

constexpr unsigned int port_range_begin = 49152;
constexpr unsigned int port_range = 5;
constexpr unsigned int max_clients = 64;

int main()
{
	int opt = true;
	int port = port_range_begin;
	int master_socket, addrlen, new_socket, client_socket[30], i, valread, sd;
	struct sockaddr_in address {};

	char buffer[1025];

	fd_set readfds;

	char* message = "Pixelgame server 1.0\n";

	for (i = 0; i < max_clients; i++)
	{
		client_socket[i] = 0;
	}

	if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("Failed to create socket");
		exit(EXIT_FAILURE);
	}
 
	if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt,
		sizeof(opt)) < 0)
	{
		perror("Failed to setup socket options");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	int attempts = 0;
	while (!(bind(master_socket, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) < 0))
	{
		if (attempts > port_range)
		{
			perror("Failed to bind port");
			exit(EXIT_FAILURE);
		}
		
		port++;
		address.sin_port = port;
		attempts++;
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

		FD_ZERO(&readfds); 
		FD_SET(master_socket, &readfds);
		int max_sd = master_socket;

		for (i = 0; i < max_clients; i++)
		{

			sd = client_socket[i];

			if (sd > 0)
				FD_SET(sd, &readfds);
 
			if (sd > max_sd)
				max_sd = sd;
		}

		const int activity = select(max_sd + 1, &readfds, nullptr, nullptr, nullptr);

		if ((activity < 0) && (errno != EINTR))
		{
			printf("Selector error");
		}

		if (FD_ISSET(master_socket, &readfds))
		{
			if ((new_socket = accept(master_socket,
				reinterpret_cast<struct sockaddr*>(&address), reinterpret_cast<socklen_t*>(&addrlen))) < 0)
			{
				perror("Accept error");
				exit(EXIT_FAILURE);
			}

			printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs 
				(address.sin_port));

			if (send(new_socket, message, strlen(message), 0) != strlen(message))
			{
				perror("Send error");
			}

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

		for (i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];

			if (FD_ISSET(sd, &readfds))
			{

				if ((valread = read(sd, buffer, 1024)) == 0)
				{
					
					getpeername(sd, reinterpret_cast<struct sockaddr*>(&address), \
						reinterpret_cast<socklen_t*>(&addrlen));
					printf("Host disconnected , ip %s , port %d \n",
						inet_ntoa(address.sin_addr), ntohs(address.sin_port));

					close(sd);
					client_socket[i] = 0;
				}
				else
				{
					buffer[valread] = '\0';
					send(sd, buffer, strlen(buffer), 0);
				}
			}
		}
	}

	return 0;
}