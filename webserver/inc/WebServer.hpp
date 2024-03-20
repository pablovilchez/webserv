#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include "libraries.hpp"
#include "Server.hpp"
#include "Request.hpp"

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

		bool parseConfigFile(const std::string &file);
		void initService();
		const Server &getServerConfig(char *buffer);
        bool correctConfig();

	private:
		std::vector<Server>	                _servers;
		std::vector<pollfd>			        _poll_fds;
		std::vector<int>			        _listeners;
		std::map<int, std::vector<Server> > _portsMap;
        bool                                _correctConfig;
};

#endif
