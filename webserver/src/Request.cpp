#include "Request.hpp"

Request::Request(const std::string &raw, const Server &srv, const Cookie &sessionCookie) : _raw(raw), _config(srv), _cookie(sessionCookie) {
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
	_response = "";
	_cgiResponse = "";
	_done = false;
	_isChunked = false;
	_redirectionLocation = "";
	fileToOpen = "";
	char	buf[1024];
	_servDrive = getcwd(buf, sizeof(buf));
	_errorLocation = "/var/srv_" + _config.getServerName() + "/error";
	_cgiFile = "";
	_rawParams = "";
	handleRequest();
};

Request::Request() {};

Request::Request(Request const& other) {
	*this = other;
};
Request::~Request() {};

std::string Request::getRaw() const
{
	return _raw;
}

std::string	Request::dafaultFileCount() {
	int i = 1;
	std::string	folder = _servDrive + _location.getRoot() + "/default";

	while (true) {
		std::ostringstream oss;
		if (access(folder.c_str(), F_OK) == -1)
			break;
		i++;
		oss << i;
		folder = _servDrive + _location.getRoot() + "/default" + oss.str();
	}
	if (i == 1)
		return ("default");
	else {
		std::ostringstream oss2;
		oss2 << i;
		return ("default" + oss2.str());
	}
}

bool	Request::captureFileName(std::string receivedData) {
	int flag = 0;
	size_t filenamePos = receivedData.find("filename=");
	size_t flagSet = receivedData.find("Content-Disposition:");

	if (flagSet != std::string::npos)
		flag = 1;
	if (filenamePos != std::string::npos) {
		size_t quoteStart = receivedData.find("\"", filenamePos);
		size_t quoteEnd = receivedData.find("\"", quoteStart + 1);
		if (quoteStart != std::string::npos && quoteEnd != std::string::npos) {
			_fileName = receivedData.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
		}
		size_t contTypePos = receivedData.find("Content-Type: ", filenamePos);
		if (contTypePos != std::string::npos) {
			size_t contTypeEndPos = receivedData.find("\r\n", contTypePos);
			if (contTypeEndPos != std::string::npos) {
				_contentType = receivedData.substr(contTypePos + 14, contTypeEndPos - contTypePos - 14);
			}
		}
	}
	else
		_fileName = dafaultFileCount();
	return (flag);
}

void	Request::parseBody(const char *buf, int bytesReceived) {
	static int	i = 0;
	size_t	endPos = 0;

	std::string boundary = "\r\n\r\n";
	std::string receivedData(buf, bytesReceived);
	size_t	boundaryPos = receivedData.find(boundary);

	if (_body.empty())
		_body.reserve(_contentLength);
	if ((i < 2 && boundaryPos != std::string::npos) || _isChunked) {
		captureFileName(receivedData);
		_body.insert(_body.end(), receivedData.begin() + boundaryPos + boundary.size(), receivedData.end());
		i++;
	} else
		_body.insert(_body.end(), buf, buf + bytesReceived);
	std::string bodyString(_body.begin(), _body.end());

	if (bodyString.find(_boundary + "--") != std::string::npos)
		endPos = bodyString.find(_boundary + "--");
	else if (bodyString.find("0\r\n\r\n") != std::string::npos)
		endPos = bodyString.find("0\r\n\r\n");
	else if (_body.size() == (unsigned long)_contentLength)
		endPos = _contentLength + 4;
	if (endPos != 0) {
		if (fileExtension(_contentType)) {
			std::string	saveFileIn = _servDrive + _location.getRoot() + "/" + _fileName;
			std::ofstream outputFile(saveFileIn.c_str(), std::ios::binary);
			if (_isChunked)
				endPos = dechunkBody();
			if (outputFile.is_open()) {
				outputFile.write(_body.data(), endPos - 4);
				outputFile.close();
				setStatus("200 OK");
			} else {
				setStatus("404 OK");
				std::cerr << "Error opening file for writing." << std::endl;
			}
		}
		_body.clear();
		i = 0;
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
			size_t	paramsFound = _path.find('?'); // search for parameters in the URL
			if (paramsFound != std::string::npos) {
				_rawParams = _path.substr(paramsFound + 1);
				_path = _path.substr(0, paramsFound);
			}
			_version = line.substr(space2 + 1);
		}
		pos = end + 2;
	}
	if (_method != "GET" && _method != "POST" && _method != "DELETE" && _version != "HTTP/1.1") {
		_done = true;
		return (setStatus("400 Bad Request"));
	}
	if (!validateRequest(_method)) {
		_done = true;
		return;
	}
	fileToOpen = extractPathFromUrl(_path);
	if (access(fileToOpen.c_str(), F_OK) == -1) {
		_done = true;
		return (setStatus("404 Not Found"));
	}
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
			else if (key == "Transfer-Encoding" && value == "chunked")
				_isChunked = true;
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
		else
			break ;
		pos = end + 2;
	}
	if (_contentLength == 0 && _isChunked) {
		_contentLength = _raw.size() - pos;
	}
}

