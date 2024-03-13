#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include "libraries.hpp"
#include "Server.hpp"

class Server;

class WebServer {
	public:
		WebServer();
		WebServer(const std::string &file);
		WebServer(const WebServer &other);
		WebServer& operator=(const WebServer &other);
		~WebServer();

		void parseConfigFile(const std::string &file);

	private:
		std::vector<const Server*> _servers;
		std::vector<pollfd> _poll_fds;
		std::map<int, std::vector<const Server*> > _portsMap;
};

#endif
