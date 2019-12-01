// Disenatior, codificador, tester: Oscar Daniel Ramos Ramirez
// UCSP 2019-2
#include <stdio.h>
#include <iostream>
#include "peer/Client.h"
#include "peer/Server.h"
#include "Connections/MasterConnection.h"
#include "Connections/SlaveServerConnection.h"


using namespace std;

void argvHandler(int argc, const char** argv);

//////////////////////////////MAIN////////////////////////////////
int main(int argc, const char** argv)
{
	argvHandler(argc, argv);
    Server<MasterConnection> masterconn;
    Server<SlaveServerConnection> slaveconn;
    bool isopen_master = masterconn.newThread_turnOnServer(port_master);
    bool isopen_slave = slaveconn.newThread_turnOnServer(port_slave+slaveid); // slaveid es de debug
    if(isopen_master && isopen_slave){
    	sleep(100000); // Paro este thread. Con signal se desactiva	esto
    }
    else {
	    if(!isopen_master) printf("El servidor para el Master no pudo iniciar\n");
	    if(!isopen_slave)  printf("El servidor para los Slave no pudo iniciar\n");
    }
    printf("Programa terminado\n");
    return 0;
}

//////////////////////////////FUNCTIONS////////////////////////////////
void argvHandler(int argc, const char** argv)
{
	if(argc < 3)
	{
		printf("Parameters: ./slave.exe <PORT_SRVMASTER> <PORT_SRVSLAVES> [<SLAVE_ID>]\n");
		exit(0);
	}
	if(argc >=3)
	{
		port_master = atoi(argv[1]);
		port_slave = atoi(argv[2]);
	}
	if(argc >= 4)
	{
		slaveid = atoi(argv[3]);
		debugMode = true;
		printf("Fase de debug\n");
	}
}
