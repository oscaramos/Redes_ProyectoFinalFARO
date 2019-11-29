#ifndef SLAVECONNECTION_H
#define SLAVECONNECTION_H
#include <iostream>
#include <queue>
#include "peer/Client.h"
#include "globals.h"
#include "auxPackCmd.h"
using namespace std;


class SlaveServerConnection: public PeerConnection
{
private:
	VerifierSlavePack verifierpack;
	CreatorSlavePack creatorpack;

public:
	SlaveServerConnection(int socketFD): PeerConnection(socketFD)
	{
	    ;
	}

	virtual void sendPackagesHandler() override
	{
		this->disableSender = true;
	}

	virtual void receivePackagesHandler() override
	{
		char firstchar = receiver.recvChar();
		receiver.ignore(1);
		if(firstchar == '\0'){
			printf("Conexion con slaves cerrada\n");
			closeConnection();
			return;
		}

		string pack;
		typeSlavePack typepack = verifierpack.getTypeOfPack(firstchar);
		switch(typepack)
		{
			case PCKSLAVE_EXIST: pack=processExist();
			case PCKSLAVE_ERROR: pack=processError();
		}
		sender.sendStr(pack);
	}

	// ejemplo: 1 03 C:/
	// estructura: [1, 2, var]
	string processExist();
	string processError();
};

string SlaveServerConnection::processExist()
{
	string pk = receiver.recvField(2);
	if(database.existNode(pk))
		return creatorpack.createPack(PCKSLAVE_TRUE);
	else 
		return creatorpack.createPack(PCKSLAVE_FALSE);
}

string SlaveServerConnection::processError()
{
	cout << "Error!!, paquete no reconocido" << endl;
	exit(0);
}


#endif