//////////////////////////////AUX/////////////////////////////
// Agrega una capa de seguridad a cada funcion
// Separa el flujo principal del algoritmo, del flujo de excepciones
// Es "Safe" solo porque puede lanzar excepciones, no mas ni menos
// Lo unico que hace es que, si hay error, lanzar excepcion.
// No me pregunten mas de que sirve!!
#ifndef SAFE_H
#define SAFE_H
#include <sys/types.h>
#include <sys/socket.h>

namespace Safe
{
int socket(int domain, int type, int protocol);
int inet_pton(int af, const char* src, void* dst);
void connect(int sockFD, struct sockaddr* serv_addr, int addrlen);
void bind(int sockFD, struct sockaddr* my_addr, int addrlen);
void listen(int sockFD, int backlog);
int accept(int sockFD, struct sockaddr* addr, socklen_t* addrlen);
}

namespace Safe
{
int socket(int domain, int type, int protocol)
{
  int sockFD = ::socket(domain, type, protocol);
  if (sockFD < 0)
    throw std::string("cannot create socket");
  return sockFD;
}

int inet_pton(int af, const char* src, void* dst)
{
  int res = ::inet_pton(af, src, dst);
  if (0 > res)
    throw std::string("error: first parameter is not a valid address family");
  else if (0 == res)
    throw std::string("char std::string (second parameter does not contain valid ipaddress");
  return res;
}

void connect(int sockFD, struct sockaddr* serv_addr, int addrlen)
{
  int res = ::connect(sockFD, serv_addr, addrlen);
  if (res < 0)
    throw std::string("connect failed");
}

void bind(int sockFD, struct sockaddr* my_addr, int addrlen)
{
  int res = ::bind(sockFD, my_addr, addrlen);
  if (res < 0)
    throw std::string("error bind failed");
}

void listen(int sockFD, int backlog)
{
  int res = ::listen(sockFD, backlog);
  if (res < 0)
    throw std::string("error listen failed");
}

int accept(int sockFD, struct sockaddr* addr, socklen_t* addrlen)
{
  int connectFD = ::accept(sockFD, addr, addrlen);
  if (connectFD < 0)
    throw std::string("error accept failed");
  return connectFD;
}
}

#endif