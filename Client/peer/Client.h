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

public:
	Client() = default;

	void newThread_connectToServer(std::string servIp, int servPort)
	{
		thread th(&Client::connectToServer, this, servIp, servPort);
		th.detach();
	}

	void connectToServer(std::string servIp, int servPort)
	{
		try{
			// Crear socket
			int socketFD = Safe::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			struct sockaddr_in servAddr = constructServerAddressStructure(servIp, servPort);

			// Establecer conexion a server
			Safe::connect(socketFD, (struct sockaddr *)&servAddr, sizeof(struct sockaddr_in));
			printf("Cliente: conectado a servidor %s | puerto %d\n", servIp.c_str(), servPort);

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
		catch (std::string msg_error)
		{
			fprintf(stderr, "Error en Client.h\n");
			perror(msg_error.c_str());
			exit(EXIT_FAILURE);
		}
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

	T* getInstanceOfPeerConnection()
	{
		return peerConn;
	}
};