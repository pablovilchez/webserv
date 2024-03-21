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

void WebServer::initService() {
	::initListeners(_portsMap, _poll_fds, _listeners);
	_running = true;

	std::string response;
	while (_running) {
		int ret = poll(reinterpret_cast<pollfd *>(&_poll_fds[0]), static_cast<unsigned int>(_poll_fds.size()), -1);
		if (ret == -1) {
			perror("poll failed");
			_exit(1);
		}
		std::vector<pollfd>::iterator it;
        std::vector<pollfd>::iterator end = _poll_fds.end();

		char buffer[1024];
		for (it = _poll_fds.begin(); it != end; it++) {
			if (it->revents & POLLIN) {
				std::vector<int>::const_iterator it_listen;
				for (it_listen = _listeners.begin(); it_listen != _listeners.end(); it_listen++) {
					if (it->fd == *it_listen) {
						sockaddr_in client;
						socklen_t addr_size = sizeof(sockaddr_in);

						int client_sock = accept(*it_listen, reinterpret_cast<sockaddr*>(&client), &addr_size);
						if (client_sock == -1) {
							perror("Can't accept client");
							break;
						}
						if (_poll_fds.size()-1 < MAXCLIENTS) {
							insertNewPollfd(_poll_fds, client_sock);
							std::cout << GREEN_TEXT << "New client connected: " << client_sock << std::endl;
						}
						else {
							std::cout << "Server is full" << std::endl;
							send(client_sock, "too many clients!!!", 20, 0);
							close(client_sock);
						}
						break;
					}
				}
				if (it_listen == _listeners.end()) {
					int bytes = recv(it->fd, buffer, 1024, 0);
					if (bytes <= 0) {
						if (bytes == 0) {
							std::cout << RED_TEXT << "Client disconnected: " << it->fd << RESET_COLOR << std::endl;
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

						std::cout << YELLOW_TEXT << "Request:  " << request_line << RESET_COLOR << std::endl;
						if (continueServer(buffer)) {

							Server server = getServerConfig(buffer);

							Request newRequest(buffer, server);
							response = newRequest.getResponse();
							it->events = POLLOUT;
						}
					}
				}
			}
			else if (it->revents & POLLOUT) {
				send(it->fd, response.c_str(), response.size(), 0);
				std::cout << RED_TEXT << "Client disconnected: " << it->fd << RESET_COLOR << std::endl;
				close(it->fd);
				_poll_fds.erase(it);
				memset(buffer, 0, 1024);
				response.clear();
			}
			else if (it->revents & POLLERR) {
				std::vector<int>::const_iterator it_listen;
				for (it_listen = _listeners.begin(); it_listen != _listeners.end(); it_listen++) {
					if (it->fd == *it_listen) {
						perror("listen socket error");
						_exit(1);
					}
				}
				if (it_listen == _listeners.end()) {
					perror("client socket error");
					close(it->fd);
					_poll_fds.erase(it);
				}
			}
		}
	}
	std::cout << "Server shutting down" << std::endl;
}

/*______________________________CLASS-METHODS-END______________________________*/