void	Request::buildHeader() {
	std::stringstream	contLenStr;
	contLenStr << _responseBody.size();
	_responseHeader = "HTTP/1.1 " + _status + "\r\n";
	if (_redirectionLocation != "")
		_responseHeader += "Location: " + _redirectionLocation + "\r\n";
	_responseHeader += "Content-Type: " + _contentType + "\r\n";
	_responseHeader += "Content-Length: " + contLenStr.str() + "\r\n";
	_responseHeader += "Connection: Keep-Alive\r\n";
	_responseHeader += "Keep-Alive: timeout=5, max=100\r\n";
	if (_cookie.getCookieHeader() != "")
		_responseHeader += _cookie.getCookieHeader();
	_responseHeader += "\r\n";
}

void	Request::buildResponse() {
	if (!handleError()) {
		if (_method == "GET" && _cgiResponse == "") {
			std::cout << "CGI response is empty." << std::endl;
			std::fstream	fileStream(fileToOpen.c_str());
			if (!_responseBody.empty())
				setStatus("200 OK");
			else if (fileStream) {
				_responseBody.assign(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());
				fileStream.close();
				setExtension(fileToOpen);
				if (_cookie.getCookieBody() != "" && _extension == (".html")) {
					std::size_t pos = _responseBody.find("</body>");
					if (pos != std::string::npos && _config.getServerName() == "cookie") {
						std::string loginTimeHTML = _cookie.getCookieBody();
						_responseBody.insert(pos, loginTimeHTML);
					}
				}
			}
		}
		else if (_method == "POST" || _method == "DELETE" || _cgiResponse != "") {
			_responseBody += "<html>\n <head>\n <title>Posted</title>\n </head>\n <body>";
			_responseBody += "<h1 style=\"text-align: center;\">+++ SERVER: " + _config.getServerName() + " +++</h1>\n";
			_responseBody += "<h3 style=\"text-align: center;\">Request served</h3>\n";
			if (_cgiResponse != "") {
				_responseBody += "<p> </p>\n";
				_responseBody += "<p style=\"text-align: center;\">CGI response: " + _cgiResponse + "</p>";
				_responseBody += "<p> </p>\n";
			}
			_responseBody += "<p style=\"text-align: center;\"><br /> <a href=\"/\"><button>Back</button></a></p>";
			_responseBody += "</body>\n";
			_responseBody += "</html>\n";
			_contentType = "text/html";
		}
	}
	buildHeader();
	setResponse();
	_done = true;
	std::cout << CYAN_TEXT << "Response header:  " << _responseHeader << RESET_COLOR << std::endl;
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
	if (_location.getLocation() == "null")
		_location = _config.getLocation("/");
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
	_responseBody += "<html>\n <head>\n <title>Index of " + _location.getLocation() + "</title>\n </head>\n <body>";
	_responseBody += "<h1 style=\"text-align: center;\">+++ SERVER: " + _config.getServerName() + " +++</h1>\n";
	_responseBody += "<h2 style=\"text-align: center;\">" + _location.getLocation() + "</h2>\n<p> </p>\n";
	_responseBody += "<h3>Directory listing:</h3>\n<p> </p>\n";
	_responseBody += "<ul>\n";
	while ((currDir = readdir(dir)) != NULL) {
		if (currDir->d_type == DT_REG) {
			std::string	filePath = std::string(currDir->d_name);
			_responseBody += "<li><a href=\"" + _location.getLocation() + "/" + filePath + "\">" + filePath + "</a></li>\n";
		}
	}
	_responseBody += "</ul>\n";
	_responseBody += "</body>\n";
	_responseBody += "</html>\n";
	_contentType = "text/html";
	setStatus("200 OK");
	closedir(dir);
}

