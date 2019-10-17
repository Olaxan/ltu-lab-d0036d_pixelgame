#pragma once

#define ASIO_STANDALONE
#include "asio/asio.hpp"

#include "client.h"
#include <string>
#include <map>

enum class directions
{
	up,
	down,
	left,
	right,
	none
};

enum class socket_state
{
	waiting,
	header_received,
	message_received,
	hold,
	idle,
	closing,
	closed
};

class client_mgr
{
private:
	
	asio::ip::tcp::socket endpoint_;
	std::map<int, client> players_;
	std::string canvas_address_, canvas_service_;
	std::thread update_listener_;
	unsigned int sequence_;
	unsigned int id_;
	bool is_running_;
	socket_state state_;

	static directions get_dir(const std::string& input);

	/**
	 * \brief Looping thread method for receiving server updates.
	 */
	void update();
	
	/**
	 * \brief Holding method for receiving a single server message.
	 */
	void receive();
	
	/**
	 * \brief Method for retrieving user input and handling it.
	 */
	void input();
	
	/**
	 * \brief Sends one or more move messages to the server.
	 * \param dir The desired direction of movement
	 * \param count The number of steps to take
	 */
	void move(directions dir, int count);
	
	/**
	 * \brief Sets up an ephemeral UDP socket and sends draw requests to the canvas program.
	 */
	void draw() const;
	
public:
	explicit client_mgr(asio::ip::tcp::socket& endpoint, std::string canvas_address, std::string canvas_service);
	
	/**
	 * \brief Check if the manager has an open TCP port and valid ID from server.
	 * \return Whether ready to communicate with server
	 */
	bool is_ready() const;
	
	/**
	 * \brief Sends player data to the server and attempts to join.
	 * \param player Player data; name, form, description
	 * \param err Any network error encountered while joining
	 * \return True if successful, false otherwise
	 */
	bool join(client player, asio::error_code& err);
	
	/**
	 * \brief Starts the manager loop, if a connection to the server exists.
	 * \return True upon successfully exiting the server, or false if the manager failed to start
	 */
	bool start();
};
