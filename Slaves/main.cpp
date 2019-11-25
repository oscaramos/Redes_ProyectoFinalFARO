#include "master.hpp"
#include "client.hpp"
#include "slave.hpp"

int main() {
	sockets socks;
	int currfd = 0;
	vector<pair<string, int>> sockets;	//sockets[fd] <IP,PORT>
	vector<string> messages;			//messages
	string MasterIP = "";
	int MasterPORT = 0;
	cout << "Insert MasterIP: " << endl;
	cin >> MasterIP;
	cout << "Insert MasterPORT: " << endl;
	cin >> MasterPORT;
	/*MASTER*/
	cout << "MASTER" << endl;
	master m1("10.0.1.1", 5000, socks);
	int sockfd = socket(socks, PF_INET, SOCK_STREAM, IPPROTO_TCP);
	createServer(socks, sockfd, MasterIP, MasterPORT);
	int connfd = accept(socks, sockfd, "NULL", 0);
	/*CLIENTS*/
	cout << "CLIENTS" << endl;
	client c1("10.0.0.1", 5000, socks);
	int sockcfd1 = socket(socks, PF_INET, SOCK_STREAM, IPPROTO_TCP);
	createClient(socks, sockcfd1, MasterIP, MasterPORT);
	client c2("10.0.0.2", 5000, socks);
	int sockcfd2 = socket(socks, PF_INET, SOCK_STREAM, IPPROTO_TCP);
	createClient(socks, sockcfd2, MasterIP, MasterPORT);
	/*SLAVES*/
	cout << "SLAVES" << endl;
	slave s1("10.0.2.1", 5000, socks);
	int socksfd1 = socket(socks, PF_INET, SOCK_STREAM, IPPROTO_TCP);
	createClient(socks, socksfd1, MasterIP, MasterPORT);
	slave s2("10.0.2.2", 5000, socks);
	int socksfd2 = socket(socks, PF_INET, SOCK_STREAM, IPPROTO_TCP);
	createClient(socks, socksfd2, MasterIP, MasterPORT);
	slave s3("10.0.2.3", 5000, socks);
	int socksfd3 = socket(socks, PF_INET, SOCK_STREAM, IPPROTO_TCP);
	createClient(socks, socksfd3, MasterIP, MasterPORT);

	//TEST
	cout << "TEST" << endl;
	m1.test(socks);
	string query = "6 01 A 02";
	write(socks, 3, query, query.size());
	string res = "";
	s1.recv_send(sockfd, 0, socks);
	read(socks, 0, res, res.size());
	cout << "RES: " << res << endl;
}