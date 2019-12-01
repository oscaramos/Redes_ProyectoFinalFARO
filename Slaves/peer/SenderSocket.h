#ifndef SENDERSOCKET_H
#define SENDERSOCKET_H

#include <sys/socket.h>
#include <string>
#include <iostream>

class SenderSocket
{
private:
    int socketFD;

public:
    SenderSocket(int socketFD)
    {
        this->socketFD = socketFD;
    }

    void sendStr(std::string str)
    {
        send(this->socketFD, str.c_str(), str.size(), 0);
    }

    template <typename T> 
    void sendStruct(T mystruct)
    {
        send(this->socketFD, (T*)&mystruct, sizeof(T), 0); // TODO: Reconocer cuando recv == 0, conexion cerrada
    }
};

#endif