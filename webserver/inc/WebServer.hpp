#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include <iostream>
# include <fstream>
# include <vector>
# include <string>
# include <cerrno>
# include <cstring>
# include "Server.hpp"

class WebServer
{
	public:
		WebServer();
		WebServer(const std::string &file);
		~WebServer();
		
		void parseConfigFile(const std::string &file);
	
	private:
		std::string _configData;
		std::vector<Server> _servers;
};

#endif