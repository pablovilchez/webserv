#ifndef SERVER_HPP
# define SERVER_HPP

# ifndef DEBUG
#  define DEBUG false
# endif

# include "Config.hpp"

class Server
{
	public:
		Server();
		Server(const std::string &config);
		~Server();

	private:
		Config _config;
};

#endif
