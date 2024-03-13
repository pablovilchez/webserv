#include "Server.hpp"

Server::Server() {
	_config = new Config();
}

Server::Server(const Server &other) {
	_config = new Config(*other._config);
}

Server &Server::operator=(const Server &other) {
	if (this != &other) {
		delete _config;
		_config = new Config(*other._config);
	}
	return *this;
}

Server::Server(const std::string &config, int servNum) {
	_config = new Config(config ,servNum);
}

Server::~Server() {
	delete _config;
}

const Config &Server::getConfig() const {
	return *_config;
}

/* int getServerSocket() {
	int listening = socket(PF_INET, SOCK_STREAM, 0);
	if (listening == -1) {
		perror("Can't create a socket");
		_exit(1);
	}
	return listening;
}
 */
/* void Server::_fillServerAddr() {
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(PORT);
	_server_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(_server_addr.sin_zero), '\0', 8);
} */

/* void printServerInfo() {
	char hostName[1024];
    gethostname(hostName, 1024);
    std::cout << "Server started on " << hostName << std::endl;

	hostent* host = gethostbyname(hostName);
	if (host == NULL) {
		herror("gethostbyname");
		_exit(1);
	}

	std::cout << "IP Address: " << inet_ntoa(*(in_addr*)*host->h_addr_list) << std::endl;
}
 */
/* void createNewpollfd(int some_socket, std::vector<pollfd> &_poll_fds) {
	pollfd listen_pollfd;
	listen_pollfd.fd = some_socket;
	listen_pollfd.events = POLLIN;
	_poll_fds.push_back(listen_pollfd);
} */

/* void Server::_startServerLoop() {
	createNewpollfd(_listen_sock, _poll_fds);

	while(1) {
		int ret = poll(reinterpret_cast<pollfd *>(&_poll_fds[0]), static_cast<unsigned int>(_poll_fds.size()), -1);
		if (ret == -1) {
			perror("poll failed");
			_exit(1);
		}

        std::vector<pollfd>::iterator it;
        std::vector<pollfd>::iterator end = _poll_fds.end();

		for (it = _poll_fds.begin(); it != end; it++) {
			if (it->revents & POLLIN) {
				if (it->fd == _listen_sock) {
					sockaddr_in client;
					socklen_t addr_size = sizeof(sockaddr_in);

					int client_sock = accept(_listen_sock, reinterpret_cast<sockaddr*>(&client), &addr_size);
					if (client_sock == -1) {
						perror("Can't accept client");
						continue;
					}
					if (_poll_fds.size()-1 < MAX_CLIENTS)
                    {
						createNewpollfd(client_sock, _poll_fds);
						std::cout << "New client connected: " << client_sock << std::endl;
					}
					else
					{
						std::cout << "Server is full" << std::endl;
						send(client_sock, "too many clients!!!", 20, 0);
						close(client_sock);
						continue;
                    }
				}
				else {
					char buffer[1024];
					int bytes = recv(it->fd, buffer, 1024, 0);
					if (bytes <= 0) {
						if (bytes == 0) {
							std::cout << "Client disconnected: " << it->fd << std::endl;
						}
						else {
							perror("Unable to read from socket");
						}
						std::cout << "Closing socket: " << it->fd << std::endl;
						close(it->fd);
						_poll_fds.erase(it);
						std::cout << "Fds array size: " << _poll_fds.size() << std::endl;
					}
					else {
						std::istringstream request(buffer);
						std::string request_line;
						getline(request, request_line);

						std::istringstream iss(request_line);
						std::string method;
						std::string page;
						iss >> method >> page;

						std::cout << "Received:___________\n " << request_line << std::endl;
						if (method == "GET") {
							if (page == "/") {
								std::string page_html = "<!DOCTYPE html>"
												"<html lang=\"en\">"
												"<head>"
												"    <meta charset=\"UTF-8\">"
												"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
												"    <title>Error Page</title>"
												"</head>"
												"<body>"
												"    <h1>Hello</h1>"
												"    <h2>World</h2>"
												"    <a href=\"/\">Back</a>"
												"</body>"
												"</html>";
								std::string response = "HTTP/1.1 200 OK\r\n";
								std::stringstream ss;
								ss << page_html.size();
								response += "Content-Type: text/html\r\n";
								response += "Content-Length: " + ss.str() + "\r\n";
								response += "Connection: close\r\n";
								response += "\r\n"; // End of headers
								response += page_html; // Message body

								std::cout << "Response: PAGE \n" << std::endl;
								send(it->fd, response.c_str(), response.size(), 0);
							}
							else {
								std::string response = "HTTP/1.1 200 OK\r\n";
								response += "Content-Type: text/plain\r\n";
								response += "Content-Length: 0\r\n";
								response += "Connection: close\r\n";
								response += "\r\n"; // End of headers

								std::cout << "Response: OTHER \n" << std::endl;
								send(it->fd, response.c_str(), response.size(), 0);
							}
						}
						memset(buffer, 0, 1024);
					}
				}
			}
			else if (it->revents & POLLERR) {
				 if (it->fd == _listen_sock)
                {
                    perror("listen socket error");
                    _exit(1);
                }
                else
                {
					perror("client socket error");
					close(it->fd);
					_poll_fds.erase(it);
                }
			}
		}
	}
}

void Server::startServer() {
	_poll_fds.reserve(MAX_CLIENTS);
	int yes = 1;
	if (setsockopt(_listen_sock, SOL_SOCKET, SO_REUSEADDR,  &yes, sizeof(int)) == -1) {
		perror("setsockopt failed");
    	_exit(-1);
	}

	if (setsockopt(_listen_sock, SOL_SOCKET, SO_BROADCAST,  &yes, sizeof(int)) == -1) {
		perror("setsockopt failed");
    	_exit(-1);
	}

	if(bind(_listen_sock, reinterpret_cast<sockaddr*>(&_server_addr), sizeof(_server_addr)) == -1) {
		perror("Can't bind to IP/port");
		_exit(-1);
	}

	if (listen(_listen_sock, SOMAXCONN) == -1) {
		perror("Can't listen");
		_exit (-1);
	}

	printServerInfo();

	_startServerLoop();
} */


