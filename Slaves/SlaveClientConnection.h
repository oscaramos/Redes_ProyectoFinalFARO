#ifndef SLAVECLIENTCONNECTION_H
#define SLAVECLIENTCONNECTION_H
#include <iostream>
#include <queue>
#include "peer/Client.h"
using namespace std;

class SlaveClientConnection: public PeerConnection
{
public:
	SlaveClientConnection(int socketFD): PeerConnection(socketFD)
	{
	    ;
	}

	virtual void sendPackagesHandler() override
	{
		;
	}

	virtual void receivePackagesHandler() override
	{
		this->disableReceiver = true;
	}

	void sendPack(string pack)
	{
		sender.sendStr(pack);
	}

	// ejemplo: T
	// estructura: [cmd]
	// longitudes: [1]
	bool receivePackTrueOrFalse()
	{
		char firstchar = receiver.recvChar();
		cout << "SlaveClientConnection(T|F): firstchar = " << firstchar << endl;
		if(firstchar == '\0'){
			printf("Conexion con slaves cerrada\n");
			closeConnection();
			return false;
		}
		if(firstchar == 'T') return true;
		else return false;
	}

	// ejemplo: 4 01 00 03 C:/
	// estructura: [cmd, len_list, [prof, len_pk, pk]]
	// longitudes: [1, 2 [2, 2 var]+]
	vector<pair<int, string>> receivePackExplore()
	{
		vector<pair<int , string>> response;
		char firstchar = receiver.recvChar();
		cout << "SlaveClientConnection(Explore): firstchar = " << firstchar << endl;
		receiver.ignore(1); int len_list = receiver.recvInt(2);
		for(int i=0; i<len_list; ++i){
			receiver.ignore(1); int prof = receiver.recvInt(2);
			receiver.ignore(1);	string pk = receiver.recvField(2);
			response.push_back(make_pair(prof, pk));
		}
		cout << "SlaveClientConnection(Explore): son " << response.size() << endl;
		return response;
	}

	// es como el explore, pero + contenido
	// ejemplo: 5 01 00 03 C:/ 24 (files: 2, size: 1044MB)
	// estructura: [cmd, len_list, [prof, len_pk, pk, len_content, content]]
	// longitudes: [1, 2 [2, 2 var, 3 var]+]
	vector<pair<int, pair<string, string>>> receivePackSelect()
	{
		vector<pair<int, pair<string, string>>> response;
		char firstchar = receiver.recvChar();
		cout << "SlaveClientConnection(Explore): firstchar = " << firstchar << endl;
		receiver.ignore(1); int len_list = receiver.recvInt(2);
		for(int i=0; i<len_list; ++i){
			receiver.ignore(1); int prof = receiver.recvInt(2);
			receiver.ignore(1);	string pk = receiver.recvField(2);
			receiver.ignore(1);	string content = receiver.recvField(3);
			response.push_back(make_pair(prof, make_pair(pk, content)));
		}
		cout << "SlaveClientConnection(Explore): son " << response.size() << endl;
		return response;
	}
};

#endif