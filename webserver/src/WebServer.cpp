#include "WebServer.hpp"

WebServer::WebServer() : _servers(), _poll_fds(), _portsMap(){

}

WebServer::WebServer(const WebServer &other) {
	for (std::vector<Server>::const_iterator it = other._servers.begin(); it != other._servers.end(); it++) {
		this->_servers.push_back(*it);
	}
	_poll_fds = other._poll_fds;
	perror("Copy constructor for _portsMap in progress...");
}

WebServer::WebServer(const std::string &file) {
	parseConfigFile(file);
}

WebServer::~WebServer() {

}

WebServer &WebServer::operator=(const WebServer &other) {
	if (this != &other) {
		_servers.clear();
		for (std::vector<Server>::const_iterator it = other._servers.begin(); it != other._servers.end(); it++) {
			this->_servers.push_back(*it);
		}
		_poll_fds = other._poll_fds;
		_portsMap.clear();
		perror("Asignation operator for _portsMap in progress...");
	}
	return *this;
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
			Server servAux = Server(buffer, servNum++);
			_servers.push_back(servAux);

			std::set<int>ports = servAux.getConfig().getPort();
			std::set<int>::iterator it;
			for (it = ports.begin(); it != ports.end(); it++) {
				_portsMap[*it].push_back(servAux);
			}
			servers++;
		}
	}
	fileStream.close();
	std::map<int, std::vector<Server> >::const_iterator it2;
	for (it2 = _portsMap.begin(); it2 != _portsMap.end(); it2++) {
		std::cout << "Port: " << it2->first << std::endl;
		std::vector<Server>::const_iterator it3 = it2->second.begin();
		it3++;
		std::cout << "Server: " << it3->getConfig().getServerName() << std::endl;
		/* for (it3 = it2->second.begin(); it3 != it2->second.end(); it3++) {
			std::cout << "Server: " << it3->getConfig().getServerName() << std::endl;
		} */
	}
	std::cout << "Server size:" << _servers.size() << std::endl;
	//if(DEBUG) std::cout << "WebServer:" << servers << " servers created" << std::endl;
}
