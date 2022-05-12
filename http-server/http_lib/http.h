// HTTP Server supports protocol version HTTP/1.0
// Does not support HTTP/0.9, HTTP/1.1 & HTTP/2.0

#ifndef HTTP_SERVER_HTTP_H
#define HTTP_SERVER_HTTP_H

#include "../scheduler_lib/scheduler_lib.h"
#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <netdb.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>
#include <vector>

#define RESPONSE_400 http::Response("400", "Bad Request")
#define RESPONSE_404 http::Response("404", "Not Found")
#define RESPONSE_411 http::Response("411", "Length Required");
#define RESPONSE_500 http::Response("500", "Internal Server Error")

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

  Request() = default;
};

struct Response {
  std::map<std::string, std::string> headers;
  std::string statusNumber;
  std::string statusInfo;
  std::string version;
  std::string body;

  Response() = default;

  Response(std::string number, std::string info)
      : statusNumber(std::move(number)), statusInfo(std::move(info)),
        version("HTTP/1.0") {}
};

struct Service {
  virtual Response doService(const Request &) = 0;
};

template <typename T>
void handle(const T &, const T &, const std::string &, bool = true);

Text parse(const std::string &);

class Server {
private:
  struct Translator {
    int clientSocket;
    const std::map<std::pair<std::string, std::string>, Service *>
        *functionality;
  };

  std::unique_ptr<std::map<std::pair<std::string, std::string>, Service *>>
      functionality;

  static void getHead(int, std::string &, std::string &);
  static int parseRequestHead(http::Request &, std::string &);
  static void getBody(int, int, std::string &);
  [[nodiscard]] static Request getRequest(int);
  [[nodiscard]] static Response generateResponse(
      const std::map<std::pair<std::string, std::string>, Service *> *,
      const Request &);
  static void sendResponse(int, const Response &);
  static void answer(void *);

  [[nodiscard]] static int bindServerSocket(uint16_t, int);
  [[nodiscard]] static int acceptClientSocket(int);

public:
  Server()
      : functionality(
            std::make_unique<
                std::map<std::pair<std::string, std::string>, Service *>>()) {}

  void init(const std::string &, const std::string &, http::Service *);

  [[noreturn]] void run(uint16_t port = 8080, int numberOfThreads = 16) const;
};
} // namespace http

#endif // HTTP_SERVER_HTTP_H

/*
---Examples---

-(Request)-
GET /my-page.html HTTP/1.0
User-Agent: NCSA_Mosaic/2.0 (Windows 3.1)

-(Response)-
HTTP/1.0 200 OK
Content-Type: text/html
Content-Length: 137582
Expires: Thu, 01 Dec 1997 16:00:00 GMT
Last-Modified: Wed, 1 May 1996 12:45:26 GMT
Server: Apache 0.84

<HTML>
A page with an image
  <IMG SRC="/myimage.gif">
</HTML>
 */
