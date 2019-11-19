/* Client code in C */
 
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
using namespace std;

#define IP_CLIENT "127.0.0.1"

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

///////////////////////////////////SERVER/////////////////////////////////
class ClientConnection: public PeerConnection
{
public:
    ClientConnection(int socketFD): PeerConnection(socketFD)
    {}

    virtual void sendPackagesHandler() override
    {
        
    }

    virtual void receivePackagesHandler() override
    {
        this->disableReceiver = true;
    }

    void sendFileToClient(string filename)
    {
        DBG(filename)
        File file = readFile(filename);
        sender.sendStruct<File>(file);
    }
};


class ServerConnection: public PeerConnection
{
private:
    Client<ClientConnection> clientconn;

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
        char packid = receiver.recvStr(1)[0];
        receiver.ignore(1);
        switch(packid)
        {
            case '1':
            {
                int port_client = stoi(receiver.recvField(2));
                string ip_client = IP_CLIENT;
                clientconn.newThread_connectToServer(ip_client, port_client);
            }
            break;
            case '2':
            {
                string filename = receiver.recvField(3);
                ClientConnection* clnt = clientconn.getInstanceOfPeerConnection();
                clnt->sendFileToClient(filename);
            }
            break;
            case '3':
            {
                File file = receiver.recvStruct<File>();
                setPrefixFilename("PUT_", file);
                saveFile(file);
            }
            case '4':
            {
                closeConnection();
                printf("Conexion cerrada\n");
            }
        }
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
    int servport;
    if(argc < 2)
    {
        printf("Parameters: ./server.exe <PORT_SERVER>\n");
        return 0;
    }
    if(argc >= 2) 
    {
        servport = atoi(argv[1]);
    }
    if(argc >= 3) 
    {
        printf("TESTING WITH USER INPUT:\n%s\n", argv[2]);
        sui.simulate(argv[2]);
    }

    Server<ServerConnection> receiver;
    receiver.newThread_turnOnServer(servport);

    sleep(100000); // Con signal se desactiva
    printf("Programa terminado\n");
    return 0;
}


