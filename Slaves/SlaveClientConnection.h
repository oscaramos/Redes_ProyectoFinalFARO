#ifndef SLAVECLIENTCONNECTION_H
#define SLAVECLIENTCONNECTION_H
#include <iostream>
#include <queue>
#include "peer/Client.h"
#include "globals.h"
#include "auxPackCmd.h"
using namespace std;

class SlaveClientConnection: public PeerConnection
{
public:
	VerifierSlavePack verifierpack;

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

	bool receivePackTrueOrFalse()
	{
		char firstchar = receiver.recvChar();
		cout << "SlaveClientConnection(T|F): firstchar = " << firstchar << endl;
		if(firstchar == '\0'){
			printf("Conexion con slaves cerrada\n");
			closeConnection();
			return false;
		}

		string pack;
		typeSlavePack typepack = verifierpack.getTypeOfPack(firstchar);
		if(typepack==PCKSLAVE_TRUE) return true;
		else                        return false;
	}
};

#endif