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
#define DBG(x) cout << #x << " = " << x << endl;
#define DBGVEC(v) cout << #v << " = "; for(const auto&vv: v) cout << vv << ", "; cout << endl;
#define CLIENT_PORT 1111
using namespace std;

///////////////////////////////////FILE/////////////////////////////////////////////////////

struct File
{
	char name[100];
	int size;
	char bin[1000000];
};

void saveFile(File file)
{
	ofstream ofs(file.name, ios::binary);
	if(!ofs.is_open()) throw "Archivo no abierto";
	for(int i=0; i<file.size; ++i)
		ofs.put(file.bin[i]);
	ofs.close();
}

File readFile(string filename)
{
	string filebin, tmp;
	ifstream ifs(filename.c_str(), ios::binary);
	if(!ifs) throw "Error al abrir " + filename;
	while(!ifs.eof())
	{
		char c;
		ifs.read(&c, 1);
		filebin += c;
	}
	ifs.close();

	File ans; 
	memset(&ans, 0, sizeof(File));
	memcpy(ans.name, filename.c_str(), filename.size());
	memcpy(ans.bin, filebin.c_str(), filebin.size());
	ans.size = filebin.size();
	return ans;
}

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
		if(onCreate)
		{
			onCreate = false;
			string port = to_string(CLIENT_PORT);
			string pack = join({"1", packageField(port, 2)}); 
			sender.sendStr(pack);
		}
		else
		{
			string cmd;
			cout.put('>'); getline(cin, cmd);
			vector<string> tokens = tokenizeCommand(cmd);

			if(tokens[0] == "GET")
			{
				string filename = tokens[1];
				string pack = join({"2", packageField(filename, 3)});
				sender.sendStr(pack);
			}
			else if(tokens[0] == "PUT")
			{
				File file = readFile(tokens[1]);
				sender.sendStr("3 ");
				sender.sendStruct<File>(file);
			}
			else if(tokens[0] == "CLOSE")
			{
				sender.sendStr("4");
				closeConnection();
				return;
			}
		}
	}

	virtual void receivePackagesHandler() override
	{
		this->disableReceiver = true;
	}
};

class ServerConnection: public PeerConnection
{
public:
	ServerConnection(int socketFD): PeerConnection(socketFD)
	{
	    ;
	}

	virtual void sendPackagesHandler() override
	{
		this->disableSender = true;
	}

	virtual void receivePackagesHandler() override
	{
		File file = receiver.recvStruct<File>();
		if(strlen(file.name) == 0)
		{
			printf("Cerraron!\n");
			closeConnection();
			return;
		}
		setPrefixFilename("GET_", file);
		saveFile(file);
	}

	void setPrefixFilename(string prefix, File& file)
	{
		char newfilename[100];
		memset(newfilename, 0, 100);
		strcat(newfilename, prefix.c_str());
		strcat(newfilename, file.name);
		strcpy(file.name, newfilename);
	}
};

// TODO: al cerrar conexion, mostrar mensaje "Cerre conexion con X cliente con ip:..."


//////////////////////////////MAIN////////////////////////////////
int main(int argc, const char** argv)
{
	SimulatorUserInput sui;
	string ip;
	int port;
	if(argc < 3)
	{
		printf("Parameters: ./client.exe <IP_SERVER> <PORT_SERVER>\n");
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

    Server<ServerConnection> receiver;
    receiver.newThread_turnOnServer(CLIENT_PORT);

    sleep(100000); // Con signal se desactiva
    printf("Programa terminado\n");
    return 0;
}
