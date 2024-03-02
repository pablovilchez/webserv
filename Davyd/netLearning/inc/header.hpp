#ifndef HEADER_HPP
#define HEADER_HPP

#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <csignal>
#include <poll.h>
#include <signal.h>
#include <sys/wait.h>

extern int listening;

void soloClient();
void multyClient();

#endif
