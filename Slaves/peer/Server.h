#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <thread>
#include <unistd.h>
#include "../aux/Safe.h"
#include "PeerConnection.h"
  

template<typename T>
class Server
{
    static_assert(std::is_base_of<PeerConnection, T>::value, "T debe ser derivado de PeerConnection");
private:
    int maxNumClients = 10;
    T* peerConn; 
    int socketFD;

public:
    // IP y puerto perteneciente a server a conectar
    Server() = default;

    bool newThread_turnOnServer(int servPort)
    {
        bool isCreated = turnOnServer(servPort);
        if(isCreated){
            std::thread th(&Server::acceptNewConnections, this, socketFD);
            th.detach();
            return true;
        }
        return false;
    }

    void newThread_printNumberOfClientsOnline()
    {
        ; // TODO 
    }

    void setMaxNumClients(int x)
    {
        this->maxNumClients = x;
    }

private:
    bool turnOnServer(int servPort)
    {
        try{
            // Crear socket
            socketFD = Safe::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            struct sockaddr_in servAddr = constructServerAddressStructure(servPort);
            
            //Asignar puerto y Habilitar comunicaciones
            Safe::bind(socketFD, (struct sockaddr*)&servAddr, sizeof(struct sockaddr_in));
            Safe::listen(socketFD, this->maxNumClients); 
            printf("Servidor: abierto!\n");
            return true;
        }
        catch (std::string msg_error)
        {
            fprintf(stderr, "Error en Server.h\n");
            perror(msg_error.c_str());
            return false;
        }
    }

    void acceptNewConnections(int socketFD)
    {
        // Aceptar conexiones de clientes
        do{
            sockaddr_in clntAddr;
            socklen_t clntAddrLen = sizeof(clntAddr);
            int connectFD = Safe::accept(socketFD, (struct sockaddr*)&clntAddr, &clntAddrLen);
            printf("Servidor: nuevo cliente\n");
            printIpPort(clntAddr);

            peerConn = new T(connectFD);
            std::thread th1(&T::sendPackages, peerConn);
            std::thread th2(&T::receivePackages, peerConn);
            th1.detach();
            th2.detach();
        }while(true);

        // Cerrar servidor
        shutdown(socketFD, SHUT_RDWR);
        close(socketFD);
        printf("Servidor: cerrado!\n");
    }

    struct sockaddr_in constructServerAddressStructure(int servPort)
    {
        struct sockaddr_in servAddr;
        memset(&servAddr, 0, sizeof(struct sockaddr_in));
        servAddr.sin_family = AF_INET;
        servAddr.sin_port = htons(servPort);
        servAddr.sin_addr.s_addr = INADDR_ANY;
        return servAddr;
    }

    void printIpPort(sockaddr_in addr)
    {
        char clntIp[INET_ADDRSTRLEN]; 
        if(inet_ntop(AF_INET, &addr.sin_addr.s_addr, clntIp, INET_ADDRSTRLEN))
            printf("(%s:%d)\n", clntIp, ntohs(addr.sin_port));
        else
            printf("sin ip y puerto disponibles\n");
    }
};

#endif