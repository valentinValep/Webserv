/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequest.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chmadran <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/15 13:59:35 by chmadran          #+#    #+#             */
/*   Updated: 2023/11/21 11:05:37 by chmadran         ###   ########.fr       */
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

		void			setEnv();
		void			executeScript();
		void			cgiChildProcess();
		void			cgiParentProcess();
		void			printResponse();
		void			convertEnv();
		void			setPath();
		void			freeEnvp();
		std::string		getResponse();

	private:
		ServerResponse						&serverResponse;
		std::string							_path;
		std::string							_queryString;
		std::string							_response;
		std::string							_cgiBody;
		const char*							_cgiInterpreter;
		const char*							_scriptName;
		char**								_envp;
		std::map<std::string, std::string>	_env;
		int									_fd[2];
		int									_pid;
		int									_child_pid;
};

#endif