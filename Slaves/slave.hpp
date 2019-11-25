#pragma once

#include <unordered_map>
#include "functions.hpp"
#include "sockets.hpp"

struct node {
	string pk;
	string content;
	vector<string> links;
	node(string _pk, string _content);
	int findLink(string pk);
};

class slave {
private:
	string IP;
	int PORT;
	vector<pair<string, int>> devices;//?
	//unordered_map<string, pair<string,int>> pk_socket;
	unordered_map<string, int> pk_socket;
	vector<node> nodes;
	vector<string> v_explore;
	vector<string> v_select;
public:
	sockets socks;
	slave(string _IP, int _PORT, sockets& _socks);
	string getIP();
	int getPORT();
	void setDevices(vector<pair<string, int>> nDevices);
	void updateDB(string packet);
	int findNode(string pk);
	string q_create(string input);
	string q_link(string input);
	string q_delete(string input);
	string q_unlink(string input);
	string q_update(string input);
	string q_explore(string input);
	string q_select(string input);
	string parser(string packet);
	void recv_send(int sockfd, int fd, sockets& _socks);
};

