#ifndef CLIENT_H
#define CLIENT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <thread>
#include "../aux/Safe.h"
#include "PeerConnection.h" 

template<typename T>
class Client
{
	static_assert(std::is_base_of<PeerConnection, T>::value, "T debe ser derivado de PeerConnection");
private:
	T* peerConn;
	int socketFD;
	std::string ip;
	int port;

public:
	Client() = default;

	bool newThread_connectToServer(std::string servIp, int servPort)
	{
		bool connectionOk = connectToServer(servIp, servPort);
		if(connectionOk){
			this->ip = servIp;
			this->port = servPort;
			std::thread th(&Client::handlePeerConnection, this, this->socketFD);
			th.detach();
			return true;
		}
		else 
			return false;
	}

	T* getInstanceOfPeerConnection()
	{
		return peerConn;
	}

	std::string getIp()
	{
		return this->ip;
	}

	int getPort()
	{
		return this->port;
	}

private:
	bool connectToServer(std::string servIp, int servPort)
	{
		try{
			// Crear socket
			socketFD = Safe::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			struct sockaddr_in servAddr = constructServerAddressStructure(servIp, servPort);

			// Establecer conexion a server
			Safe::connect(socketFD, (struct sockaddr *)&servAddr, sizeof(struct sockaddr_in));
			printf("Cliente: conectado a servidor (%s:%d)\n", servIp.c_str(), servPort);
			return true;
		}
		catch (std::string msg_error)
		{
			fprintf(stderr, "Error en Client.h\n");
			perror(msg_error.c_str());
			return false;
		}
	}

	void handlePeerConnection(int socketFD)
	{
		// Comunicarse con server
		peerConn = new T(socketFD);
		std::thread t1(&T::sendPackages, peerConn);
		std::thread t2(&T::receivePackages, peerConn);
		t1.join();
		t2.join();	

		// Cerrar conexion con server
		shutdown(socketFD, SHUT_RDWR);
		close(socketFD);
		printf("Cliente: Conexion con servidor finalizada\n");
	}

	struct sockaddr_in constructServerAddressStructure(std::string servIp, int servPort)
	{
		struct sockaddr_in servAddr;
		memset(&servAddr, 0, sizeof(struct sockaddr_in));
		servAddr.sin_family = AF_INET;
		servAddr.sin_port = htons(servPort);
		Safe::inet_pton(AF_INET, servIp.c_str(), &servAddr.sin_addr);
		return servAddr;
	}



};

#endif