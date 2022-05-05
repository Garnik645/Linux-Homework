// HTTP Server supports protocol version HTTP/1.0
// Does not support HTTP/0.9, HTTP/1.1 & HTTP/2.0

#ifndef HTTP_SERVER_HTTP_H
#define HTTP_SERVER_HTTP_H

#include <cstring>
#include <map>
#include <exception>
#include <iostream>
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include "../scheduler_lib/scheduler_lib.h"

namespace http {

struct Request {
  std::map<std::string, std::string> header;
  std::string method;
  std::string path;
  std::string version;
  std::string body;
};

struct Response {
  std::map<std::string, std::string> header;
  std::string statusNumber;
  std::string statusInfo;
  std::string version;
  std::string body;
};

struct Service {
  virtual Response doService(const Request &) = 0;
};

class Server {
private:
  struct Translator {
    int clientSocket;
    const std::map<std::pair<std::string, std::string>, Service *> *functionality;
  };

  std::map<std::pair<std::string, std::string>, Service *> functionality;
  uint16_t port;
  int numberOfThreads;

  template<typename T>
  static void handle(const T &, const T &, const std::string &);

  [[nodiscard]] static Request getRequest(int);
  [[nodiscard]] static void sendResponse(int, const Response&);

  static void answer(void *);

public:
  explicit Server(uint16_t _port = 80, int _numberOfThreads = 16)
      : port(_port), numberOfThreads(_numberOfThreads) {}

  [[noreturn]] void run() const;

  void addFunctionality(const std::pair<std::string, std::string> &key, http::Service *value) {
    functionality[key] = value;
  }

  [[nodiscard]] uint16_t getPort() const { return port; }

  [[nodiscard]] int getNumberOfThreads() const { return numberOfThreads; }
};
} // namespace http

#endif // HTTP_SERVER_HTTP_H
