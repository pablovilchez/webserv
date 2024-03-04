#include "Server.hpp"

const int MAX_EVENTS = 10;
const int PORT = 8080;

Server::Server() {
	// Constructor
}

Server::Server(Server const &src) {
	// Copy constructor
	*this = src;
}

Server &Server::operator=(Server const &src) {
	// Assignment operator
	(void)src;
	return *this;
}

Server::~Server() {
	// Destructor
}

void Server::_setServerSocket() {
	_server_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (_server_socket == -1) {
		perror("Can't create a socket");
		exit(1);
	}
}

void Server::_fillServerAddr() {
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(PORT);
	_server_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(_server_addr.sin_zero), '\0', 8);
}

void Server::_printServerInfo() {
	char hostName[1024];
    gethostname(hostName, 1024);
    std::cout << "Server started on " << hostName << std::endl;

	hostent* host = gethostbyname(hostName);
	if (host == NULL) {
		herror("gethostbyname");
		exit(1);
	}

	std::cout << "IP Address: " << inet_ntoa(*(in_addr*)*host->h_addr_list) << std::endl;
}

void Server::_startServerLoop() {
	std::vector<struct pollfd> _fds[MAX_EVENTS];

}

void Server::startServer() {
	// Create Socket
	_setServerSocket();
	_fillServerAddr();
	int yes = 1;
	if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR,  &yes, sizeof(int)) == -1) {
		perror("setsockopt failed");
    	exit(1);
	}

	if (setsockopt(_server_socket, SOL_SOCKET, SO_BROADCAST,  &yes, sizeof(int)) == -1) {
		perror("setsockopt failed");
    	exit(1);
	}

	if(bind(_server_socket, reinterpret_cast<sockaddr*>(&_server_addr), sizeof(_server_addr)) == -1) {
		perror("Can't bind to IP/port");
		exit(1);
	}

	if (listen(_server_socket, SOMAXCONN) == -1) {
		perror("Can't listen");
		exit (1);
	}

	_printServerInfo();

	_startServerLoop();
}
