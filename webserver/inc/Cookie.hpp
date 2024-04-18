#ifndef COOKIE_HPP
# define COOKIE_HPP

#include "libraries.hpp"

#define MAXAGE 10

class Cookie {
	public:
		Cookie();
		~Cookie();
		void	checkCookieExpiry(const char *requestBuffer);
		void	generateTimeStamp();
		static std::string	generateRandomString(int length);
		std::string	formatTime(std::time_t timeInSeconds);
		std::string	getCookieBody() const;
		std::string	getCookieHeader() const;
		std::string	getCookieId() const;
		std::string	setCookieHeader();
		bool	isExpired() const;
	private:
		int			_cookieMaxAge;
		std::string	_cookieSessionId;
		int			_firstAccessTime;
		int			_lastEntry;
		std::vector<std::string> _loginHistory;
		std::string	_cookieHeader;
		bool		_expired;
};

#endif