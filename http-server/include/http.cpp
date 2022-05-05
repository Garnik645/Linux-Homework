#include "http.h"

[[noreturn]] void http::Server::run() const
{
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  error_handler(serverSocket, -1, "Couldn't create an endpoint for communication!");

  sockaddr_in address{};
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  int binding = bind(serverSocket, (const struct sockaddr *) &address, sizeof(address));
  error_handler(binding, -1, "Couldn't bind a name to a socket!");

  int listening = listen(serverSocket, numberOfThreads);
  error_handler(listening, -1, "Couldn't listen for connections!");

  while (true)
  {
    sockaddr clientAddress{};
    socklen_t clientAddressLen = 0;
    int clientSocket = accept(serverSocket, &clientAddress, &clientAddressLen);
    error_handler(clientSocket, -1, "Couldn't accept a connection!");
  }
}
