#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include "libraries.hpp"
#include "Server.hpp"

class Server;

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
