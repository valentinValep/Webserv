/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:40:38 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/02 14:47:23 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_MANAGER_HPP
# define SERVER_MANAGER_HPP

# include <string>
# include <vector>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <cstdlib>
# include <cstring>
# include <cstring>
# include <fstream>
# include <iostream>
# include <algorithm>
# include "ClientRequest.hpp"
# include "ServerResponse.hpp"

class ServerManager {
public:
	ServerManager();
	void start();

protected:
	int server_fd;
	struct sockaddr_in address;

	void setupNetwork();
	void handleClientRequest(int clientSocket);
};

#endif
