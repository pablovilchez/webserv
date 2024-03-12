#include "Config.hpp"

Config::Config() : _maxSize(0) {
	defaultConfig(0);
	Location *newLocation = new Location();
	_locations.push_back(newLocation);
}

Config::Config(const std::string &config, int servNum) {
	(void) config;
	(void) servNum;
	/* parseConfig(config, servNum);
	if (DEBUG) printData(); */
}


Config::Config(const Config &other) {
	(void) other;
/* 	std::set<int>::iterator it_set;
	for (it_set = other._port.begin(); it_set != other._port.end(); it_set++) {
		_port.insert(*it_set);
	}

	_serverName = other._serverName;

	std::map<int, std::string>::const_iterator it_map;
	for (it_map = other._errorPages.begin(); it_map != other._errorPages.end(); it_map++) {
		_errorPages.insert(std::make_pair(it_map->first, it_map->second));
	}

	_maxSize = other._maxSize;

	std::vector<Location>::const_iterator it_vec;
	for (it_vec = other._locations.begin(); it_vec != other._locations.end(); it_vec++) {
		_locations.push_back(*it_vec);
	} */
}

Config& Config::operator=(const Config &other) {
	(void) other;
/* 	if (this != &other) {
		_port.clear();
		std::set<int>::iterator it_set;
		for (it_set = other._port.begin(); it_set != other._port.end(); it_set++) {
			_port.insert(*it_set);
		}

		_serverName = other._serverName;

		_errorPages.clear();
		std::map<int, std::string>::const_iterator it_map;
		for (it_map = other._errorPages.begin(); it_map != other._errorPages.end(); it_map++) {
			_errorPages.insert(std::make_pair(it_map->first, it_map->second));
		}

		_maxSize = other._maxSize;

		_locations.clear();
		std::vector<Location>::const_iterator it_vec;
		for (it_vec = other._locations.begin(); it_vec != other._locations.end(); it_vec++) {
			_locations.push_back(*it_vec);
		}
	} */
	return *this;
}

Config::~Config() {
	std::vector<Location*>::iterator it;
	std::vector<Location*>::iterator end = _locations.end();
	for (it = _locations.begin(); it != end; it++) {
		delete *it;
	}
}

void Config::defaultConfig(int servNum) {
	if(_port.empty())
		_port.insert(8080);
	if(_serverName.empty())
		_serverName = "nuevo_" + std::to_string(servNum);
	if(_errorPages.empty())
		_errorPages.insert(std::make_pair(400, "var/www/error/400.html"));
	if(_maxSize == 0)
		_maxSize = 1024 * 10;
}

/* bool conf_isComment(const std::string &line) {
	for (size_t i = 0; i < line.length(); i++)
	{
		if (!std::isspace(line[i]))
			return line[i] == '#';
	}
	return true;
} */

/* void Config::parseConfig(const std::string &config, int servNum) {
	std::istringstream stream(config);
	std::string line;
	std::string key;
	std::string value;

	std::getline(stream, line);
	while (std::getline(stream, line))
	{
		if (conf_isComment(line))
			continue;
		std::istringstream lineStream(line);
		lineStream >> key;
		if (key == "server_name")
		{
			lineStream >> value;
			_serverName = value;
		}
		else if (key == "listen")
		{
			while (lineStream >> value)
			{
				int intVal = strtol(value.c_str(), NULL, 10);
				std::set<int>::iterator it = _port.find(intVal);
				if(it == _port.end())
					_port.insert(intVal);
			}
		}
		else if (key == "client_max_body_size")
		{
			lineStream >> value;
			_maxSize = strtol(value.c_str(), NULL, 10);
			if(value.find("M") != std::string::npos)
				_maxSize *= 1024 * 1024;
			else if(value.find("K") != std::string::npos)
				_maxSize *= 1024;
		}
		else if (key == "error_page")
		{
			int code;
			lineStream >> code;
			lineStream >> value;
			_errorPages.insert(std::make_pair(code, value));
		}
		else if (key == "location")
		{
			std::string locatConfig;
			locatConfig += line;
			locatConfig += "\n";
			while(std::getline(stream, line) && line.find("}") == std::string::npos)
			{
				locatConfig += line;
				locatConfig += "\n";
			}
			Location *newLocation = new Location(locatConfig);
			_locations.push_back(*newLocation);
		}
		else if(key == "}")
			break;
		else
			std::cerr << "Error: Unknown key: " << key << std::endl;
	}
	defaultConfig(servNum);
} */

/* void Config::printData() {
	std::cout << "Server name:  " << _serverName << std::endl;
	std::cout << "Ports:  ";
	for(std::set<int>::iterator it = _port.begin(); it != _port.end(); it++)
		std::cout << *it << "  ";
	std::cout << std::endl;
	std::cout << "Error pages:  ";
	std::map<int, std::string>::iterator it;
	for(it = _errorPages.begin(); it != _errorPages.end(); it++)
		std::cout << it->first << "  ";
	std::cout << std::endl;
	std::cout << "Max Size:  " << _maxSize << std::endl;
	for(std::vector<Location>::iterator it = _locations.begin(); it != _locations.end(); it++)
		it->printData();
} */

/* void Config::setLocation(Location *location) {
	_locations.push_back(location);
} */

/* std::set<int> Config::getPort() const {
	return _port;
}

std::string Config::getServerName() const {
	return _serverName;
} */

/* std::string Config::getErrorPage(const int &errorCode) const {
	std::map<int, std::string>::const_iterator it;
	it = _errorPages.find(errorCode);
	if (it != _errorPages.end())
		return it->second;
	return "";
}

int Config::getMaxSize() const {
	return _maxSize;
}

const Location& Config::getLocation(const std::string &location) const {
	std::vector<Location>::const_iterator it;
	for(it = _locations.begin(); it != _locations.end(); it++)
	{
		if(it->getLocation() == location)
			return *it;
	}
	static const Location invalidLocation("null");
	return invalidLocation;
} */
