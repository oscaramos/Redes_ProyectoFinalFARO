// Autor: Oscar Daniel Ramos Ramirez
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <termios.h>
#include <chrono>
#include <fstream>
#include "peer/Client.h"
#include "peer/Server.h"
#include "aux/SimulatorUserInput.h"
#include "aux/stringhelper.h"
using namespace std;


///////////////////////////////////CONEXION CON SERVER/////////////////////////////////
class ClientConnection: public PeerConnection
{
public:
	bool onCreate;

public:
	ClientConnection(int socketFD): PeerConnection(socketFD)
	{
	    this->onCreate = true;
	}

	virtual void sendPackagesHandler() override
	{
		cout.put('>');
		string cmd; getline(cin, cmd);
		cmd = cmd + '\n';
		sender.sendStr(cmd);
	}

	virtual void receivePackagesHandler() override
	{
		string message = receiver.recvStr(1000);
		if(message.empty())
		{
			printf("Cerraron!\n");
			closeConnection();
		}
		else 
		{
			cout << "Mensaje recibido: " << message << endl;			
		}
	}
};
//////////////////////////////MAIN////////////////////////////////
int main(int argc, const char** argv)
{
	SimulatorUserInput sui;
	string ip;
	int port;
	if(argc < 3)
	{
		printf("Usage: ./client.exe <IP_SERVER> <PORT_SERVER>\n");
		return 0;
	}
	if(argc >= 3) 
	{
		ip = argv[1];
		port = atoi(argv[2]);
	}
	if(argc >= 4) 
	{
		printf("TESTING WITH USER INPUT:\n%s\n", argv[3]);
		sui.simulate(argv[3]);
	}

	Client<ClientConnection> sender; 
	sender.newThread_connectToServer(ip, port);

    sleep(100000); // Con signal se cancela el sleep
    printf("Programa terminado\n");
    return 0;
}
