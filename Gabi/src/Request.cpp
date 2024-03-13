/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gkrusta <gkrusta@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/02 21:18:22 by pvilchez          #+#    #+#             */
/*   Updated: 2024/03/13 16:49:31 by gkrusta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <sys/types.h>
Request::Request(const std::string &raw, int clientSocket) : _raw(raw) {
	_method = "";
	_path = "";
	_version = "";
	_host = "";
	_contentType = "";
	_extension = "";
	_contentLength = 0;
	_fileName = "";
	_status = "";
	_boundary = "";
	_responseHeader = "";
	_responseBody = "";
	_done = false;
	_redirectionLocation = "";
	fileToOpen = "";
	handleRequest(clientSocket);
}

Request::~Request() { }

std::string Request::getRaw() const
{
	return _raw;
}

void	Request::captureFileName(std::string receivedData) {
	size_t filenamePos = receivedData.find("filename=");

	if (filenamePos != std::string::npos) {
		size_t quoteStart = receivedData.find("\"", filenamePos);
		size_t quoteEnd = receivedData.find("\"", quoteStart + 1);
		if (quoteStart != std::string::npos && quoteEnd != std::string::npos) {
			if (quoteEnd - quoteStart == 1)
				_fileName = "default";
			else
				_fileName = receivedData.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
		}
	}
}

bool Request::fileExtension(const std::string& contentType) {
	std::map<std::string, std::string> contentTypeExtensions;
	std::cout << "in filext: FILE'S type: " << contentType << std::endl;
	contentTypeExtensions.insert(std::make_pair("image/jpeg", ".jpg"));
	contentTypeExtensions.insert(std::make_pair("image/jpg", ".jpg"));
	contentTypeExtensions.insert(std::make_pair("image/png", ".png"));
	contentTypeExtensions.insert(std::make_pair("image/gif", ".gif"));
	contentTypeExtensions.insert(std::make_pair("text/plain", ".txt"));
	contentTypeExtensions.insert(std::make_pair("text/html", ".html"));
	contentTypeExtensions.insert(std::make_pair("application/json", ".json"));
	contentTypeExtensions.insert(std::make_pair("application/pdf", ".pdf"));
	contentTypeExtensions.insert(std::make_pair("application/msword", ".doc"));
	contentTypeExtensions.insert(std::make_pair("application/vnd.openxmlformats-officedocument.wordprocessingml.document", ".docx"));

	std::map<std::string, std::string>::iterator it = contentTypeExtensions.find(contentType);
	if (it != contentTypeExtensions.end()) {
		_extension = it->second;
		return true;
	} else {
		_contentType = "Unsupported content type";
		return false;
	}
}

/* bool Request::fileExtension() {
	// an image
	if (_contentType.find("image/jpeg") != std::string::npos ||
		_contentType.find("image/png") != std::string::npos ||
		_contentType.find("image/gif") != std::string::npos) {
		return true;
	}
	// text file
	else if (_contentType.find("text/plain") != std::string::npos ||
				_contentType.find("text/html") != std::string::npos ||
				_contentType.find("application/json") != std::string::npos) {
		return true;
	}
	// document file
	else if (_contentType.find("application/pdf") != std::string::npos ||
				_contentType.find("application/msword") != std::string::npos ||
				_contentType.find("application/vnd.openxmlformats-officedocument.wordprocessingml.document") != std::string::npos) {
		return true;
	}
	std::cerr << "Unsupported content type." << std::endl;
	return false;
} */

void	Request::parseBody(const char *buf, int bytesReceived) {
	static int	i = 0;
	std::string boundary = "\r\n\r\n";
	std::string receivedData(buf, bytesReceived);
	size_t boundaryPos = receivedData.find(boundary);

/* 	if (_contentLength == 0) {
		std::cerr << "Content-Length header is missing or zero." << std::endl;
		return;
	} */
	if (_body.empty())
		_body.reserve(_contentLength);
	if (i == 0 && boundaryPos != std::string::npos) {
		captureFileName(receivedData);
		_body.insert(_body.end(), receivedData.begin() + boundaryPos + boundary.size(), receivedData.end());
		i = 1;
	} else
		_body.insert(_body.end(), buf, buf + bytesReceived);

	// Save the file with the appropriate extension
	if (receivedData.find(_boundary + "--") != std::string::npos) {
		//std::cout << "\n\nBOUNDRY" << _boundary << std::endl;
		if (fileExtension(_contentType) == true) {
			std::ofstream outputFile(_fileName, std::ios::binary);
			if (outputFile.is_open()) {
				outputFile.write(_body.data(), _body.size());
				outputFile.close();
				std::cout << "\n\nfile created: \n\n\n" << _contentType << std::endl; 
			} else
				std::cerr << "Error opening file for writing." << std::endl;
		}
		//_body.clear();
		//_done = true;
		build
	}
}

