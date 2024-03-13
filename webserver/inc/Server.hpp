#ifndef SERVER_HPP
#define SERVER_HPP

#include "WebServer.hpp"
#include "Config.hpp"

class Server {
	private:
		const Config *_config;

	public:
		Server();
		Server(const Server &src);
		Server &operator=(const Server &src);
		Server(const std::string &config, int servNum);
		~Server();
		const Config &getConfig() const;
};

#endif
