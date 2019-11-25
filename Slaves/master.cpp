#include "master.hpp"

master::master(string _IP, int _PORT, sockets& _socks) {
	IP = _IP;
	PORT = _PORT;
	socks = _socks;
}

void master::test(sockets& socks) {
	int n = 0;
	slaves.push_back(make_pair("10.0.2.1", 5000));
	slaves.push_back(make_pair("10.0.2.2", 5000));
	slaves.push_back(make_pair("10.0.2.3", 5000));
	string query = "";
	//Create
	query = "1 01 A 006 A.data";
	write(socks, 3, query, query.size());
	query = "1 01 B 006 B.data";
	write(socks, 4, query, query.size());
	query = "1 01 C 006 C.data";
	write(socks, 5, query, query.size());
	query = "1 01 D 006 D.data";
	write(socks, 5, query, query.size());
	//UpdateDB
	query = "U 01 A 3";
	write(socks, 3, query, query.size());
	write(socks, 4, query, query.size());
	write(socks, 5, query, query.size());
	query = "U 01 B 4";
	write(socks, 3, query, query.size());
	write(socks, 4, query, query.size());
	write(socks, 5, query, query.size());
	query = "U 01 C 5";
	write(socks, 3, query, query.size());
	write(socks, 4, query, query.size());
	write(socks, 5, query, query.size());
	query = "U 01 D 5";
	write(socks, 3, query, query.size());
	write(socks, 4, query, query.size());
	write(socks, 5, query, query.size());
	//Links
	query = "2 01 A 01 B";
	write(socks, 3, query, query.size());
	query = "2 01 A 01 D";
	write(socks, 3, query, query.size());
	query = "2 01 B 01 C";
	write(socks, 4, query, query.size());
	query = "2 01 B 01 D";
	write(socks, 4, query, query.size());
	query = "2 01 C 01 B";
	write(socks, 5, query, query.size());
	query = "2 01 D 01 A";
	write(socks, 5, query, query.size());
}
