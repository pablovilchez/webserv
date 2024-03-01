#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

const int MAX_CLIENTS = 10;

int main() {
    // Create a socket
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1) {
        std::cerr << "Can't create a socket!" << std::endl;
        return -1;
    }

    // Bind the socket to an IP/port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000); // Little Endian to Big Endian
    hint.sin_addr.s_addr = INADDR_ANY; // Bind to any address

    if (bind(listening, reinterpret_cast<sockaddr*>(&hint), sizeof(hint)) == -1) {
        std::cerr << "Can't bind to IP/port" << std::endl;
        return -2;
    }

    // Mark the socket for listening in
    if (listen(listening, MAX_CLIENTS) == -1) {
        std::cerr << "Can't listen!" << std::endl;
        return -3;
    }

    std::vector<int> clients(MAX_CLIENTS, 0);

    while (true) {
        fd_set master;
        FD_ZERO(&master);
        FD_SET(listening, &master);

        int max_sd = listening;

        for (size_t i = 0; i < clients.size(); ++i) {
            int sd = clients[i];

            if (sd > 0) {
                FD_SET(sd, &master);
            }

            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        // Wait for activity on any socket
        int activity = select(max_sd + 1, &master, NULL, NULL, NULL);

        if (activity < 0 && errno != EINTR) {
            std::cerr << "Select error" << std::endl;
        }

        if (FD_ISSET(listening, &master)) {
            sockaddr_in client;
            socklen_t clientSize = sizeof(client);
            int clientSocket = accept(listening, reinterpret_cast<sockaddr*>(&client), &clientSize);

            if (clientSocket == -1) {
                std::cerr << "Problem with client connection" << std::endl;
            } else {
                for (size_t i = 0; i < clients.size(); ++i) {
                    if (clients[i] == 0) {
                        clients[i] = clientSocket;
                        break;
                    }
                }
            }
        }

        for (size_t i = 0; i < clients.size(); ++i) {
            int sd = clients[i];
            if (FD_ISSET(sd, &master)) {
                char buffer[4096];
                memset(buffer, 0, sizeof(buffer));

                int bytesRecv = recv(sd, buffer, sizeof(buffer), 0);
                if (bytesRecv <= 0) {
                    close(sd);
                    clients[i] = 0;
                } else {
					std::cout << "Received (" << i << "): ";
					std::cout << std::string(buffer, 0, bytesRecv);
					for (size_t j = 0; j < clients.size(); j++)
					{
						if (j != i)
						{
							send(clients[j], "other: ", 7, 0);
                    		send(clients[j], buffer, bytesRecv, 0);
						}
					}
                }
            }
        }
    }

    // Clean up
    close(listening);

    return 0;
}
