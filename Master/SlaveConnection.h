#ifndef SLAVECONNECTION_H
#define SLAVECONNECTION_H
#include <iostream>
#include <queue>
#include "peer/Client.h"
#include "globals.h"
using namespace std;

class SlaveConnection: public PeerConnection
{
private:
	bool onCreate;
	bool pingResponse;
	queue<string> queue_msgs;


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

	// Recuerda, no debe recibir mensajes entre '\n' 
	virtual void receivePackagesHandler() override
	{
		string message;
		try{
			message = receiver.recvField(3);
		}catch(...){
			cout << "Recibi mensaje con formato invalido, cerrando conexion! " << endl;
			closeConnection();
			queue_msgs.push("Error interno en comunicaciones entre Master & Slaves");
		}		
		if(isPingMsg(message))
		{
			cout << "-Recibi ping" << endl;
			pingResponse = true;
		}
		else
			queue_msgs.push(message);
	}

	void sendPack(string pack)
	{
		sender.sendStr(pack);
	}
	
	string receiveMessage()
	{
		string msg;
		while(queue_msgs.empty()){;}
		msg = queue_msgs.front(); queue_msgs.pop();
		return msg;
	}

	bool timeout()
	{
		return !pingResponse;
	}

	void setPingResponse(bool x)
	{
		pingResponse = x;
	}

	bool isPingMsg(string msg)
	{
		return msg[0]=='p';
	}

	void closeConn()
	{
		closeConnection();
	}
};

#endif