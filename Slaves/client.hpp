#pragma once

#include "functions.hpp"
#include "sockets.hpp"

class client {
private:
	string IP;
	int PORT;
public:
	sockets socks;
	client(string _IP, int _PORT, sockets& _socks);
};