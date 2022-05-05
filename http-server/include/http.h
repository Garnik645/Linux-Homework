#ifndef HTTP_SERVER_HTTP_H
#define HTTP_SERVER_HTTP_H

#include <sys/socket.h>
#include <netdb.h>
#include <cstring>

#include "error_handler.h"

namespace http
{
class Request
{
};

class Response
{
};

class Server
{
private:
  uint16_t port;
  int numberOfThreads;

public:
  explicit Server(uint16_t _port = 80, int _numberOfThreads = 16)
          : port(_port), numberOfThreads(_numberOfThreads)
  {}

  [[noreturn]] void run() const;

  uint16_t getPort() const
  { return port; }

  int getNumberOfThreads() const
  { return numberOfThreads; }
};
}


#endif //HTTP_SERVER_HTTP_H
