#include "client.hpp"

client::client(string _IP, int _PORT, sockets& _socks) {
	IP = _IP;
	PORT = _PORT;
	socks = _socks;
}