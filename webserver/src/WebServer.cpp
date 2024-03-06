#include "WebServer.hpp"

WebServer::WebServer() { }

WebServer::WebServer(const std::string &file) {
	parseConfigFile(file);

}

WebServer::~WebServer() { }
		
void WebServer::parseConfigFile(const std::string &file) {
	std::ifstream fileStream(file);
	std::string line;
	std::string buffer;
	int servers = 0;
	int i = 0;

	if (!fileStream.is_open())
	{
		std::cerr << "Error: " << strerror(errno) << std::endl;
		return;
	}
	while (std::getline(fileStream, line))
	{
		
		if (line.find("server") != std::string::npos && line.find("{") != std::string::npos)
		{
			buffer = line;
			buffer += "\n";
			i = 1;
			while (std::getline(fileStream, line) && i > 0)
			{
				buffer += line;
				buffer += "\n";
				if (line.find("{") != std::string::npos)
					i++;
				if (line.find("}") != std::string::npos)
					i--;
			}
			_servers.push_back(Server(buffer));
			servers++;
		}
	}
	fileStream.close();
	std::cout << servers << " servers created." << std::endl;
}