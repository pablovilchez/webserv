#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "libraries.hpp"
#include "WebServer.hpp"

class Location;
class Server;

class Request
{
	public:
		/* Request();
		Request(const Request& other);
		Request& operator=(const Request& other); */
		Request(const std::string &raw, const Server &srv);
		~Request();

		void	handleRequest();
		void	handleGetMethod(std::string &fileToOpen);
		void	handlePostMethod();
		void	handleDeleteMethod(std::string &fileToDelete);
		bool	handleError();
		void	defaultErrorPage(std::string errorCode);
		std::string	extractPathFromUrl(std::string& url);
		bool	validateRequest(const std::string& method);
		void	parseHeader();
		void	parseBody(const char *buf, int bytesReceived);
		void	printData();
		bool	captureFileName(std::string receivedData);
		bool	fileExtension(const std::string& contentType);
		bool	fileType(const std::string& extension);
		void	setStatus(const std::string &status);
		void	setExtension(const std::string &path);
		std::string getPath() const;
		std::string getRaw() const;
		std::string getResponseHeader() const;
		std::string getResponseBody() const;
		bool		isResponseReady() const;
		void	setResponse();
		std::string	getResponse() const;
		void	generateAutoIndex(std::string &uri);
		void	buildHeader();
		void	buildResponse();
		std::string	extractDirectory(const std::string& path);
		std::string getMethod() const;
		std::string getExtension() const;
		bool isDirectory(const std::string& path);
		bool fileOrDirectory(const std::string& path);


	private:
		std::string	_raw;
		std::string	_method;
		std::string	_path;
		std::string	_version;
		std::string	_host;
		std::string	_contentType;
		std::string	_extension;
		std::string	_fileName;
		int		_contentLength;
		std::vector<char>	_body;
		std::string	_boundary;
		std::string	_status;
		Location	_location;
		Server		_config;
		std::string	_responseHeader;
		std::string	_responseBody;
		std::string	_response; // the whole response from server is saved here !!
		std::string	_redirectionLocation;
		bool		_done;
		std::string	fileToOpen;
		std::string	_servDrive;
		std::string	_errorLocation;
		std::map<std::string, std::string> contentTypeExtensions;
};

#endif