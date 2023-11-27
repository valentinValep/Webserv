#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <poll.h>
#include <sys/poll.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>

struct test
{
	std::string str;
};


int	main()
{
	// ### With a file
	//int filefd = open("test.txt", O_RDONLY);
	//if (filefd == -1)
	//{
	//	perror("open");
	//	return (1);
	//}

	// ### With a socket
	int filefd = socket(AF_INET, SOCK_STREAM, 0);
	if (filefd == -1)
	{
		perror("socket");
		return (1);
	}

	if (fcntl(filefd, F_SETFL, O_NONBLOCK) == -1)
	{
		perror("fcntl");
		return (1);
	}

	if (setsockopt(filefd, SOL_SOCKET, SO_REUSEADDR, (int []){1}, sizeof(int)) == -1)
	{
		perror("setsockopt");
		return (1);
	}

	struct sockaddr_in	address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(8080);
	if (bind(filefd, (struct sockaddr*)&address, sizeof(address)) == -1)
	{
		perror("bind");
		return (1);
	}

	if (listen(filefd, 10) == -1)
	{
		perror("listen");
		return (1);
	}

	// ###############################################

	// ### WITH EPOLL

	int epollfd = epoll_create(1);
	if (epollfd == -1)
	{
		perror("epoll_create");
		return (1);
	}

	struct test *test = new struct test;
	{
		struct epoll_event event;

		event.events = EPOLLIN;
		event.data.ptr = test;
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, filefd, &event) == -1)
		{
			perror("epoll_ctl");
			return (1);
		}
	}

	test->str = "test";

	struct epoll_event events[1];

	int nb = epoll_wait(epollfd, events, 1, -1);
	if (nb == -1)
	{
		perror("epoll_wait");
		return (1);
	}

	for (int i = 0; i < nb; i++)
	{
		std::cout << "events[i].events: " << events[i].events << std::endl;
		std::cout << "events[i].data.ptr: " << ((struct test*)events[i].data.ptr)->str << std::endl;
	}

	close(epollfd);
	delete test;

	// ### WITH POLL

	//struct pollfd pfd[1];
	//pfd[0].fd = filefd;
	//pfd[0].events = POLLIN;

	//int nb = poll(pfd, 1, -1);

	//for (int i = 0; i < nb; i++)
	//{
	//	std::cout << "pfd[i].fd: " << pfd[i].fd << std::endl;
	//	std::cout << "pfd[i].events: " << pfd[i].events << std::endl;
	//	std::cout << "pfd[i].revents: " << pfd[i].revents << std::endl;
	//}

	return (0);
}