#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include "libraries.hpp"
#include "Server.hpp"
#include "Request.hpp"

#define MAXCLIENTS 150

class Server;

class WebServer {
	public:
		WebServer();
		WebServer(const std::string &file);
		WebServer(const WebServer &other);
		WebServer& operator=(const WebServer &other);
		~WebServer();

		bool parseConfigFile(const std::string &file);
		void initService();
		void createNewClient(int it_listen);
		const Server &getServerConfig(char *buffer);
		bool correctConfig() const;
		bool continueServer(char *buffer);

		void checkServes();
		void checkClients();

	private:
		std::vector<Server>	                _servers;
		std::vector<pollfd>			        _poll_fds;
		size_t 								_pollSize;
		std::vector<int>			        _listeners;
		size_t							    _listSize;
		std::map<int, std::vector<Server> > _portsMap;
        bool                                _correctConfig;
		bool 								_running;
		std::string 						_response;
};

#endif
