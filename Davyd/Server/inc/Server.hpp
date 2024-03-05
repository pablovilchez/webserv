#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <csignal>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <vector>
#include <map>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <poll.h>

class Server {
	private:
		int					_listen_sock;
		struct sockaddr_in	_server_addr;
		std::vector<pollfd> _poll_fds;

		void _fillServerAddr();
		void _startServerLoop();

	public:
		Server();
		Server(Server const &src);
		Server &operator=(Server const &src);
		~Server();
		void startServer();
};

#endif
