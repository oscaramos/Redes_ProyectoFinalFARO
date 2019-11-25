#include "sockets.hpp"

int socket(sockets& socks, int domain, int type, int protocol) {
	int sockfd = socks.currfd;
	socks.sockfds.push_back(make_pair("IP", 1));
	socks.messages.push_back("Connect");
	socks.currfd += 1;
	return sockfd;
}

int accept(sockets& socks, int sockfd, string addr, int addrlen) {
	return sockfd;
}

int read(sockets& socks, int fd, string& buf, int count) {
	buf = socks.messages[fd];
	return 0;
}

int write(sockets& socks, int fd, string buf, int count) {
	socks.messages[fd] = buf;
	return 0;
}

void shutdown(sockets& socks, int sockfd, int how) {
	socks.sockfds[sockfd] = make_pair("NULL", 1);
}

void close(sockets& socks, int fd) {
	socks.sockfds[fd] = make_pair("NULL", 0);
}

void createServer(sockets& socks, int sockfd, string ip, int port) {
	socks.sockfds[sockfd] = make_pair(ip,port);
}

void createClient(sockets& socks, int sockfd, string ip, int port) {
	socks.sockfds[sockfd] = make_pair(ip, port);
}
