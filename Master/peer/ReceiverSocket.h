#include <sys/socket.h>
#include <string>
#include <cmath>

class ReceiverSocket
{
private:
    int socketFD;

public:
    ReceiverSocket(int socketFD)
    {
        this->socketFD = socketFD;
    }

    // TODO: si recibe 0 bytes, se cerro conexion
    // Llega a lo mucho x bytes
    std::string recvStr(int bytes)
    {
        std::string s(bytes, '\0');
        int cnt = recv(this->socketFD, (char*)s.c_str(), bytes, 0);
        s = s.substr(0, cnt);
        return s;
    }

    // Se bloquea cuando llega un paquete chiquito
    std::string recvBigStr(int bytes)
    {
        std::string ans, s;
        size_t maxpacksize = 0;
        do{
            s = recvStr(bytes);
            maxpacksize = std::max(maxpacksize, s.size());
            ans += s;
        }while(s.size() == maxpacksize);
        return ans;
    }

    // Para recibir paquetes tipo: "1 5 Oscar 7 Ramirez"
    std::string recvField(int bytes)
    {
        std::string size = recvStr(bytes);
        ignore(1);
        std::string field = recvStr(stoi(size));
        return field;
    }

    // La estructura DEBE ser serializable(puro int, char[]. no std::string, std::vector)
    template <typename T> 
    T recvStruct()
    {
        T mystruct;
        recv(this->socketFD, (T*)&mystruct, sizeof(T), 0); // TODO: Reconocer cuando recv == 0, conexion cerrada
        return mystruct;
    }

    void ignore(int bytes)
    {
        char trash[bytes];
        recv(this->socketFD, trash, bytes, 0);
    }    
};
