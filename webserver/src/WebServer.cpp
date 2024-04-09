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
		_exit(1);
	}

	if (setsockopt(listening, SOL_SOCKET, SO_BROADCAST,  &yes, sizeof(int)) == -1) {
		perror("setsockopt failed");
		_exit(1);
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(server_addr.sin_zero), '\0', 8);

	if(bind(listening, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == -1) {
		perror("Can't bind to IP/port");
		_exit(1);
	}

	if (listen(listening, SOMAXCONN) == -1) {
		perror("Can't listen");
		_exit (1);
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
	if (line.find("shutdown") != std::string::npos) {
		_running = false;
		std::cout << GREEN_TEXT << "Server is shutting down" << RESET_COLOR << std::endl;
	}
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
		perror("Can't Open file");
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
				perror("Can't config server");
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
	if ((_pollSize - _listSize) < MAXCLIENTS) {
		insertNewPollfd(_poll_fds, client_sock);
		_pollSize = _poll_fds.size();
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
	while (it < _listSize) {
		if(_poll_fds[it].revents & POLLIN) {
			createNewClient(_poll_fds[it].fd);
		}
		it++;
	}
	_pollSize = _poll_fds.size();
}

void WebServer::checkClients() {
	char buffer[1024];
	memset(buffer, 0, 1024);
	std::vector<pollfd>::iterator fd_it;
	std::vector<pollfd>::iterator end = _poll_fds.end();
	size_t it = _listSize;
	while (it < _pollSize) {
		if (_poll_fds[it].revents & POLLIN && _poll_fds[it].fd != -1) {
			int bytes = recv(_poll_fds[it].fd, buffer, 1024, 0);
				if (bytes == -1) {
					std::cout << RED_TEXT << "ERROR (recv): Client disconnected: " << _poll_fds[it].fd << RESET_COLOR << std::endl;
					close(_poll_fds[it].fd);
					_poll_fds[it].fd = -1;
				}
				else if (bytes == 0) {
					std::cout << RED_TEXT << "OK: Client disconnected: " << _poll_fds[it].fd << RESET_COLOR << std::endl;
					close(_poll_fds[it].fd);
					_poll_fds[it].fd = -1;
				}
				else if (bytes > 0) {
					std::istringstream request(buffer);
					std::string request_line;
					getline(request, request_line);

					//std::cout << YELLOW_TEXT << "REQUEST: \n" << buffer << RESET_COLOR << std::endl;
					std::cout << "request_line" << request_line << std::endl;

					std::map<int, Request>::iterator it_req = _clientRequests.find(_poll_fds[it].fd);
					if (it_req != _clientRequests.end()) {
						it_req->second.parseBody(buffer, bytes);
					} else {
						Server server = getServerConfig(buffer);
						Request newRequest(buffer, server);
						memset(buffer, 0, 1024);
						_clientRequests.insert(std::make_pair(_poll_fds[it].fd, newRequest));
					}
					for (std::map<int, Request>::iterator it_req = _clientRequests.begin(); it_req != _clientRequests.end(); ++it_req) {
						if (it_req->second.isResponseReady()) {
							std::cout << "is ready" << std::endl;
							_response = it_req->second.getResponse();
							for (fd_it = _poll_fds.begin(); fd_it != end; fd_it++) {
								if ((_poll_fds[it].fd) == it_req->first) {
									_poll_fds[it].events = POLLOUT;
									break;
								}
							}
						}
					}
				}
		}
		else if (_poll_fds[it].revents & POLLOUT && _poll_fds[it].fd != -1) {
			std::cout << GREEN_TEXT << "RESPONSE: \n" << _response << RESET_COLOR << std::endl;
			send(_poll_fds[it].fd, _response.c_str(), _response.size(), 0);
			_poll_fds[it].events = POLLIN;
			_clientRequests.erase(fd_it->fd);
			_response.clear();
		}
		else if ((_poll_fds[it].revents & POLLERR || _poll_fds[it].revents & POLLHUP) && _poll_fds[it].fd != -1) {
			if (_poll_fds[it].revents & POLLERR) {
				std::cout << RED_TEXT << "ERROR (POLLERR): Client disconnected: " << _poll_fds[it].fd << RESET_COLOR << std::endl;
			}
			else {
				std::cout << RED_TEXT << "OK (POLLHUP): Client disconnected: " << _poll_fds[it].fd << RESET_COLOR << std::endl;
			}
			close(_poll_fds[it].fd);
			_poll_fds[it].fd = -1;
		}
		it++;
	}
}


void WebServer::initService() {
	::initListeners(_portsMap, _poll_fds, _listeners);
	_listSize = _listeners.size();

	while (_running) {
		_pollSize = _poll_fds.size();

		int ret = poll(reinterpret_cast<pollfd *>(&_poll_fds[0]), static_cast<unsigned int>(_poll_fds.size()), -1);
		if (ret == -1) {
			std::cout << RED_TEXT << "ERROR (poll): Server is shutting down"<< RESET_COLOR << std::endl;
			_running = false;
		}

		checkServes();
		checkClients();

		size_t it_delete = _listSize;
		while (it_delete < _pollSize) {
			if (_poll_fds[it_delete].fd == -1) {
				_poll_fds.erase(_poll_fds.begin() + it_delete);
			}
			it_delete++;
		}
	}
}

/*______________________________CLASS-METHODS-END______________________________*/
