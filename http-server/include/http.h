// HTTP Server supports protocol version HTTP/1.0 & HTTP/1.1
// Does not support HTTP/0.9 & HTTP/2.0

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

  [[nodiscard]] uint16_t getPort() const
  { return port; }

  [[nodiscard]] int getNumberOfThreads() const
  { return numberOfThreads; }
};
}

#endif //HTTP_SERVER_HTTP_H
