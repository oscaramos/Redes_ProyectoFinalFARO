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
#include "MasterConnection.h"
#include "SlaveServerConnection.h"
#include "SlaveClientConnection.h"


using namespace std;

//////////////////////////////MAIN////////////////////////////////
int main(int argc, const char** argv)
{
	if(argc < 3)
	{
		printf("Parameters: ./slave.exe <PORT_SRVMASTER> <PORT_SRVSLAVES> [<SLAVE_ID>]\n");
		return 0;
	}
	if(argc >=3)
	{
		port_master = atoi(argv[1]);
		port_slave = atoi(argv[2]);
	}
	if(argc >= 4)
	{
		slaveid = atoi(argv[3]);
		cout << "Fase de debug" << endl;
	}

    Server<MasterConnection> masterconn;
    Server<SlaveServerConnection> slaveconn;
    bool isopen_master = masterconn.newThread_turnOnServer(port_master);
    bool isopen_slave = slaveconn.newThread_turnOnServer(port_slave+slaveid); // slaveid es de debug
    if(isopen_master && isopen_slave){
    	sleep(100000); // Con signal se desactiva	
    }
    else {
	    if(!isopen_master) printf("El servidor para el Master no pudo iniciar\n");
	    if(!isopen_slave)  printf("El servidor para los Slave no pudo iniciar\n");
    }
    printf("Programa terminado\n");
    return 0;
}
