#include "ServerReactor.hpp"
#include "ServerManager.hpp"
#include "AcceptHandler.hpp"
#include "ProcessHandler.hpp"
#include "CgiChildHandler.hpp"
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cerrno>
#include <cstring>

/************************************************************
 *					Constructors & Destructor				*
 ************************************************************/

ServerReactor::ServerReactor(): epoll_fd(-1) {}

ServerReactor::ServerReactor(std::vector<Server> &servers)
{
	// Create epoll instance
	errno = 0;
	this->epoll_fd = epoll_create(EPOLL_CREATION_SOCKET_COUNT);
	if (this->epoll_fd == -1)
		throw std::runtime_error("epoll_create() failed: " + std::string(strerror(errno)));

	// Add servers to epoll
	this->setupNetwork(servers);
}

void ServerReactor::setupNetwork(std::vector<Server> &servers)
{
	std::set<int> 		listening_ports;
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		struct sockaddr_in	address;
		int					server_fd;

		if (listening_ports.find(it->getPort()) != listening_ports.end())
			continue;

		// Create socket
		errno = 0;
		server_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (server_fd == -1)
			throw std::runtime_error("socket() failed: " + std::string(strerror(errno)));

		// Set to non-blocking
		errno = 0;
		if (fcntl(server_fd, F_SETFL, O_NONBLOCK) == -1)
			throw std::runtime_error("fcntl() failed: " + std::string(strerror(errno)));

		// Set SO_REUSEADDR
		errno = 0;
		if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (int []){1}, sizeof(int)) == -1)
			throw std::runtime_error("setsockopt() failed: " + std::string(strerror(errno)));

		// Bind
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(it->getPort());
		errno = 0;
		if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == -1)
			throw std::runtime_error("bind() failed: " + std::string(strerror(errno)));

		// Listen
		errno = 0;
		if (listen(server_fd, QUEUE_SIZE) == -1)
			throw std::runtime_error("listen() failed: " + std::string(strerror(errno)));

		// Add to epoll
		EventHandler	*handler = new AcceptHandler(server_fd, it->getPort());
		struct epoll_event	event;
		event.events = EPOLLIN;
		event.data.ptr = handler;
		errno = 0;
		if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1)
			throw std::runtime_error("epoll_ctl() failed: " + std::string(strerror(errno)));

		this->event_handlers.push_back(handler);
		listening_ports.insert(it->getPort());
	}
}

ServerReactor::~ServerReactor()
{
	close(this->epoll_fd);

	for (std::vector<EventHandler*>::iterator it = this->event_handlers.begin(); it != this->event_handlers.end(); it++)
		delete *it;
}

/************************************************************
 *							Methods							*
 ************************************************************/

int	ServerReactor::addClient(int socket_fd, int port)
{
	if (this->event_handlers.size() >= MAX_CONNECTION)
	{
		std::cout << "🛑 Max connections reached" << std::endl;
		return -1;
	}

	// Add to epoll
	EventHandler	*handler = new ProcessHandler(socket_fd, port);
	struct epoll_event	event;
	event.events = EPOLLIN;
	event.data.ptr = handler;
	errno = 0;
	if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, socket_fd, &event) == -1)
	{
		perror(SCSTR(__FILE__ << ":" << __LINE__ << " epoll_ctl() failed"));
		close(socket_fd);
		delete handler;
		return -1;
	}

	this->event_handlers.push_back(handler);
	return 0;
}

int ServerReactor::addCgiChild(int child_fd, int parent_fd, EventHandler &parent_handler)
{
	EventHandler	*handler = new CgiChildHandler(child_fd, parent_fd, parent_handler);
	struct epoll_event	event;
	event.events = EPOLLIN;
	event.data.ptr = handler;
	errno = 0;
	if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, child_fd, &event) == -1)
	{
		perror(SCSTR(__FILE__ << ":" << __LINE__ << " epoll_ctl() failed"));
		close(child_fd);
		delete handler;
		return -1;
	}

	this->event_handlers.push_back(handler);
	return 0;
}

void ServerReactor::deleteClient(int socket_fd)
{
	errno = 0;
	std::cout << "Deleting client " << socket_fd << std::endl;
	if (epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, socket_fd, NULL) == -1)
		return perror(SCSTR(__FILE__ << ":" << __LINE__ << " epoll_ctl() failed"));

	for (std::vector<EventHandler*>::iterator it = this->event_handlers.begin(); it != this->event_handlers.end(); it++)
	{
		if ((*it)->getSocketFd() == socket_fd)
		{
			delete *it;
			this->event_handlers.erase(it);
			break ;
		}
	}
}

void ServerReactor::listenClient(int socket_fd, EventHandler &handler)
{
	struct epoll_event	event;

	event.events = EPOLLIN;
	event.data.ptr = &handler;
	errno = 0;
	if (epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, socket_fd, &event) == -1)
		return perror(SCSTR(__FILE__ << ":" << __LINE__ << " epoll_ctl() failed"));
}

void ServerReactor::talkToClient(int socket_fd, EventHandler &handler)
{
	struct epoll_event	event;

	event.events = EPOLLOUT;
	event.data.ptr = &handler;
	errno = 0;
	if (epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, socket_fd, &event) == -1)
		return perror(SCSTR(__FILE__ << ":" << __LINE__ << " epoll_ctl() failed"));
}

void ServerReactor::ignoreClient(int socket_fd)
{
	struct epoll_event	event;

	event.events = 0;
	errno = 0;
	if (epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, socket_fd, &event) == -1)
		return perror(SCSTR(__FILE__ << ":" << __LINE__ << " epoll_ctl() failed"));
}

void ServerReactor::run()
{
	struct epoll_event	events[MAX_CONNECTION];
	int					event_count;

	while (true)
	{
		errno = 0;
		event_count = epoll_wait(this->epoll_fd, events, MAX_CONNECTION, 10000);
		if (event_count == -1)
			std::cerr << __FILE__ << ":" << __LINE__ << " epoll_wait(): " << strerror(errno) << std::endl;

		for (int i = 0; i < event_count; i++)
		{
			EventHandler	*handler = static_cast<EventHandler*>(events[i].data.ptr);

			std::cout << "🔥 Event on socket_fd: " << handler->getSocketFd() << std::endl;
			handler->handle();
		}
		if (event_count == 0)
		{
			std::vector<EventHandler *>	handlers = this->event_handlers;

			std::cout << "🕑 Timeout time" << std::endl; // @TODO pick right moment
			for (std::vector<EventHandler*>::iterator it = handlers.begin(); it != handlers.end(); it++)
				(*it)->checkTimeout();
		}
	}
}
