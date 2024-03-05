#include "WebServer.hpp"

WebServer::WebServer() { }

WebServer::WebServer(const std::string &file) {
	parseConfigFile(file);

}

WebServer::~WebServer() { }
		
void WebServer::parseConfigFile(const std::string &file) {
	std::ifstream fileStream(file);
	std::string line;
	std::string serverConf;
	int server = 0;
	int i = 1;

	if (!fileStream.is_open())
	{
		std::cerr << "Error: " << strerror(errno) << std::endl;
		return;
	}
	while (std::getline(fileStream, line))
	{
		serverConf = "";
		if (line.find("server") != std::string::npos)
		{
			while (std::getline(fileStream, line) && !line.find("{"))
				serverConf += line;
			while (std::getline(fileStream, line))
			{
				serverConf += line;
				if (line.find("{") != std::string::npos)
					i++;
				if (line.find("}") != std::string::npos)
					i--;
				if (i == 0)
					break;
			}
		}
		_servers.push_back(Server(serverConf));
	}
	fileStream.close();
}
