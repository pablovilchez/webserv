#include "Request.hpp"

Request::Request(const std::string &raw) : _raw(raw) {
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
	char	buf[1024];
	_servDrive = getcwd(buf, sizeof(buf));
	handleRequest();
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

void	Request::parseBody(const char *buf, int bytesReceived) {
	static int	i = 0;
	std::string boundary = "\r\n\r\n";
	std::string receivedData(buf, bytesReceived);
	size_t boundaryPos = receivedData.find(boundary);

	if (_contentLength == 0) {
		std::cerr << "Content-Length header is missing or zero." << std::endl;
		return;
	}
	if (_body.empty())
		_body.reserve(_contentLength);
	if (i == 0 && boundaryPos != std::string::npos) {
		captureFileName(receivedData);
		_body.insert(_body.end(), receivedData.begin() + boundaryPos + boundary.size(), receivedData.end());
		i = 1;
	} else
		_body.insert(_body.end(), buf, buf + bytesReceived);

	if (receivedData.find(_boundary + "--") != std::string::npos) {
		if (fileExtension(_contentType) == true) {
			std::string	saveFileIn = _servDrive + "/var/drive/" + _fileName;
			std::ofstream outputFile(saveFileIn, std::ios::binary);
			if (outputFile.is_open()) {
				outputFile.write(_body.data(), _body.size());
				outputFile.close();
				setStatus("200 OK");
			} else
				std::cerr << "Error opening file for writing." << std::endl;
		}
		//_body.clear();
		//_done = true;
		buildResponse();
	}
}

void Request::parseHeader()
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
	if (_method != "GET" && _method != "POST" && _method != "DELETE" && _version != "HTTP/1.1")
		return (setStatus("400 Bad Request"));
	if (!validateRequest(_method))
		return;
	fileToOpen = extractPathFromUrl(_path);
	if (access(fileToOpen.c_str(), F_OK) == -1)
		return (setStatus("404 Not Found"));
	while ((end = _raw.find("\r\n", pos)) != std::string::npos && pos < _raw.size()) {
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
		bool	readyToAcceptPayload = false;

		if (readyToAcceptPayload)
			_responseHeader = "HTTP/1.1 100 Continue\r\n\r\n";
		else
			_responseHeader = "HTTP/1.1 400 Bad Request\r\n\r\n";
		setResponse();
		_done = true;
	}
}

void	Request::buildHeader() {
	std::stringstream	contLenStr;
	contLenStr << _contentLength;
	_responseHeader = "HTTP/1.1 " + _status + "\n";
	if (_redirectionLocation != "")
		_responseHeader += "Location: " + _redirectionLocation + "\n";
	_responseHeader += "Content-Type: " + _contentType + "\n\n";
	//_responseHeader += "Content-Length: " + contLenStr.str() + "\n\n";
}

void	Request::buildResponse() {
	if (_method == "GET") {
		std::fstream	fileStream(fileToOpen.c_str());
		if (fileStream) {
			_responseBody.assign(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());
			fileStream.close();
			setStatus("200 OK");
			setExtension(fileToOpen);
		}
		else if (!_responseBody.empty())
			setStatus("200 OK");
		else
			setStatus("500 Internal Server Error"); // Failed to open the file
	}
	else if (_method == "POST")
		_responseBody = "Request served";
	else if (_method == "DELETE")
		_contentType = "text/plain";
	buildHeader();
	setResponse();
	_done = true;
	std::cout << "Response header:  " << _responseHeader << std::endl;
	std::cout << "Response content: " << _responseBody << std::endl;
	std::cout << std::endl;
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
		std::cout << "HERE " << std::endl;
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
	_responseBody += "<html>\n <head>\n <title>Index of " + _location.getLocation() + "</title>\n </head>\n <body>";
	_responseBody += "<h1>These are files inside " + _location.getLocation() + "</h1>";
	_responseBody += "<ul>\n";
	while ((currDir = readdir(dir)) != NULL) {
		if (currDir->d_type == DT_REG && std::string(currDir->d_name).find(".html") != std::string::npos) {
			std::string	filePath = uri + "/" + std::string(currDir->d_name);
			_responseBody += "<li><a href=\"" + filePath + "\">" + filePath + "</a></li>\n";
		}
	}
	_responseBody += "</ul>\n";
	_responseBody += "</body>\n";
	_responseBody += "</html>\n";
	_contentType = "text/html";
	closedir(dir);
}

/* std::string	Request::extractDirectory(const std::string& path) {
	size_t lastSlashPos = path.find_last_of('/');
	size_t secondLastSlashPos = path.find_last_of('/', lastSlashPos - 1);

	if (lastSlashPos != std::string::npos && secondLastSlashPos != std::string::npos)
		return path.substr(secondLastSlashPos + 1, lastSlashPos - secondLastSlashPos - 1);
	std::string	dirPath;
	if (path.back() != '/')
		dirPath = path + '/';
	return dirPath;r
}
 */

