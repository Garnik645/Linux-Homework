#include "http.h"

template<typename T>
void http::Server::handle(const T &returnValue, const T &errorValue, const std::string &errorMessage) {
  if (returnValue == errorValue) {
    throw std::runtime_error(errorMessage + " Error : " + std::to_string(errno));
  }
}

void http::Server::answer(void *data) {
  auto translationUnit = reinterpret_cast<Translator *>(data);
  auto clientSocket = translationUnit->clientSocket;

}

[[noreturn]] void http::Server::run() const {
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  handle(serverSocket, -1, "Couldn't create an endpoint for communication!");

  sockaddr_in address{};
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  int binding = bind(serverSocket, reinterpret_cast<const struct sockaddr *>(&address), sizeof(address));
  handle(binding, -1, "Couldn't bind a name to a socket!");

  int listening = listen(serverSocket, numberOfThreads);
  handle(listening, -1, "Couldn't listen for connections!");

  auto *scheduler = new parallel_scheduler(numberOfThreads);

  while (true) {
    sockaddr clientAddress{};
    socklen_t clientAddressLen = 0;
    int clientSocket = accept4(serverSocket, &clientAddress, &clientAddressLen, SOCK_CLOEXEC);
    handle(clientSocket, -1, "Couldn't accept a connection!");

    auto translationUnit = new Translator;
    translationUnit->clientSocket = clientSocket;
    scheduler->run(answer, reinterpret_cast<void *>(translationUnit));
  }
}
