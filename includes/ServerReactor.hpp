// FileName: ServerReactor.hpp

#pragma once

#include "EventHandler.hpp"
#include "Server.hpp"
#include <vector>

class ServerReactor {
private:
	// Attributes
	int								epoll_fd;
	std::vector<EventHandler *>		event_handlers;

	void setupNetwork(std::vector<Server> &servers);
public:
	// Constructors & Destructor
	ServerReactor();
	ServerReactor(std::vector<Server> &servers);
	~ServerReactor();
	// Getters

	// Setters

	// Methods
	int					addClient(int socket_fd, int port);
	void				deleteClient(int socket_fd);
	void				listenClient(int socket_fd, EventHandler &handler);
	void				talkToClient(int socket_fd, EventHandler &handler);
	void				run();
};