void	Request::handleGetMethod(std::string &fileToOpen){
	
/* 	if (!validateRequest("GET") || access(fileToOpen.c_str(), R_OK) == -1)
		return; */
	if (fileOrDirectory(fileToOpen)) { // path is a directory
		if (!_location.getReturn().empty()) {
			std::map<int, std::string>	redirections = _location.getReturn();
			if (redirections.find(301) != redirections.end()) {
				std::string	redirectionLocation = redirections[301];
				setStatus("301 Moved Permanently");
			}
			buildResponse();
		}
		else if (!_location.getIndex().empty()) {
			const std::set<std::string>&	indexFiles = _location.getIndex();
			for (std::set<std::string>::const_iterator it = indexFiles.begin(); it != indexFiles.end(); it++){
				const std::string&	currIndexFile = *it;
				fileToOpen += (fileToOpen.back() != '/') ? '/' + currIndexFile : currIndexFile;
				if (access(fileToOpen.c_str(), F_OK) == 0) {
					buildResponse();
					break ;
				}
			}
		}
		else if (_location.getDirectoryListing()) {
			generateAutoIndex(fileToOpen);
			buildResponse();
		}
		else
			setStatus("404 Not Found");
	}
	else { // path is a file

		if (fileType(_extension)) {
			// if (!_location.getCgiExtension().empty())
				// Handle CGI processing
			setStatus("200 OK");
			buildResponse();
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
	char	resolvedPath[PATH_MAX];
	struct stat fileInfo;

	strcpy(resolvedPath, fileToDelete.c_str());

	if (realpath(fileToDelete.c_str(), resolvedPath) == nullptr) {
		setStatus("404 Not Found");
		return;
	}
	else if (stat(resolvedPath, &fileInfo) != 0 || !S_ISREG(fileInfo.st_mode) || access(resolvedPath, W_OK) != 0) {
		setStatus("403 Forbidden");
		return;
	}
	if (remove(resolvedPath) == 0)
		setStatus("204 No Content");
	else
		setStatus("500 Internal Server Error");
}

// por ahora falta "." ".." comprobaciones
std::string	Request::extractPathFromUrl(std::string& url) {
	size_t	firstSlashPos = url.find('/');
	size_t	secondSlashPos = url.find('/', firstSlashPos + 1);
    std::cout << "root: " << _location.getRoot() << std::endl;

    if (firstSlashPos != std::string::npos && secondSlashPos != std::string::npos) {
		size_t	pathStartPos = secondSlashPos;
		std::string	path = url.substr(pathStartPos);
		if (path[1])
			return (_servDrive + _location.getRoot() + path);
	}
	return (_servDrive + _location.getRoot());
}

void	Request::handleRequest() {
	parseHeader();
	if (_status == "" && !_done) {
		if (_method == "GET")
			handleGetMethod(fileToOpen);
		else if (_method == "POST")
			handlePostMethod();
		else if (_method == "DELETE")
			handleDeleteMethod(fileToOpen);
		else
			setStatus("405 Method Not Allowed");
	}
	if (!_done)
		buildResponse();
}

void Request::printData()
{
	std::cout << "Request _method:    " << _method << std::endl;
	std::cout << "Request _path:    " << _path << std::endl;
	std::cout << "Request _version:    " << _version << std::endl;
	std::cout << "Request _host:    " << _host << std::endl;
	std::cout << "Request _contentType:    " << _contentType << std::endl;
	std::cout << "Request _contentLength:    " << _contentLength << std::endl;
}

bool	Request::fileExtension(const std::string& contentType) {
	std::map<std::string, std::string> contentTypeExtensions;
	//std::cout << "in filext: FILE'S type: " << contentType << std::endl;
	contentTypeExtensions.insert(std::make_pair("image/jpeg", ".jpeg"));
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

bool	Request::fileType(const std::string& extension) {
	std::map<std::string, std::string> extensionToContentType;
	extensionToContentType.insert(std::make_pair(".jpeg", "image/jpeg"));
	extensionToContentType.insert(std::make_pair(".jpg", "image/jpeg"));
	extensionToContentType.insert(std::make_pair(".png", "image/png"));
	extensionToContentType.insert(std::make_pair(".gif", "image/gif"));
	extensionToContentType.insert(std::make_pair(".txt", "text/plain"));
	extensionToContentType.insert(std::make_pair(".html", "text/html"));
	extensionToContentType.insert(std::make_pair(".htm", "text/html"));
	extensionToContentType.insert(std::make_pair(".json", "application/json"));
	extensionToContentType.insert(std::make_pair(".pdf", "application/pdf"));
	extensionToContentType.insert(std::make_pair(".doc", "application/msword"));
	extensionToContentType.insert(std::make_pair(".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"));

	// Find the content type corresponding to the extension
	std::map<std::string, std::string>::iterator it = extensionToContentType.find(extension);
	if (it != extensionToContentType.end()) {
		_contentType = it->second;
		return true;
	} else {
		_contentType = "application/octet-stream";
		return false;
	}
}

bool	Request::isDirectory(const std::string& path) {
	struct stat info;
	if (stat(path.c_str(), &info) != 0)
		return false;
	return S_ISDIR(info.st_mode);
}

bool	Request::fileOrDirectory(const std::string& path) {
	if (isDirectory(path))
		return true;
	else {
		setExtension(path);
		return false;
	}
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

void	Request::setResponse()
{
	_response = _responseHeader + _responseBody;
}

std::string	Request::getResponse() const { return _response; }