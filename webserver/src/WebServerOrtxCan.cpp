#include "WebServer.hpp"

WebServer::WebServer() {

}

WebServer::WebServer(const std::string &file) : _correctConfig(false) {
	_running = true;
	_correctConfig = parseConfigFile(file);
}

WebServer::WebServer(const WebServer &other) {
	_servers = other._servers;
	_poll_fds = other._poll_fds;
	_listeners = other._listeners;
	_portsMap = other._portsMap;
	_correctConfig = other._correctConfig;
}

WebServer &WebServer::operator=(const WebServer &other) {
	if (this != &other) {
		_servers = other._servers;
		_poll_fds = other._poll_fds;
		_listeners = other._listeners;
		_portsMap = other._portsMap;
		_correctConfig = other._correctConfig;
	}
	return *this;
}

WebServer::~WebServer() {

}

