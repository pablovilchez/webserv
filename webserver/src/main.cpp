#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "Request.hpp"
#include "Response.hpp"

#define PORT 8080
#define MAX_CONNECTIONS 3
#define MAX_BUFFER_SIZE 1024


std::string readFile(const char *filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Error: Unable to open file: " << filename << std::endl;
        return "";
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}


int main() {
    int server_fd, new_socket;
    sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    char buffer[MAX_BUFFER_SIZE];

    // Creating socket fd
    if((server_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Allow reuse of the adress
    int opt = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(&opt)) == -1)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }


    address.sin_family = PF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;
    // Binding socket to port
    if(bind(server_fd, (sockaddr *)&address, addrlen) == -1)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listening
    if(listen(server_fd, MAX_CONNECTIONS) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while(true)
    {
        // Accepting incoming connections
        if((new_socket = accept(server_fd, (sockaddr *)&address, &addrlen)) == -1)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Reading http request
        memset(buffer, 0, MAX_BUFFER_SIZE);
        recv(new_socket, buffer, MAX_BUFFER_SIZE, 0);

        // Parse request
        Request req(buffer);
        req.printData();

        // Sending http response
        Response res(req);
        
        std::string htmlContent;
        std::string response;
        if (req.getPath() == "/")
        {
            std::string filePath = "../web/library.html";
            htmlContent = readFile(filePath.c_str());
            response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n" + htmlContent;
        }
        else

        send(new_socket, response.c_str(), response.length(), 0);

        close(new_socket);
    }
    return 0;
}
