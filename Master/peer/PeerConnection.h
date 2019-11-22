#ifndef PEERCONNECTION_H 
#define PEERCONNECTION_H
#include <iostream>
#include <string>
#include "ReceiverSocket.h"
#include "SenderSocket.h"

class PeerConnection
{
protected:
	SenderSocket sender;
	ReceiverSocket receiver;
	bool isActiveConnection = true;
	bool disableSender = false;
	bool disableReceiver = false;

public:
	PeerConnection(int socketFD)
	: sender(socketFD), receiver(socketFD)
	{
	    ;
	}

	void sendPackages()
	{
		try{
		    do{
		    	sendPackagesHandler();
		    }while(isActiveConnection && disableSender==false);
		}
		catch(std::string msg_error)
		{
			perror(msg_error.c_str());
			exit(EXIT_FAILURE);
		}
	}

	void receivePackages()
	{
		try
		{
		    do{
		    	receivePackagesHandler();
		    }while(isActiveConnection && disableReceiver==false);
		}
		catch(std::string msg_error)
		{
			perror(msg_error.c_str());
			exit(EXIT_FAILURE);
		}
	}

	// Sobreescribir los virtuals en clases derivadas de PeerConnection
	virtual void sendPackagesHandler()
	{
		printf("Este es un ejemplo de uso de PeerConnection\n");
		printf("Que mensaje quiere enviar?\n>");
		std::string pack; getline(std::cin, pack);
		sender.sendStr(pack);
	}

	virtual void receivePackagesHandler()
	{
		std::string pack = receiver.recvStr(1000);
		printf("Mensaje recibido:\n%s\n", pack.c_str());
		if(pack.size() == 0)
			closeConnection();
	}

	void closeConnection()
	{
		this->isActiveConnection = false;
	}
};

#endif