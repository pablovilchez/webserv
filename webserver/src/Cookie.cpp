#include "Cookie.hpp"

Cookie::Cookie() {
	_cookieSessionId = generateRandomString(6);
	_firstAccessTime = std::time(nullptr);
	_cookieMaxAge = MAXAGE;
	_lastEntry = _firstAccessTime;
	_expired = false;
	_cookieHeader = setCookieHeader();
}

Cookie::~Cookie() { }

std::string	Cookie::formatTime(std::time_t timeInSeconds) {
	std::string	formattedTime;
	char buffer[80];

	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&timeInSeconds));
	formattedTime = buffer;
	return (formattedTime);
}

std::string	Cookie::generateRandomString(int length) {
	const char charset[] =
		"0123456789"
		"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const int charsetSize = sizeof(charset) - 1;
	std::string randomString;

	for (int i = 0; i < length; ++i) {
		randomString += charset[rand() % charsetSize];
	}
	return randomString;
}

void	Cookie::checkCookieExpiry(const char *requestBuffer) {
	std::time_t	currTime = std::time(nullptr);

	std::string receivedData(requestBuffer);
	size_t idPos = receivedData.find("Sec-Fetch-Dest: document");

	if (currTime > (_firstAccessTime + _cookieMaxAge)) {
		_loginHistory.clear();
		_cookieSessionId = generateRandomString(6);
		_firstAccessTime = currTime;
		_lastEntry = currTime;
		_cookieHeader = setCookieHeader();
		_expired = true;
		return ;
	}
	else {
		_cookieHeader = "";
		if (idPos != std::string::npos)
			generateTimeStamp();
	}
}

void	Cookie::generateTimeStamp() {
	std::time_t	currTime = std::time(nullptr);
	std::string	formattedTime = formatTime(currTime);

	if (_loginHistory.empty()) {
		_loginHistory.push_back("LOG IN => " + formattedTime);
	}
	else {
		_loginHistory.push_back(formattedTime);
	}
	_lastEntry = currTime;
}

std::string	Cookie::getCookieBody() const {
	std::ostringstream	oss;

	if (_loginHistory.empty())
		return ("");
	for (std::vector<std::string>::const_iterator it = _loginHistory.begin(); it != _loginHistory.end(); ++it) {
		oss << "<div style=\"text-align:center;color:white;\">";
		oss << *it;
		oss << "</div>";
	}
	return (oss.str());
}

std::string	Cookie::getCookieHeader() const { return _cookieHeader; }

std::string	Cookie::setCookieHeader() {
	std::stringstream	maxAge;
	maxAge << _cookieMaxAge;

	_cookieHeader += "Set-Cookie: session_id=" + _cookieSessionId + "; Max-Age=" + maxAge.str();
	_cookieHeader += "; Path=/; Secure; HttpOnly";
	_cookieHeader += "\r\n";
	return (_cookieHeader);
}

bool	Cookie::isExpired() const { return _expired; }

std::string	Cookie::getCookieId() const { return _cookieSessionId; }