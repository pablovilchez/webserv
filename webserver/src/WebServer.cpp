#include "WebServer.hpp"

/*______________________________UTILS-FUNCTIONS-START______________________________*/

bool web_isComment(const std::string &line) {
	for (size_t i = 0; i < line.length(); i++) {
		if (!std::isspace(line[i])) {
			return line[i] == '#';
		}
	}
	return true;
}

void insertNewPollfd(std::vector<pollfd> &_poll_fds, int socket) {
	pollfd listen_pollfd;
	listen_pollfd.fd = socket;
	listen_pollfd.events = POLLIN;
	_poll_fds.push_back(listen_pollfd);
}

int createNewListener(int port , std::vector<Server> servers) {
	int listening = socket(PF_INET, SOCK_STREAM, 0);
	if (listening == -1) {
		perror("Can't create a socket");
		_exit(1);
	}

	int yes = 1;
	if (setsockopt(listening, SOL_SOCKET, SO_REUSEADDR,  &yes, sizeof(int)) == -1) {
		perror("setsockopt failed");
		_exit(-1);
	}

	if (setsockopt(listening, SOL_SOCKET, SO_BROADCAST,  &yes, sizeof(int)) == -1) {
		perror("setsockopt failed");
		_exit(-1);
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(server_addr.sin_zero), '\0', 8);

	if(bind(listening, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == -1) {
		perror("Can't bind to IP/port");
		_exit(-1);
	}

	if (listen(listening, SOMAXCONN) == -1) {
		perror("Can't listen");
		_exit (-1);
	}

	std::vector<Server> ::const_iterator it_serv;
	std::cout << BLUE_TEXT << "Listening on port: " << port <<" [Servers: ";
	for (it_serv = servers.begin(); it_serv != servers.end(); it_serv++) {
		std::cout << (*it_serv).getServerName();
		if (it_serv + 1 != servers.end()) {
			std::cout << ", ";
		}
	}
	std::cout << "]" << RESET_COLOR << std::endl;

	return listening;
}

void initListeners(std::map<int, std::vector<Server> >& _portsMap, std::vector<pollfd>& _poll_fds, std::vector<int>& _listeners) {
	std::map<int, std::vector<Server> >::const_iterator it;
	std::map<int, std::vector<Server> >::const_iterator end = _portsMap.end();
	for (it = _portsMap.begin(); it != end; it++) {
		int listening = createNewListener(it->first, it->second);
		_listeners.push_back(listening);
		insertNewPollfd(_poll_fds, listening);
	}
}

std::string extractServerName(char *buffer) {
	std::istringstream bufferStream(buffer);
	std::string line;
	std::string srv;
	while (std::getline(bufferStream, line)) {
		if (line.find("Host:") != std::string::npos) {
			std::istringstream iss(line);
			std::string host;
			iss >> host >> srv;
			size_t pos = srv.find(".");
			if (pos != std::string::npos) {
				srv = srv.substr(0, pos);
				return srv;
			}
		}
	}
	return "";
}

std::string extractServerPort(char *buffer) {
	std::istringstream bufferStream(buffer);
	std::string line;
	std::string port;
	while (std::getline(bufferStream, line)) {
		if (line.find("Host:") != std::string::npos) {
			std::istringstream iss(line);
			std::string host;
			iss >> host >> port;
			size_t pos = port.find(":");
			if (pos != std::string::npos) {
                port = port.substr(pos + 1);
				return port;
			}
		}
	}
	return "";
}

const Server &WebServer::getServerConfig(char *buffer) {
    std::string srv = ::extractServerName(buffer);
    std::string port = ::extractServerPort(buffer);
	std::map<int, std::vector<Server> >::const_iterator itMap;
	std::map<int, std::vector<Server> >::const_iterator endMap = _portsMap.end();
	for (itMap = _portsMap.begin(); itMap != endMap; itMap++) {
		if (itMap->first == std::atoi(port.c_str())) {
			std::vector<Server>::const_iterator itServ;
			std::vector<Server>::const_iterator end = itMap->second.end();
			for (itServ = itMap->second.begin(); itServ != end; itServ++) {
				if (itServ->getServerName() == srv) {
					return *itServ;
				}
			}
			return *itMap->second.begin();
		}
	}
	return *(_portsMap.begin()->second.begin());
}

bool WebServer::correctConfig() const {
	return _correctConfig;
}

bool WebServer::continueServer(char *buffer) {
	std::istringstream bufferStream(buffer);
	std::string line;

	std::getline(bufferStream, line);
	if (line.find("shutdown") != std::string::npos)
		_running = false;
	return _running;
}

/*______________________________UTILS-FUNCTIONS-END______________________________*/


/*______________________________CLASS-METHODS-START______________________________*/

bool WebServer::parseConfigFile(const std::string &file) {
	std::ifstream fileStream(file.c_str());
	std::string line;
	std::string buffer;
	int servers = 0;
	int checkEnd = 0;

	if (!fileStream.is_open()) {
		std::cerr << "Error: " << strerror(errno) << std::endl;
		return false;
	}
	while (std::getline(fileStream, line)) {
		if (web_isComment(line)) {
			continue;
		}
		if (line.find("server") != std::string::npos && line.find("{") != std::string::npos) {
			buffer = line;
			buffer += "\n";
			checkEnd = 1;
			while (std::getline(fileStream, line) && checkEnd > 0) {
				buffer += line;
				buffer += "\n";
				if (line.find("{") != std::string::npos) {
					checkEnd++;
				}
				if (line.find("}") != std::string::npos) {
					checkEnd--;
				}
			}
			Server servAux(buffer);
			_servers.push_back(servAux);

			std::set<int>ports = servAux.getPort();
			std::set<int>::iterator it;
			for (it = ports.begin(); it != ports.end(); it++) {
				_portsMap[*it].push_back(servAux);
			}
            if (!servAux.checkConfig()) {
                std::cerr << "Error: invalid server config" << std::endl;
				fileStream.close();
                return false;
            }
			servers++;
		}
	}
	fileStream.close();
    return true;
}

void WebServer::createNewClient(int it_listen) {
	sockaddr_in client;
	socklen_t addr_size = sizeof(sockaddr_in);

	int client_sock = accept(it_listen, reinterpret_cast<sockaddr*>(&client), &addr_size);

	if (client_sock == -1) {
		perror("Can't accept client");
		return ;
	}
	if ((_poll_fds.size() - _listeners.size()) < MAXCLIENTS) {
		insertNewPollfd(_poll_fds, client_sock);
		_clients.push_back(client_sock);
		std::cout << GREEN_TEXT << "New client connected: " << client_sock << std::endl;
	}
	else {
		std::string page = Request::defaultErrorPage("503", "Service Unavailable");

		std::stringstream ss;
		int size = page.size();
		ss << size;

		_response.clear();
		_response += "HTTP/1.1 503 Service Unavailable\r\n";
		_response += "Content-Type: text/html\r\n";
		_response += "Content-Length:" + ss.str() + "\r\n";
		_response += "Connection: close\r\n\r\n";
		_response += page;
		std::cout << "Server is full" << std::endl;
		std::cout << "RESPONESE: " << _response << std::endl;
		send(client_sock, _response.c_str(), _response.size(), 0);
		close(client_sock);
	}
}

void WebServer::checkServes() {
	size_t it = 0;
	while (it != _poll_fds.size()) {
		if (_poll_fds[it].revents & POLLIN) {
			size_t it_listen = 0;
			while (it_listen != _listeners.size()) {
				if (_poll_fds[it].fd == _listeners[it_listen]) {
					createNewClient(_listeners[it_listen]);
				}
				 it_listen++;
			}
		}
		it++;
	}

}

void WebServer::checkClients() {
	char buffer[1024];
	memset(buffer, 0, 1024);
	size_t it = 0;
	while (it != _poll_fds.size()) {
		size_t it_client = 0;
		while (it_client < _clients.size()) {
			if (_poll_fds[it].fd == _clients[it_client]) {
				if (_poll_fds[it].revents & POLLIN) {
					int bytes = recv(_poll_fds[it].fd, buffer, 1024, 0);
					if (bytes <= 0) {
						if (bytes == 0) {
							std::cout << RED_TEXT << "Client disconnected: " << _poll_fds[it].fd << RESET_COLOR << std::endl;
						}
						else {
							perror("Unable to read from socket");
						}
						std::cout << "Closing socket: " << _poll_fds[it].fd << std::endl;
						close(_poll_fds[it].fd);
						_poll_fds[it].fd = -1;
						_clients.erase(_clients.begin() + it_client);
					}
					else {
						std::istringstream request(buffer);
						std::string request_line;
						getline(request, request_line);

						std::cout << YELLOW_TEXT << "Request:  " << request_line << RESET_COLOR << std::endl;
						if (continueServer(buffer)) {
							Server server = getServerConfig(buffer);

							Request newRequest(buffer, server);
							_response = newRequest.getResponse();
							_poll_fds[it].events = POLLOUT;
						}
					}
				}
				else if (_poll_fds[it].revents & POLLOUT) {
					send(_poll_fds[it].fd, _response.c_str(), _response.size(), 0);
					close(_poll_fds[it].fd);
					_poll_fds[it].fd = -1;
					_clients.erase(_clients.begin() + it_client);
				}
				else if (_poll_fds[it].revents & POLLHUP) {
					std::cout << RED_TEXT << "Client disconnected: " << _poll_fds[it].fd << RESET_COLOR << std::endl;
					close(_poll_fds[it].fd);
					_poll_fds[it].fd = -1;
					_clients.erase(_clients.begin() + it_client);
				}
				else if (_poll_fds[it].revents & POLLERR) {
					perror("Poll error");
					_exit(1);
				}
			}
			it_client++;
		}
		it++;
	}
}

void WebServer::initService() {
	::initListeners(_portsMap, _poll_fds, _listeners);

	while (_running) {
		int ret = poll(reinterpret_cast<pollfd *>(&_poll_fds[0]), static_cast<unsigned int>(_poll_fds.size()), -1);
		if (ret == -1) {
			perror("poll failed");
			_exit(1);
		}

		checkServes();
		checkClients();
		size_t it_delete = 0;
		while (it_delete < _poll_fds.size()) {
			if (_poll_fds[it_delete].fd == -1) {
				std::cout << RED_TEXT << "Client disconnected: " << _poll_fds[it_delete].fd << RESET_COLOR << std::endl;
				_poll_fds.erase(_poll_fds.begin() + it_delete);
			}
			it_delete++;
		}
	}
	std::cout << "Server shutting down" << std::endl;
}

/*______________________________CLASS-METHODS-END______________________________*/
