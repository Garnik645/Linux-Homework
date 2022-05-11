// HTTP Server supports protocol version HTTP/1.0
// Does not support HTTP/0.9, HTTP/1.1 & HTTP/2.0

#ifndef HTTP_SERVER_HTTP_H
#define HTTP_SERVER_HTTP_H

#include <cstring>
#include <map>
#include <exception>
#include <utility>
#include <memory>
#include <sstream>
#include <iostream>
#include <vector>
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../scheduler_lib/scheduler_lib.h"

#define BUFFER_SIZE 10002

namespace http {
typedef std::vector<std::vector<std::string>> Text;
typedef std::vector<std::string> Line;

struct Request {
  std::map<std::string, std::string> headers;
  std::string method;
  std::string path;
  std::string version;
  std::string body;
};

struct Response {
  std::map<std::string, std::string> headers;
  std::string statusNumber;
  std::string statusInfo;
  std::string version;
  std::string body;
};

struct Service {
  virtual Response doService(const Request &) = 0;
};

template<typename T>
void handle(const T &, const T &, const std::string &);

Text parse(const std::string &);

class Server {
private:
  struct Translator {
    int clientSocket;
    const std::map<std::pair<std::string, std::string>, Service *> *functionality;
  };

  std::unique_ptr<std::map<std::pair<std::string, std::string>, Service *>> functionality;

  [[nodiscard]] static Request getRequest(int);
  static void sendResponse(int, const Response &);
  static void answer(void *);

  [[nodiscard]] static int getClientSocket(int);
  [[nodiscard]] static int getServerSocket(uint16_t, int);

public:
  Server() : functionality(std::make_unique<std::map<std::pair<std::string, std::string>, Service *>>()) {}

  void init(const std::string &method, const std::string &path, http::Service *value) {
    (*functionality)[std::make_pair(method, path)] = value;
  }

  [[noreturn]] void run(uint16_t, int) const;
};
} // namespace http

#endif // HTTP_SERVER_HTTP_H
