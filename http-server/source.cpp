#include "http_lib/http.h"

int main(int argc, char *argv[]) {
  http::Server server;
  server.run();
}
