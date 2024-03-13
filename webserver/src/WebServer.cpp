#include "WebServer.hpp"

/*______________________________UTILS-FUNCTIONS-START______________________________*/

/*
// It uses forbidden functions -> should be redone
void printServersInfo(int port, std::vector<const Server*> servers) {
	for (std::vector<const Server*>::const_iterator it_serv = servers.begin(); it_serv != servers.end(); it_serv++) {
		char hostName[1024];
		gethostname(hostName, 1024);
		std::cout << "Server: " << (*it_serv)->getConfig().getServerName() << ":" << port << " started on " << hostName << std::endl;

		hostent* host = gethostbyname(hostName);
		if (host == NULL) {
		herror("gethostbyname");
		_exit(1);
		}

		std::cout << "IP Address: " << inet_ntoa(*(in_addr*)*host->h_addr_list) << std::endl;
	}
}
*/

bool web_isComment(const std::string &line) {
	for (size_t i = 0; i < line.length(); i++)
	{
		if (!std::isspace(line[i]))
			return line[i] == '#';
	}
	return true;
}

void insertNewPollfd(std::vector<pollfd> &_poll_fds, int socket) {
	pollfd listen_pollfd;
	listen_pollfd.fd = socket;
	listen_pollfd.events = POLLIN;
	_poll_fds.push_back(listen_pollfd);
}

int createNewListener(int port) {
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

	struct sockaddr_in _server_addr;
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(port);
	_server_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(_server_addr.sin_zero), '\0', 8);

	if(bind(listening, reinterpret_cast<sockaddr*>(&_server_addr), sizeof(_server_addr)) == -1) {
		perror("Can't bind to IP/port");
		_exit(-1);
	}

	if (listen(listening, SOMAXCONN) == -1) {
		perror("Can't listen");
		_exit (-1);
	}

	return listening;
}

void initListeners(std::map<int, std::vector<const Server*> >& _portsMap, std::vector<pollfd>& _poll_fds, std::vector<int>& _listeners) {
	std::map<int, std::vector<const Server*> >::const_iterator it;
	std::map<int, std::vector<const Server*> >::const_iterator end = _portsMap.end();
	for (it = _portsMap.begin(); it != end; it++) {
		int listening = createNewListener(it->first);

		// Print server info
		//printServersInfo(it->first, it->second);

		_listeners.push_back(listening);
		insertNewPollfd(_poll_fds, listening);
	}
}

/*______________________________UTILS-FUNCTIONS-END______________________________*/



/*______________________________CLASS-METHODS-START______________________________*/

void WebServer::parseConfigFile(const std::string &file) {
	std::ifstream fileStream(file.c_str());
	std::string line;
	std::string buffer;
	int servers = 0;
	int checkEnd = 0;
	int servNum = 0;

	if (!fileStream.is_open())
	{
		std::cerr << "Error: " << strerror(errno) << std::endl;
		return;
	}
	while (std::getline(fileStream, line))
	{
		if (web_isComment(line))
			continue;
		if (line.find("server") != std::string::npos && line.find("{") != std::string::npos)
		{
			buffer = line;
			buffer += "\n";
			checkEnd = 1;
			while (std::getline(fileStream, line) && checkEnd > 0)
			{
				buffer += line;
				buffer += "\n";
				if (line.find("{") != std::string::npos)
					checkEnd++;
				if (line.find("}") != std::string::npos)
					checkEnd--;
			}
			const Server *servAux = new Server(buffer, servNum++);
			_servers.push_back(servAux);

			std::set<int>ports = servAux->getConfig().getPort();
			std::set<int>::iterator it;
			for (it = ports.begin(); it != ports.end(); it++) {
				_portsMap[*it].push_back(servAux);
			}
			servers++;
		}
	}
	fileStream.close();
}

void WebServer::initService() {
	::initListeners(_portsMap, _poll_fds, _listeners);

	std::string response;
	while (1) {
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
							std::cout << "New client connected: " << client_sock << std::endl;
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
								response = "HTTP/1.1 200 OK\r\n";
								std::stringstream ss;
								ss << page_html.size();
								response += "Content-Type: text/html\r\n";
								response += "Content-Length: " + ss.str() + "\r\n";
								response += "\r\n"; // End of headers
								response += page_html; // Message body
							}
							else {
								response = "HTTP/1.1 200 OK\r\n";
								response += "Content-Type: text/plain\r\n";
								response += "Content-Length: 0\r\n";
								response += "\r\n"; // End of headers
							}
							it->events = POLLOUT;
						}
					}
				}
			}
			else if (it->revents & POLLOUT) {
				std::cout << "Response: PAGE \n" << std::endl;
				send(it->fd, response.c_str(), response.size(), 0);
				std::cout << "Client disconnected: " << it->fd << std::endl;
				std::cout << "Closing socket: " << it->fd << std::endl;
				close(it->fd);
				_poll_fds.erase(it);
				std::cout << "Fds array size: " << _poll_fds.size() << std::endl;
				memset(buffer, 0, 1024);
				response.clear();
			}
/* 			else if (it->revents & POLLERR) {
				std::vector<int>::const_iterator it_listen;
				for (it_listen = _listeners.begin(); it_listen != _listeners.end(); it_listen++) {
					if (it->fd == *it_listen) {
						perror("listen socket error");
						_exit(1);
					}
                	else {
						perror("client socket error");
						close(it->fd);
						_poll_fds.erase(it);
                	}
				}
			} */
		}
	}
}

/*______________________________CLASS-METHODS-END______________________________*/
