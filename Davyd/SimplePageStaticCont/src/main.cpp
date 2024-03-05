#include "header.hpp"

const int PORT = 8080;

std::map<std::string, std::string> web_pages;

void loadWebPages() {
    web_pages["/"] = "index.html";
    web_pages["/about"] = "about.html";
    web_pages["/main.css"] = "main.css";
    web_pages["/script.js"] = "script.js";
    web_pages["/download.jpeg"] = "download.jpeg";
}

void writeErrorPage(const std::string& content) {
    std::ofstream file("error.html");
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open error.html for writing." << std::endl;
        return;
    }

    file << content;
    file.close();
}

// Function to serve a web page.
void servePage(int client_socket, const std::string& page_path) {
    std::ifstream file(page_path, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Unable to open " << page_path << std::endl;
        std::string not_found_response = "HTTP/1.1 404 Not Found\r\n\r\n";
        send(client_socket, not_found_response.c_str(), not_found_response.size(), 0);
        return;
    }

    std::stringstream response;
    std::string content_type;

    if (page_path.find(".html") != std::string::npos) {
        content_type = "text/html";
    } else if (page_path.find(".css") != std::string::npos) {
        content_type = "text/css";
    } else if (page_path.find(".js") != std::string::npos) {
        content_type = "application/javascript";
    } else {
        content_type = "text/plain";
    }

    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: " << content_type << "\r\n";
    response << "Close: Connection\r\n\r\n";

    char buffer[1024];
    while (file.read(buffer, sizeof(buffer)).gcount() > 0) {
        response.write(buffer, file.gcount());
    }

    std::cout << "Response:__________ \n" << response.str() << std::endl;
    send(client_socket, response.str().c_str(), response.str().size(), 0);
}

int main() {
    int server_socket;
    int client_socket;
    sockaddr_in server_addr;
    sockaddr_in client_addr;
    int yes = 1;

    socklen_t client_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Unable to create socket.");
        _exit(1);
    }

    if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt(SO_REUSEADDR) failed");
        _exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));


    // Bind the server socket.
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Unable to bind.");
        _exit(1);
    }

    // Listen for incoming connections.
    if (listen(server_socket, 5) < 0) {
        perror("Unable to listen.");
        _exit(1);
    }

    loadWebPages();

	char hostName[1024];
    gethostname(hostName, 1024);
    std::cout << "Server started on " << hostName << std::endl;

	hostent* host = gethostbyname(hostName);
	if (host == NULL) {
		herror("gethostbyname");
		exit(1);
	}

	std::cout << "IP Address: " << inet_ntoa(*(in_addr*)*host->h_addr_list) << std::endl;

    while (true) {
        // Accept a client connection.
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("Unable to accept connection");
            _exit(1);
        }

        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        if(recv(client_socket, buffer, sizeof(buffer), 0) == -1) {
            perror("Unable to read from socket");
            _exit(1);
        }

        std::istringstream request(buffer);
        std::string request_line;
        getline(request, request_line);

        std::istringstream iss(request_line);
        std::string method;
        std::string page;
        iss >> method >> page;

        std::string errorTitle = "ERROR 404";
        std::string errorText = "Page not found:()";

        std::string error_html_content = "<!DOCTYPE html>\n"
                                        "<html lang=\"en\">\n"
                                        "<head>\n"
                                        "    <meta charset=\"UTF-8\">\n"
                                        "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                                        "    <title>Error Page</title>\n"
                                        "    <link rel=\"stylesheet\" href=\"./main.css\">\n"
                                        "</head>\n"
                                        "<body>\n"
                                        "    <h1>" + errorTitle + "</h1>\n"
                                        "    <h2>" + errorText + "</h2>\n"
                                        "    <a href=\"/\">Back</a>\n"
                                        "</body>\n"
                                        "</html>\n";


        writeErrorPage(error_html_content);

        std::cout << "Request:___________________\n"<< buffer << std::endl;
        if (method == "GET") {
            std::string page_path = (web_pages.find(page) != web_pages.end()) ? web_pages[page] : "error.html";
            servePage(client_socket, page_path);
        }
        else if (method == "POST") {
            std::string response = "HTTP/1.1 200 OK\r\n"; // Response line
            response += "Content-Type: text/plain\r\n"; // Headers
            response += "\r\n"; // An empty line indicates the end of the headers
            response += "POST request received successfully!";
            send(client_socket, response.c_str(), response.size(), 0);
        }
        else {
            std::string not_found_response = "HTTP/1.1 404 Not Found\r\n\r\n";
            send(client_socket, not_found_response.c_str(), not_found_response.size(), 0);
        }
        close(client_socket);
    }

    close(server_socket);
    return 0;
}
