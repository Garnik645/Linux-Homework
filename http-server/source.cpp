#include "http_lib/http.h"

struct StaticFile : http::Service {
  std::string fileContent;

  explicit StaticFile(std::string _fileContent)
      : fileContent(std::move(_fileContent)) {}

  http::Response doService(const http::Request &request) final {
    http::Response response;
    response.version = "HTTP/1.0";
    response.headers["Content-Length"] = std::to_string(fileContent.size());
    response.body = fileContent;
    response.statusNumber = std::to_string(200);
    response.statusInfo = "OK";
    return response;
  }
};

struct Upper : http::Service {
  http::Response doService(const http::Request &request) override {
    http::Response response;
    auto it = request.headers.find("Content-Length");
    if (request.body.empty() || it == request.headers.end()) {
      throw RESPONSE_411;
    }
    response.version = "HTTP/1.0";
    response.headers["Content-Length"] = it->second;
    response.body = request.body;
    std::transform(response.body.begin(), response.body.end(),
                   response.body.begin(), ::toupper);
    response.statusNumber = std::to_string(200);
    response.statusInfo = "OK";
    return response;
  }
};

int main(int argc, char *argv[]) {
  http::Server server;
  StaticFile file("Hello, World!\n");
  Upper upper;
  server.init("GET", "/hello", &file);
  server.init("POST", "/upper", &upper);
  server.run();
}