void	Request::handleGetMethod(std::string &fileToOpen) {
	if (_rawParams != "" && _config.getServerName() == "cgi")
		parseParams();

	if (fileOrDirectory(fileToOpen)) {  // path is a directory
		if (!_location.getReturn().empty()) {  // redirection found
			std::map<int, std::string>	redirections = _location.getReturn();
			if (redirections.find(301) != redirections.end()) {
				_redirectionLocation = redirections[301];
				setStatus("301 Moved Permanently");
			}
		}
		else if (!_location.getIndex().empty()) {  // index file found
			const std::set<std::string>&	indexFiles = _location.getIndex();
			std::set<std::string>::const_iterator it = indexFiles.begin();
			while (it != indexFiles.end()) {
				const std::string&	currIndexFile = *it;
				if (!fileToOpen.empty() && fileToOpen[fileToOpen.size() - 1] != '/') {
					fileToOpen += '/';
				}
				fileToOpen += currIndexFile;
				if (access(fileToOpen.c_str(), F_OK) == 0) {
					setStatus("200 OK");
					break ;
				}
				it++;
			}
			if (it == indexFiles.end())
				setStatus("404 Not Found");
		}
		else if (_location.getDirectoryListing())
			generateAutoIndex(fileToOpen);
		else
			setStatus("404 Not Found");
	}
	else {  // path is a file
		if (fileType(_extension)) {
			if (_contentType.find("application/x") != std::string::npos) {
				std::string cgiPath;
				std::cout << "CGI file: " << _cgiFile << std::endl;
				if (_path.find(".php") != std::string::npos) {
					cgiPath = _location.getCgiPath("php");
					if (cgiPath != "") {
						_cgiResponse += solveCgi(cgiPath);
						setStatus("200 OK");
					} else
						setStatus("404 Not Found");
				}
				else if (_path.find(".py") != std::string::npos) {
					cgiPath = _location.getCgiPath("py");
					if (cgiPath != "") {
						_cgiResponse += solveCgi(cgiPath);
						setStatus("200 OK");
					}
					else
						setStatus("404 Not Found");
				}
				else
					setStatus("404 Not Found");
			}
			else
				setStatus("200 OK");
		}
	}
	_done = true;
}

void	Request::handlePostMethod(){
	_rawParams = _raw.substr(_raw.find("\r\n\r\n") + 4);
	if (_rawParams != "" && _config.getServerName() == "cgi")
		parseParams();

	if (!_location.isAcceptedMethod("POST"))
		setStatus("403 Forbiden");
	else if (_config.getMaxSize() < _contentLength)
		setStatus("413 Request Entity Too Large");
	else if (_raw.find("Content-Type: application/x-www-form-urlencoded") != std::string::npos) {  // Form found
		size_t	lastSlashPos = _path.find_last_of('/');
		std::string	_requestedFile = _path.substr(lastSlashPos + 1);
		std::string filePath;
		if (_requestedFile.find(".php") != std::string::npos) {
			 filePath = _location.getCgiPath("py");
			if (filePath != "") {
				_cgiResponse += solveCgi(filePath);
				setStatus("200 OK");
			} else {
				setStatus("404 Not Found");
			}
		}
		else if (_requestedFile.find(".py") != std::string::npos) {
			filePath = _location.getCgiPath("py");
			if (filePath != "") {
				_cgiResponse += solveCgi(filePath);
				setStatus("200 OK");
			}
			else{
				setStatus("404 Not Found");
			}
		}
		else if (_requestedFile.find("submit") != std::string::npos) {
			logParams();
			setStatus("200 OK");
		}
		else {
			setStatus("404 Not Found");
		}
		_done = true;
	}
	else {  // handle basic file
		const char *buf = _raw.c_str();
		parseBody(buf, _raw.size());
	}
}

void	Request::parseParams() {
	if (_rawParams != "") {
		std::vector <std::string> params = cppSplit(_rawParams, "&");
		for (size_t i = 0; i < params.size(); i++) {
			size_t eqPos = params[i].find('=');
			if (eqPos != std::string::npos) {
				std::string key = params[i].substr(0, eqPos);
				std::string value = params[i].substr(eqPos + 1);
				_pairsParams[key] = value;
			} else
				_singlesParams.push_back(params[i]);
		}
		logParams();
	}
}

void	Request::logParams() {
	std::string fileName("var/srv_" + _config.getServerName() + "/submit/submits.txt");
	std::string fileToUpload(_raw.begin() + _raw.find("name=") + 6, _raw.end());
	std::ofstream file(fileName.c_str(), std::ios::app);
	if (file.is_open()) {
		file << _rawParams << std::endl;
		file.close();
	}
	else
		std::cerr << "Error opening file for writing." << std::endl;
}

