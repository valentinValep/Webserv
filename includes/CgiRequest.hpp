/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequest.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vlepille <vlepille@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/15 13:59:35 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/17 12:46:48 by vlepille         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIREQUEST_HPP
# define CGIREQUEST_HPP

# include "ServerResponse.hpp"
# include <string>
# include <map>
# include <sys/wait.h>
# include <cstring>
# include <unistd.h>
# include <stdlib.h>

class CgiRequest {
	public:
		CgiRequest(ServerResponse &response);
		~CgiRequest();

		void			setEnv(ServerResponse &response);
		void			executeScript();
		void			cgiChildProcess();
		void			cgiParentProcess();
		void			printResponse();
		void			convertEnv();
		void			freeEnvp();
		std::string		getResponse();

	private:
		std::string							_response;
		char**								_envp;
		std::map<std::string, std::string>	_env;
		int									_fd[2];
		int									_pid;
		int									_child_pid;
};

#endif