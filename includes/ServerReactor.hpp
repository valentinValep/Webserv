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
	int					addClient(int socket_fd);
	void				deleteConnection(int socket_fd);
	void				run();
};