void Request::parseHeader(int clientSocket)
{
	size_t	pos = 0;
	size_t	end = _raw.find("\r\n");


	if (end != std::string::npos) {
		std::string	line = _raw.substr(pos, end - pos);
		size_t	space1 = line.find(' ');
		size_t	space2 = line.find(' ', space1 + 1);

		if (space1 != std::string::npos || space2 != std::string::npos) {
			_method = line.substr(0, space1);
			_path = line.substr(space1 + 1, space2 - space1 - 1);
			_version = line.substr(space2 + 1);
		}
		pos = end + 2;
	}
	if ((_method != "GET" && _method != "POST" && _method != "DELETE") || _version != "HTTP/1.1")
			return (setStatus("400 Bad Request"));
	fileToOpen = extractPathFromUrl(_path);
	if (access(fileToOpen.c_str(), F_OK) == -1) {
		std::cout << "\n\nvalue: " << fileToOpen << std::endl; 
		return (setStatus("404 Not Found"));
	}
	while ((end = _raw.find("\r\n", pos)) != std::string::npos && pos < _raw.size()) {
		std::cout << "\n\nvalue: " << _version << std::endl; 
		std::string	line = _raw.substr(pos, end - pos);
		size_t	colPos = line.find(':');
		if (colPos != std::string::npos) {
			std::string	key = line.substr(0, colPos);
			std::string	value = line.substr(colPos + 2);
			if (key == "Host")
				_host = value;
			else if (key == "Content-Length")
				_contentLength = atoi(value.c_str());
			else if (key == "Content-Type") {
				size_t semicolPos = value.find(';');
				if (semicolPos != std::string::npos)
					_contentType = value.substr(0, semicolPos);
				else
					_contentType = value;
				size_t bPos = value.find("boundary=");
				if (bPos != std::string::npos) {
					_boundary = value.substr(bPos + 9);
				}
			}
		}
		pos = end + 2;
	}
	size_t expectHeaderPos = _raw.find("Expect: 100-continue");
	if (expectHeaderPos != std::string::npos) {

		bool readyToAcceptPayload = true;

		if (readyToAcceptPayload) {
			std::string continueResponse = "HTTP/1.1 100 Continue\r\n\r\n";
			send(clientSocket, continueResponse.c_str(), continueResponse.size(), 0);
		} else {
			std::string finalResponse = "HTTP/1.1 400 Bad Request\r\n\r\n";
			send(clientSocket, finalResponse.c_str(), finalResponse.size(), 0);
			// Close the connection
		}
	}
}

void	Request::buildHeader() {
	std::stringstream	contLenStr;
	contLenStr << _contentLength;
	_responseHeader = "HTTP/1.1 " + _status + "\r\n";
	if (_redirectionLocation != "")
		_responseHeader += "Location: " + _redirectionLocation + "\r\n";
	_responseHeader += "Content-Type: " + _contentType + "\r\n";
	_responseHeader += "Content-Length: " + contLenStr.str() + "\r\n";
	_responseHeader += "\r\n"; 
}

void	Request::buildResponse(std::string &fileToOpen) {
	std::fstream	fileStream(fileToOpen.c_str());
	if (fileStream) {
		_responseBody.assign(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());
		fileStream.close();
		buildHeader();
	} else {
		setStatus("500 Internal Server Error"); // Failed to open the file
	}
	_done = true;
}

bool	Request::validateRequest(const std::string& method) {
	size_t	firstSlashPos = _path.find('/');
	size_t	secondSlashPos = _path.find('/', firstSlashPos + 1);
	std::string requestedLocation;
	if (firstSlashPos != std::string::npos && secondSlashPos != std::string::npos)
		requestedLocation = _path.substr(firstSlashPos, secondSlashPos - firstSlashPos);
	else
		requestedLocation = _path;
	_location = _config.getLocation(requestedLocation);
	if (_location.getLocation() == "null") {
		setStatus("404 Not Found");
		return false;
	}
	else if (!_location.isAcceptedMethod(method)) {
		setStatus("403 Forbidden");
		return false;
	}
	else
		return true;
}

void	Request::generateAutoIndex(std::string &uri) {

	DIR *dir = opendir(uri.c_str());
	struct dirent	*currDir;

	if (!dir) {
		std::cerr << "Error opening directory" << std::endl;
		return;
	}
	_responseBody += "<html>\n <head>\n <title>Index of " + uri + "</title>\n </head>\n <body>";
	_responseBody += "<h1>Index of " + uri + "</h1>";
	_responseBody += "<ul>\n";
	while ((currDir = readdir(dir)) != NULL) {
		if (currDir->d_type == DT_REG && std::string(currDir->d_name).find(".html") != std::string::npos) {
			std::string	filePath = uri + std::string(currDir->d_name);
			_responseBody += "<li><a href=\"" + filePath + "\">" + filePath + "</a></li>\n";
		}
	}
	_responseBody += "</ul>\n";
	_responseBody += "</body>\n";
	_responseBody += "</html>\n";
	closedir(dir);
	buildHeader();

}