void	Request::handleDeleteMethod(std::string &fileToDelete){
	char	resolvedPath[PATH_MAX];
	struct stat fileInfo;

	strcpy(resolvedPath, fileToDelete.c_str());

	if (realpath(fileToDelete.c_str(), resolvedPath) == NULL) {
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
	_contentType = "text/plain";
	_done = true;
}

std::string	Request::solveCgi(std::string cgiPath) {
	std::string response = "";

	const char *args[] = {fileToOpen.c_str(), fileToOpen.c_str(), _rawParams.c_str(), NULL};

	int pipeFd[2];

	std::cout << "CGI path: " << cgiPath << std::endl;

	pipe(pipeFd);
	pid_t pid = fork();
	if (pid == 0) {
		close(pipeFd[0]);
		dup2(pipeFd[1], STDOUT_FILENO);
		close(pipeFd[1]);
		execve(cgiPath.c_str() , const_cast<char *const *>(args), NULL);
		std::cerr << "Error of execution." << std::endl;
		exit(1);
	}
	else {
		int status;
		waitpid(pid, &status, 0);
		close(pipeFd[1]);
		response.resize(4096);
		read(pipeFd[0], &response[0], 4096);
		close(pipeFd[0]);
		std::cout << "CGI response: " << response << std::endl;
	}
	return response;
}

std::string	Request::extractPathFromUrl(std::string& url) {
	size_t	firstSlashPos = url.find('/');
	size_t	secondSlashPos = url.find('/', firstSlashPos + 1);

	if (_location.getRootLocation())
		return (_servDrive + _location.getRoot() + url);
	else if (firstSlashPos != std::string::npos && secondSlashPos != std::string::npos) {
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
	if (_status != "" && _done && getResponse() == "") {
		buildResponse();
	}
}

bool	Request::fileExtension(const std::string& contentType) {
	std::map<std::string, std::string> contentTypeExtensions;
	contentTypeExtensions.insert(std::make_pair("image/icon", ".ico"));
	contentTypeExtensions.insert(std::make_pair("image/jpeg", ".jpeg"));
	contentTypeExtensions.insert(std::make_pair("image/jpg", ".jpg"));
	contentTypeExtensions.insert(std::make_pair("image/png", ".png"));
	contentTypeExtensions.insert(std::make_pair("image/gif", ".gif"));
	contentTypeExtensions.insert(std::make_pair("text/plain", ".txt"));
	contentTypeExtensions.insert(std::make_pair("plain/text", ".txt"));
	contentTypeExtensions.insert(std::make_pair("text/html", ".html"));
	contentTypeExtensions.insert(std::make_pair("text/css", ".css"));
	contentTypeExtensions.insert(std::make_pair("application/json", ".json"));
	contentTypeExtensions.insert(std::make_pair("application/pdf", ".pdf"));
	contentTypeExtensions.insert(std::make_pair("application/msword", ".doc"));
	contentTypeExtensions.insert(std::make_pair("application/javascript", ".js"));
	contentTypeExtensions.insert(std::make_pair("application/x-python-code", ".py"));
	contentTypeExtensions.insert(std::make_pair("application/x-php", ".php"));
	contentTypeExtensions.insert(std::make_pair("application/vnd.openxmlformats-officedocument.wordprocessingml.document", ".docx"));

	std::map<std::string, std::string>::iterator it = contentTypeExtensions.find(contentType);
	if (it != contentTypeExtensions.end()) {
		_extension = it->second;
		return true;
	} else {
		setStatus("500 Internal Server Error");
		return false;
	}
}

bool	Request::fileType(const std::string& extension) {
	std::map<std::string, std::string> extensionToContentType;
	extensionToContentType.insert(std::make_pair(".css", "text/css"));
	extensionToContentType.insert(std::make_pair(".ico", "image/x-icon"));
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
	extensionToContentType.insert(std::make_pair(".js", "application/javascript"));
	extensionToContentType.insert(std::make_pair(".py", "application/x-python-code"));
	extensionToContentType.insert(std::make_pair(".php", "application/x-php"));
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

void	Request::setExtension(const std::string &path) {
	size_t	ext = path.find_last_of('.');

	if (ext != std::string::npos) {
		_extension = path.substr(ext);
		fileType(_extension);
	}
	else {
		_extension = "";
		_contentType = "Unsupported content type";
	}
}

std::string Request::defaultErrorPage(std::string errorCode, std::string errorDescription) {
std::string page_html = "<!DOCTYPE html>"
						"<html lang=\"en\">"
						"<head>"
						"    <meta charset=\"UTF-8\">"
						"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
						"    <title>Error " + errorCode + "</title>"
						"    <style>"
						"        body {"
						"            font-family: Arial, sans-serif;"
						"            height: 100vh;"
						"            position: relative;"
						"            background-color: #f4f4f4;"
						"            margin: 0;"
						"            padding: 0;"
						"        }"
						"        .container {"
						"            position: absolute;"
						"            top: 50%;"
						"            left: 50%;"
						"            max-width: 800px;"
						"            margin: 0 auto;"
						"            padding: 40px 20px;"
						"            transform: translate(-50%, -50%);"
						"        }"
						"        h1 {"
						"            color: #333;"
						"        }"
						"        h2 {"
						"            color: #777;"
						"        }"
						"        .error-code {"
						"            font-size: 48px;"
						"            color: #ff5555;"
						"        }"
						"        .error-description {"
						"            font-size: 24px;"
						"        }"
						"    </style>"
						"</head>"
						"<body>"
						"    <div class=\"container\">"
						"        <h1>Error <span class=\"error-code\">" + errorCode + "</span></h1>"
						"        <h2 class=\"error-description\">" + errorDescription + "</h2>"
						"    </div>"
						"</body>"
						"</html>";
	return page_html;
}

bool	Request::handleError() {
	std::string	errCodeStr = _status.substr(0, 3);

	if (errCodeStr[0] != '3' && errCodeStr[0] != '4' && errCodeStr[0] != '5')
		return false;
	else {
		std::string	errorFileName;
		int	errCode = std::atoi(errCodeStr.c_str());
		if (_redirectionLocation != "")
			errorFileName = _servDrive + _errorLocation + _redirectionLocation;
		else if (_config.getErrorPage(errCode) != "" && !(_config.getErrorPage(errCode)).empty())
			errorFileName = _servDrive + _errorLocation + _config.getErrorPage(errCode);
		std::ifstream file(errorFileName.c_str());
		if (file.is_open()) {
			std::string	line;
			while (std::getline(file, line))
				_responseBody += line;
			file.close();
		}
		else {
			_responseBody += defaultErrorPage(errCodeStr, "Page Not Found");
		}
		_contentType = "text/html";
		return true;
	}
}

std::vector<std::string> Request::cppSplit(const std::string &str, std::string delimiter) {
	std::size_t pos;
	std::size_t last_pos = 0;
	std::vector<std::string> split_str;

	split_str.reserve(str.size() / delimiter.size() + 1);
	while ((pos = str.find(delimiter, last_pos)) != std::string::npos) {
		split_str.push_back(str.substr(last_pos, pos - last_pos));
		last_pos = pos + delimiter.size();
	}
	split_str.push_back(str.substr(last_pos));
	return (split_str);
}

size_t	Request::dechunkBody() {
	std::string	bodyString(_body.begin(), _body.end());
	std::vector<std::string>	lines = cppSplit(bodyString, "\r\n");
	std::string	dechunkedBody;

	for (size_t i = 1; i < lines.size() - 1; i++) {
		if (i % 2 == 0)
			dechunkedBody += lines[i];
	}
	_body.assign(dechunkedBody.begin(), dechunkedBody.end());
	return (_body.size() + 4);
}

void Request::printParamsCont() {
	if (!_pairsParams.empty()) {
		std::cout << "Params (pairs): " << std::endl;
		for (std::map<std::string, std::string>::iterator it = _pairsParams.begin(); it != _pairsParams.end(); it++)
			std::cout << it->first << ": " << it->second << "   ";
		std::cout << std::endl;
	}
	if (!_singlesParams.empty()) {
		std::cout << "Params (singles): " << std::endl;
		for (std::vector<std::string>::iterator it = _singlesParams.begin(); it != _singlesParams.end(); it++)
			std::cout << *it << "   ";
		std::cout << std::endl;																																			
	}
}

std::string Request::getPath() const { return _path; }

void	Request::setStatus(const std::string &status) {	_status = status; }

std::string Request::getMethod() const { return _method; }

std::string Request::getExtension() const {	return _extension; }

void	Request::setResponse() { _response = _responseHeader + _responseBody; }

std::string	Request::getResponse() const { return _response; }

bool	Request::isResponseReady() const { return _done; }
