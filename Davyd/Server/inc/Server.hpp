#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <csignal>
#include <poll.h>
#include <vector>
#include <signal.h>
#include <sys/wait.h>

class Server {
	private:
		int					_server_socket;
		struct sockaddr_in	_server_addr;


		void _setServerSocket();
		void _fillServerAddr();
		void _printServerInfo();
		void _startServerLoop();

	public:
		Server();
		Server(Server const &src);
		Server &operator=(Server const &src);
		~Server();
		void startServer();
};

#endif
