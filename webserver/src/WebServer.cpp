#include "WebServer.hpp"

WebServer::WebServer() {

}

WebServer::WebServer(const std::string &file) {
	parseConfigFile(file);
}

WebServer::WebServer(const WebServer &other) {
	std::vector<const Server*>::const_iterator vector_it;
	for (vector_it = other._servers.begin(); vector_it != other._servers.end(); vector_it++) {
		_servers.push_back(*vector_it);
	}

	std::vector<pollfd>::const_iterator it;
	for (it = other._poll_fds.begin(); it != other._poll_fds.end(); it++) {
		_poll_fds.push_back(*it);
	}

	std::map<int, std::vector<const Server*> >::const_iterator map_it;
	std::map<int, std::vector<const Server*> >::const_iterator end = other._portsMap.end();
	for (map_it = other._portsMap.begin(); map_it != end; map_it++) {
		_portsMap[map_it->first] = map_it->second;
	}
}

WebServer &WebServer::operator=(const WebServer &other) {
	if (this != &other) {
		_servers.clear();
		std::vector<const Server*>::const_iterator vector_it;
		for (vector_it = other._servers.begin(); vector_it != other._servers.end(); vector_it++) {
			_servers.push_back(*vector_it);
		}

		_poll_fds.clear();
		std::vector<pollfd>::const_iterator it;
		for (it = other._poll_fds.begin(); it != other._poll_fds.end(); it++) {
			_poll_fds.push_back(*it);
		}

		_portsMap.clear();
		std::map<int, std::vector<const Server*> >::const_iterator map_it;
		std::map<int, std::vector<const Server*> >::const_iterator end = other._portsMap.end();
		for (map_it = other._portsMap.begin(); map_it != end; map_it++) {
			_portsMap[map_it->first] = map_it->second;
		}
	}
	return *this;
}

WebServer::~WebServer() {
	std::vector<const Server*>::const_iterator it;
	for (it = _servers.begin(); it != _servers.end(); it++) {
		delete *it;
	}
}

bool web_isComment(const std::string &line) {
	for (size_t i = 0; i < line.length(); i++)
	{
		if (!std::isspace(line[i]))
			return line[i] == '#';
	}
	return true;
}

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
	std::map<int, std::vector<const Server*> >::const_iterator it2;
	for (it2 = _portsMap.begin(); it2 != _portsMap.end(); it2++) {
		std::cout << "Port: " << it2->first << std::endl;
		std::vector<const Server*>::const_iterator it3 = it2->second.begin();
		for (it3 = it2->second.begin(); it3 != it2->second.end(); it3++) {
			std::cout << "Server: " << (*it3)->getConfig().getServerName() << std::endl;
		}
	}
	std::cout << "Server size:" << _servers.size() << std::endl;
	if(DEBUG) std::cout << "WebServer:" << servers << " servers created" << std::endl;
}

