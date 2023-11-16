/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequest.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/15 13:59:35 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/16 14:30:12 by chmadran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIREQUEST_HPP
# define CGIREQUEST_HPP

# include "ClientRequest.hpp"
# include <string>
# include <map>
# include <sys/wait.h>
# include <cstring>
# include <unistd.h>
# include <stdlib.h>

class CgiRequest {
	public:
		CgiRequest(ClientRequest &request);
		~CgiRequest();

		void			setEnv(ClientRequest &request);
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