std::string	Request::extractDirectory(const std::string& path) {
	size_t lastSlashPos = path.find_last_of('/');
	size_t secondLastSlashPos = path.find_last_of('/', lastSlashPos - 1);

	if (lastSlashPos != std::string::npos && secondLastSlashPos != std::string::npos)
		return path.substr(secondLastSlashPos + 1, lastSlashPos - secondLastSlashPos - 1);
	return "";
}

void	Request::handleGetMethod(std::string &fileToOpen){
	
	if (!validateRequest("GET") || access(fileToOpen.c_str(), R_OK) == -1)
		return;
	if (_path.back() == '/') { // path is a directory
		const std::string&	dir = extractDirectory(_path);
		if (!_location.getReturn().empty()) {
			std::map<int, std::string>	redirections = _location.getReturn();
			if (redirections.find(301) != redirections.end()) {
				std::string	redirectionLocation = redirections[301];
				setStatus("301 Moved Permanently");
				_contentType = "text/html";
				buildHeader();
			}
		}
		else if (!_location.getIndex().empty() && _location.isIndexFile(dir)) {
			const std::set<std::string>&	indexFiles = _location.getIndex();
			
			for (std::set<std::string>::const_iterator it = indexFiles.begin(); it != indexFiles.end(); it++){
				const std::string&	currIndexFile = *it;
				std::string	filePath = fileToOpen + currIndexFile;
				if (access(filePath.c_str(), F_OK) == 0) {
					buildResponse(filePath);
					break ;
				}
			}
		}
		else if (_location.getDirectoryListing())
			generateAutoIndex(fileToOpen);
		else
			setStatus("404 Not Found");
	}
	else { // path is a file
		if (fileExtension(_contentType)) {
			// if (!_location.getCgiExtension().empty())
				// Handle CGI processing
			buildResponse(fileToOpen);
/* 			else
				setStatus("500 Internal Server Error"); // Unable to open file */
		}
	}
	_done = true;
}

void	Request::handlePostMethod(){
	if (!_location.isAcceptedMethod("POST"))
		setStatus("403 Forbiden");
	else if (_config.getMaxSize() < _contentLength)
		setStatus("413 Request Entity Too Large");
/* 	else
		handleCgi(); */
}

void	Request::handleDeleteMethod(std::string &fileToDelete){
	if (!validateRequest("DELETE"))
		return;
	else if (remove(fileToDelete.c_str()) != 0)
		setStatus("500 Internal Server Error");
	else
		setStatus("204 No Content");
}

std::string	Request::extractPathFromUrl(std::string& url) {
	size_t	firstSlashPos = url.find('/');
	size_t	secondSlashPos = url.find('/', firstSlashPos + 1);

	if (firstSlashPos != std::string::npos && secondSlashPos != std::string::npos) {
		size_t	pathStartPos = secondSlashPos;
		std::string	path = url.substr(pathStartPos);
		if (path[1])
			return (_location.getRoot() + path);
	}
	return (_location.getRoot());
}

void	Request::handleRequest(int clientSocket) {
/* 	std::string	fileToOpen;

	fileToOpen = extractPathFromUrl(_path);
	if (access(fileToOpen.c_str(), F_OK) == -1)
		setStatus("404 Not Found"); */
	parseHeader(clientSocket);
	if (_status == "") {
		if (_method == "GET")
			handleGetMethod(fileToOpen);
		else if (_method == "POST")
			handlePostMethod();
		else if (_method == "DELETE")
			handleDeleteMethod(fileToOpen);
		else
			setStatus("405 Method Not Allowed");
	}
	buildResponse(fileToOpen);
}

void Request::printData()
{
	std::cout << "Request _method:    " << _method << std::endl;
	std::cout << "Request _path:    " << _path << std::endl;
	std::cout << "Request _version:    " << _version << std::endl;
	std::cout << "Request _host:    " << _host << std::endl;
	std::cout << "Request _contentType:    " << _contentType << std::endl;
	std::cout << "Request _contentLength:    " << _contentLength << std::endl;
	//std::cout << "Request _body:    " << _body << std::endl;

}

std::string Request::getPath() const
{
	return _path;
}

void	Request::setStatus(const std::string &status) {
	_status = status;
}

std::string	Request::getResponseHeader() const {
	return _responseHeader;
}

std::string	Request::getResponseBody() const {
	return _responseBody;
}


std::string Request::getMethod() const
{
	return _method;
}

std::string Request::getExtension() const
{
	return _extension;
}

/* std::string Request::getHeader() const
{
	return _header;
} */