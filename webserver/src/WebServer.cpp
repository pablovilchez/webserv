#include "WebServer.hpp"

WebServer::WebServer() { }

WebServer::WebServer(const std::string &file) {
	parseConfigFile(file);

}

WebServer::~WebServer() { }


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
			_servers.push_back(Server(buffer, servNum++));
			servers++;
		}
	}
	fileStream.close();
	if(DEBUG) std::cout << "WebServer:" << servers << " servers created" << std::endl;
}
