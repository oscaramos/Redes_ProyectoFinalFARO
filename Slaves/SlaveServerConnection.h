#ifndef SLAVESERVERCONNECTION_H
#define SLAVESERVERCONNECTION_H
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
		cout << "SlaveServerConnection: firstchar = " << firstchar << endl;
		if(firstchar == '\0'){
			printf("Conexion con slaves cerrada\n");
			closeConnection();
			return;
		}

		string pack;
		typeSlavePack typepack = verifierpack.getTypeOfPack(firstchar);
		switch(typepack)
		{
			case PCKSLAVE_EXIST: pack=processExist(); break;
			case PCKSLAVE_ERROR: pack=processError(); break;
		}
		cout << "SlaveServerConnection: envio paquete = ["<<pack<<"]" << endl;
		sender.sendStr(pack);
	}

	string processExist();
	string processError();
};

// ejemplo: 1 03 C:/
// estructura: [1, 2, var]
string SlaveServerConnection::processExist()
{
	cout << "processExist: Inicia" << endl;
	receiver.ignore(1);
	string pk = receiver.recvField(2);
	cout << "processExist: PK = " << pk << endl;
	if(database.existNode(pk))
		return creatorpack.createPack(PCKSLAVE_TRUE);
	else 
		return creatorpack.createPack(PCKSLAVE_FALSE);
}

string SlaveServerConnection::processError()
{
	cout << "SlaveServerConnection: error paquete no reconocido" << endl;
	exit(0);
}


#endif