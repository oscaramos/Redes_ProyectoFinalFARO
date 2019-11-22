#ifndef SLAVECONNECTION_H
#define SLAVECONNECTION_H
#include <iostream>
#include "peer/Client.h"
using namespace std;

class SlaveConnection: public PeerConnection
{
public:
	bool onCreate;

public:
	SlaveConnection(int socketFD): PeerConnection(socketFD)
	{
	    this->onCreate = true;
	}

	virtual void sendPackagesHandler() override
	{
		if(onCreate)
		{
			onCreate = false;
			sender.sendStr("OK, me conecto contigo\n");
		}
	}

	void sendPack(string pack)
	{
		sender.sendStr(pack);
	}

	virtual void receivePackagesHandler() override
	{
		this->disableReceiver = true;
	}

	// Recuerda, no debe recibir mensajes entre '\n' 
	string receiveMessage()
	{
		string message;
		try{
			message = receiver.recvField(3);
		}catch(...){
			cout << "Recibi mensaje con formato invalido, cerrando conexion! " << endl;
			closeConnection();
			message = "Error interno en comunicaciones entre Master & Slaves";
		}
		return message;
	}
};

#endif