#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <thread>
#include "../aux/Safe.h"
#include "PeerConnection.h"
  

template<typename T>
class Server
{
    static_assert(std::is_base_of<PeerConnection, T>::value, "T debe ser derivado de PeerConnection");
private:
    int maxNumClients = 10;

public:
    // IP y puerto perteneciente a server a conectar
    Server() = default;

    void newThread_turnOnServer(int servPort)
    {
        std::thread th(&Server::turnOnServer, this, servPort);
        th.detach();
    }

    void turnOnServer(int servPort)
    {
        try{
            // Crear socket
            int socketFD = Safe::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            struct sockaddr_in servAddr = constructServerAddressStructure(servPort);
            
            //Asignar puerto y Habilitar comunicaciones
            Safe::bind(socketFD, (struct sockaddr*)&servAddr, sizeof(struct sockaddr_in));
            Safe::listen(socketFD, this->maxNumClients); 
            printf("Servidor: abierto!\n");

            // Aceptar conexiones de clientes
            do{
                sockaddr_in clntAddr;
                socklen_t clntAddrLen = sizeof(clntAddr);
                int connectFD = Safe::accept(socketFD, (struct sockaddr*)&clntAddr, &clntAddrLen);
                printf("Servidor: nuevo cliente ");
                char clntIp[INET_ADDRSTRLEN]; 
                if(inet_ntop(AF_INET, &clntAddr.sin_addr.s_addr, clntIp, INET_ADDRSTRLEN))
                    printf("con ip = %s | puerto = %d\n", clntIp, ntohs(clntAddr.sin_port));
                else
                    printf("no disponible ip y puerto\n");

                T* clientConn = new T(connectFD);
                std::thread th1(&T::sendPackages, clientConn);
                std::thread th2(&T::receivePackages, clientConn);
                th1.detach();
                th2.detach();
            }while(true);

            // Cerrar servidor
            shutdown(socketFD, SHUT_RDWR);
            close(socketFD);
            printf("Servidor: cerrado!\n"); 
        }
        catch (std::string msg_error)
        {
            fprintf(stderr, "Error en Server.h\n");
            perror(msg_error.c_str());
            exit(EXIT_FAILURE);
        }
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

    void newThread_printNumberOfClientsOnline()
    {
        ; // TODO 
    }

    void setMaxNumClients(int x)
    {
        this->maxNumClients = x;
    }
};