#include "http_lib/http.h"

struct StaticFile : http::Service {
  std::string fileContent;

  explicit StaticFile(std::string _fileContent)
      : fileContent(std::move(_fileContent)) {}

  http::Response doService(const http::Request &request) override {
    http::Response response;
    response.version = "HTTP/1.0";
    response.headers["Content-Length"] = std::to_string(fileContent.size());
    response.body = fileContent;
    response.statusNumber = std::to_string(200);
    response.statusInfo = "OK";
    return response;
  }
};

int main(int argc, char *argv[]) {
  http::Server server;
  StaticFile file("Hello, World!\n");
  server.init("GET", "/hello", &file);
  server.run();
}
