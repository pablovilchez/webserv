#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include "libraries.hpp"
#include "Server.hpp"

#define MAXCLIENTS 150

class Server;

class WebServer {
	public:
		// WebServerOrtxCan.cpp;
		WebServer();
		WebServer(const std::string &file);
		WebServer(const WebServer &other);
		WebServer& operator=(const WebServer &other);
		~WebServer();

		void parseConfigFile(const std::string &file);
		void initService();

	private:
		std::vector<const Server*>	_servers;
		std::vector<pollfd>			_poll_fds;
		std::vector<int>			_listeners;
		std::map<int, std::vector<const Server*> > _portsMap;
};

#endif
