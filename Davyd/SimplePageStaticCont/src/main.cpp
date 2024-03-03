#include "header.hpp"

const int PORT = 8080;

std::map<std::string, std::string> web_pages;

void loadWebPages() {
    web_pages["/"] = "index.html";
    web_pages["/about"] = "about.html";
    web_pages["/main.css"] = "main.css";
    web_pages["/script.js"] = "script.js";
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
    response << "Content-Type: " << content_type << "\r\n\r\n";


    char buffer[1024];
    while (file.read(buffer, sizeof(buffer)).gcount() > 0) {
        response.write(buffer, file.gcount());
    }

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

    std::cout << "Server is running on port " << PORT << std::endl;

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

        if (method == "GET") {
            std::string page_path = (web_pages.find(page) != web_pages.end()) ? web_pages[page] : "error.html";
            servePage(client_socket, page_path);
        } else {
            std::string not_found_response = "HTTP/1.1 404 Not Found\r\n\r\n";
            send(client_socket, not_found_response.c_str(), not_found_response.size(), 0);
        }
        close(client_socket);
    }

    close(server_socket);
    return 0;
}
