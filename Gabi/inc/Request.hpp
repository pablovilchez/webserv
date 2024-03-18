#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <unistd.h>
#include <dirent.h>
#include <cstdio>
#include <sys/stat.h>
#include <sys/types.h>

#include "header.hpp"
#include "Location.hpp"
#include "Config.hpp"

class Location;
class Config;

class Request
{
	public:
		Request(const std::string &raw);
		~Request();

		void	handleRequest();
		void	handleGetMethod(std::string &fileToOpen);
		void	handlePostMethod();
		void	handleDeleteMethod(std::string &fileToDelete);

		std::string	extractPathFromUrl(std::string& url);
		bool	validateRequest(const std::string& method);
		void	parseHeader();
		void	parseBody(const char *buf, int bytesReceived);
		void	printData();
		void	captureFileName(std::string receivedData);
		bool	fileExtension(const std::string& contentType);
		bool	fileType(const std::string& extension);
		void	setStatus(const std::string &status);
		std::string getPath() const;
		std::string getRaw() const;
		std::string getResponseHeader() const;
		std::string getResponseBody() const;
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
		Config		_config;
		std::string	_responseHeader;
		std::string	_responseBody;
		std::string	_response; // the whole response from server is saved here !!
		std::string	_redirectionLocation;
		bool		_done;
		std::string	fileToOpen;
		std::string	_servDrive;
		std::map<std::string, std::string> contentTypeExtensions;
};

#endif