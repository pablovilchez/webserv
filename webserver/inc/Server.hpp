#ifndef SERVER_HPP
#define SERVER_HPP

#include "WebServer.hpp"
#include "Config.hpp"

class Server {
	private:
		Config	_config;
		void _fillServerAddr();
		void _startServerLoop();

	public:
		Server();
		Server(Server const &src);
		Server &operator=(Server const &src);
		Server(const std::string &config, int servNum);
		~Server();
		const Config &getConfig() const;
};

#endif
