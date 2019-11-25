#pragma once

#include <unordered_map>
#include "functions.hpp"
#include "sockets.hpp"

class master {
private:
	string IP;
	int PORT;
	vector<pair<string, int>> slaves;
	unordered_map<string, int> pk_socket;
public:
	sockets socks;
	master(string _IP, int _PORT, sockets& _socks);
	void q_connect(string input);
	void q_start();
	void q_create(string input);
	void q_link(string input);
	void q_delete(string input);
	void q_unlink(string input);
	void q_update();
	void q_explore();
	void q_select();
	void parser(string query);
	void loadDB(string file_name);
	void recv_send(int sockfd);
	void test(sockets& socks); //DELETE THIS
};