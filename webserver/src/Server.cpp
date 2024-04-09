#include "Server.hpp"

class Location;

Server::Server() : _maxSize(0) {
	_port.insert(8080);
	_serverName = "nuevo_0";
	_errorPages.insert(std::make_pair(400, "var/www/error/400.html"));
	_maxSize = 1024 * 10;
	Location newLocation;
	_locations.push_back(newLocation);
}

Server::Server(const std::string &serverConfig) {
	parseServer(serverConfig);
	if (DEBUG) {
		printData();
	}
}

Server::Server(const Server &other) {
	_port = other._port;
	_serverName = other._serverName;
	_errorPages = other._errorPages;
	_maxSize = other._maxSize;
	_locations = other._locations;
}

Server& Server::operator=(const Server &other) {
	if (this != &other) {
		_port = other._port;
		_serverName = other._serverName;
		_errorPages = other._errorPages;
		_maxSize = other._maxSize;
		_locations = other._locations;
	}
	return *this;
}

Server::~Server() {

}

bool conf_isComment(const std::string &line) {
	for (size_t i = 0; i < line.length(); i++) {
		if (!std::isspace(line[i])) {
			return line[i] == '#';
		}
	}
	return true;
}

void Server::parseServer(const std::string &serverConfig) {
	std::istringstream stream(serverConfig);
	std::string line;
	std::string key;
	std::string value;

	std::getline(stream, line);
	while (std::getline(stream, line)) {
		if (conf_isComment(line)) {
			continue;
		}
		std::istringstream lineStream(line);
		lineStream >> key;
		if (key == "server_name") {
			if (lineStream >> value)
				_serverName = value;
			else
				_serverName = "";
		}
		else if (key == "listen") {
			while (lineStream >> value) {
				int intVal = strtol(value.c_str(), NULL, 10);
				std::set<int>::iterator it = _port.find(intVal);
				if(it == _port.end()) {
					_port.insert(intVal);
				}
			}
		}
		else if (key == "client_max_body_size") {
			lineStream >> value;
			_maxSize = strtol(value.c_str(), NULL, 10);
			if(value.find("M") != std::string::npos) {
				_maxSize *= 1024 * 1024;
			}
			else if(value.find("K") != std::string::npos) {
				_maxSize *= 1024;
			}
		}
		else if (key == "error_page") {
			int code;
			lineStream >> code;
			lineStream >> value;
			_errorPages.insert(std::make_pair(code, value));
		}
		else if (key == "location") {
			std::string locatConfig;
			locatConfig += line;
			locatConfig += "\n";
			while(std::getline(stream, line) && line.find("}") == std::string::npos) {
				locatConfig += line;
				locatConfig += "\n";
			}
			Location newLocation(locatConfig);
			_locations.push_back(newLocation);
		}
		else if (key == "}") {
			break;
		}
		else {
			std::cerr << "Error: Unknown key: " << key << std::endl;
		}
	}
}

bool Server::checkConfig() {
    if(_port.empty() || _serverName.empty() || _serverName == "" || _locations.empty()) {
        return (false);
	}
    if(_maxSize == 0) {
		_maxSize = 1024 * 10;
	}
    return true;
}

void Server::printData() {
	std::cout << "Server name:  " << _serverName << std::endl;
	std::cout << "Ports:  ";

	std::set<int>::iterator it_set;
	for(it_set = _port.begin(); it_set != _port.end(); it_set++) {
		std::cout << *it_set << "  ";
	}

	std::cout << std::endl;
	std::cout << "Error pages:  ";

	std::map<int, std::string>::iterator it_map;
	for(it_map = _errorPages.begin(); it_map != _errorPages.end(); it_map++) {
		std::cout << it_map->first << "  ";
	}

	std::cout << std::endl;
	std::cout << "Max Size:  " << _maxSize << std::endl;

	std::vector<Location>::const_iterator it_vector;
	for(it_vector = _locations.begin(); it_vector != _locations.end(); it_vector++) {
		it_vector->printData();
	}
}

void Server::setLocation(Location location) {
	_locations.push_back(location);
}

std::set<int> Server::getPort() const {
	return _port;
}

std::string Server::getServerName() const {
	return _serverName;
}

std::string Server::getErrorPage(const int &errorCode) const {
	std::map<int, std::string>::const_iterator it;
	it = _errorPages.find(errorCode);
	if (it != _errorPages.end()) {
		return it->second;
	}
	return "";
}

int Server::getMaxSize() const {
	return _maxSize;
}


const Location& Server::getLocation(const std::string &location) const {
	std::vector<Location>::const_iterator it_root;
	std::vector<Location>::const_iterator it_vector;

	for(it_vector = _locations.begin(); it_vector != _locations.end(); it_vector++) {
		if((*it_vector).getLocation() == location) {
			return *it_vector;
		}
		if((*it_vector).getRootLocation()) {
			it_root = it_vector;
		}
	}
	if (it_vector->getRootLocation())
		return *it_root;
	else {
		static const Location invalidLocation("null");
    	return invalidLocation;
	}
}

