#include "WebServer.hpp"

WebServer::WebServer() {

}

WebServer::WebServer(const std::string &file) {
	parseConfigFile(file);
}

WebServer::WebServer(const WebServer &other) {
	std::vector<Server>::const_iterator vector_it;
	for (vector_it = other._servers.begin(); vector_it != other._servers.end(); vector_it++) {
		_servers.push_back(*vector_it);
	}

	std::vector<pollfd>::const_iterator it;
	for (it = other._poll_fds.begin(); it != other._poll_fds.end(); it++) {
		_poll_fds.push_back(*it);
	}

	std::map<int, std::vector<Server> >::const_iterator map_it;
	std::map<int, std::vector<Server> >::const_iterator end = other._portsMap.end();
	for (map_it = other._portsMap.begin(); map_it != end; map_it++) {
		_portsMap[map_it->first] = map_it->second;
	}
}

WebServer &WebServer::operator=(const WebServer &other) {
	if (this != &other) {
		_servers.clear();
		std::vector<Server>::const_iterator vector_it;
		for (vector_it = other._servers.begin(); vector_it != other._servers.end(); vector_it++) {
			_servers.push_back(*vector_it);
		}

		_poll_fds.clear();
		std::vector<pollfd>::const_iterator it;
		for (it = other._poll_fds.begin(); it != other._poll_fds.end(); it++) {
			_poll_fds.push_back(*it);
		}

		_portsMap.clear();
		std::map<int, std::vector<Server> >::const_iterator map_it;
		std::map<int, std::vector<Server> >::const_iterator end = other._portsMap.end();
		for (map_it = other._portsMap.begin(); map_it != end; map_it++) {
			_portsMap[map_it->first] = map_it->second;
		}
	}
	return *this;
}

WebServer::~WebServer() {
/* 	std::vector<Server>::const_iterator it;
	for (it = _servers.begin(); it != _servers.end(); it++) {
		delete *it;
	} */
}

