#include <sys/socket.h>
#include <string>
#include <cmath>
#include <iostream> // quitar al terminar Debug
using namespace std; // tambien

class ReceiverSocket
{
private:
    int socketFD;

public:
    ReceiverSocket(int socketFD)
    {
        this->socketFD = socketFD;
    }

    // Para recibir paquetes tipo: "1 5 Oscar 7 Ramirez"
    std::string recvField(int bytes)
    {
        size_t size = recvInt(bytes);
        ignore(1);
        std::string field = recvStr(size);
        return field;
    }

    // si recibe 0 bytes, se cerro conexion del otro lado
    // Llega a lo mucho x bytes
    std::string recvStr(int bytes)
    {
        std::string s;
        //cout << "Receiver: Recibiendo string" << endl;
        for(int i=0; i<bytes; ++i)
            s += recvChar();
        //cout << endl << "Receiver: Termino de recibir" << endl;
        return s;
    }

    int recvInt(int bytes)
    {
        return stoi(recvStr(bytes));
    }

    char recvChar()
    {
        char c;
        int cnt = recv(this->socketFD, &c, 1, 0);
        if(cnt == 0) c = '\0';
        //cout << c; // Debug
        return c;
    }

    // obsoleto?
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

    // La estructura DEBE ser serializable(puro tipo primitivo: int, char[]. no std::string, std::vector)
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
