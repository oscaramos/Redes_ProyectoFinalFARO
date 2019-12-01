#ifndef SLAVECLIENTCONNECTION_H
#define SLAVECLIENTCONNECTION_H
#include <iostream>
#include <queue>
#include "../peer/Client.h"
#include "../Pack/Unpackager.h"
using namespace std;

class SlaveClientConnection: public PeerConnection
{
public:
	Unpackager unpackager;

public:
	SlaveClientConnection(int socketFD): PeerConnection(socketFD), unpackager(socketFD)
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
		char packid = receiver.recvChar();
		cout << "SlaveClientConnection(T|F): packid = " << packid << endl;
		if(packid == 'T') return true;
		else return false;
	}

	// ejemplo: 4 01 00 03 C:/
	// estructura: [cmd, len_list, [prof, len_pk, pk]]
	// longitudes: [1, 2 [2, 2 var]+]
	vector<pair<int, string>> receivePackExplore()
	{
		receiver.ignore(1);
		vector<pair<int , string>> response = unpackager.unpackageExploreResponse();
		cout << "SlaveClientConnection(Explore): son " << response.size() << endl;
		return response;
	}

	// es como el explore, pero + contenido
	// ejemplo: 5 01 00 03 C:/ 24 (files: 2, size: 1044MB)
	// estructura: [cmd, len_list, [prof, len_pk, pk, len_content, content]]
	// longitudes: [1, 2 [2, 2 var, 3 var]+]
	vector<pair<int, pair<string, string>>> receivePackSelect()
	{
		receiver.ignore(1);
		vector<pair<int, pair<string, string>>> response = unpackager.unpackageSelectResponse();
		cout << "SlaveClientConnection(Select): son " << response.size() << endl;
		return response;
	}
};

#endif