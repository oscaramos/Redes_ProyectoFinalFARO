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
#include "globals.h"
#include "auxPackCmd.h"
#include "DeviceConnection.h"


using namespace std;

//////////////////////////////MAIN////////////////////////////////
int main(int argc, const char** argv)
{
	int port;
	if(argc < 2)
	{
		printf("Parameters: ./master.exe <PORT_SERVER>\n");
		return 0;
	}
	if(argc >= 2)
	{
		port = atoi(argv[1]);
	}
	if(argc >= 3)
	{
		cout << "UNITARY TESTING" << endl;
		CreatorPack cpk;
		string pack = cpk.createPack(CMD_CREATE, "CREATE (\"C:\", \"size: 12, files: 2\")");
		DBG(pack);
	}

    Server<DeviceConnection> receiver;
    bool isServerOpened = receiver.newThread_turnOnServer(port);
    if(isServerOpened){
    	sleep(100000); // Con signal se desactiva	
    }
    else {
    	printf("El servidor del Master no pudo iniciar correctamente\n");
    }
    printf("Programa terminado\n");
    return 0;
}
