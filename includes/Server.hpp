/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/31 14:40:38 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/01 14:42:45 by chmadran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

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
# include "ConfigSettings.hpp"
# include "ClientRequest.hpp"


class Server {
public:
	Server(const ConfigSettings& settings);
	void start();

private:
	ConfigSettings settings;
	int server_fd;
	struct sockaddr_in address;
	void setupNetwork();
	void handleClientRequest(int clientSocket);
	std::string readFileContent(const std::string& filePath);
	void sendHttpResponse(int clientSocket, const std::string& content);
};

#endif
