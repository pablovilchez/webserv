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
#include <fstream>
#include <sstream>

//#include "Response.hpp"
//int soloClient(int listening);
int multyClient(int listening);

#endif
