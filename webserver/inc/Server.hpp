#ifndef SERVER_HPP
#define SERVER_HPP

#include "WebServer.hpp"
#include "Config.hpp"

class Server {
	private:
		Config				 _config;
		int					_listen_sock;
		struct sockaddr_in	_server_addr;
		std::vector<pollfd> _poll_fds;

		void _fillServerAddr();
		void _startServerLoop();

	public:
		Server();
		Server(Server const &src);
		Server &operator=(Server const &src);
		Server(const std::string &config, int servNum);
		~Server();
		void startServer();
};

#endif
