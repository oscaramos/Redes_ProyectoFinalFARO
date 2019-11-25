#pragma once

#include "functions.hpp"

#define PF_INET 0
#define SOCK_STREAM 0
#define IPPROTO_TCP 0
#define SHUT_RDWR 0

struct sockets {
	int currfd = 0;
	vector<pair<string, int>> sockfds;	//sockfds[fd] <IP,PORT>
	vector<string> messages;			//messages
};

int socket(sockets& socks, int domain, int type, int protocol);
int accept(sockets& socks, int sockfd, string addr, int addrlen);
int read(sockets& socks, int fd, string& buf, int count);
int write(sockets& socks, int fd, string buf, int count);
void shutdown(sockets& socks, int sockfd, int how);
void close(sockets& socks, int fd);
void createServer(sockets& socks, int sockfd, string ip, int port);
void createClient(sockets& socks, int sockfd, string ip, int port);
