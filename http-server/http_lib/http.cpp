#include "http.h"
// TODO add normal Log for server
// TODO handle client disconnection

template<typename T>
void http::handle(const T &returnValue, const T &errorValue, const std::string &errorMessage) {
  if (returnValue == errorValue) {
    throw std::runtime_error(errorMessage + " Error : " + std::to_string(errno));
  }
}

http::Text http::parse(const std::string &str) {
  http::Text text;
  text.push_back(http::Line());
  bool newString = true;
  for (char i : str) {
    if (std::isspace(i) == 0) {
      if (newString) {
        newString = false;
        text.back().push_back(std::string());
      }
      text.back().back().push_back(i);
    } else {
      newString = true;
      if (i == '\n') {
        text.push_back(http::Line());
      }
    }
  }
  return text;
}

[[nodiscard]] http::Request http::Server::getRequest(int clientSocket) {
  // TODO error handling
  // TODO read head and read body
  http::Request request;
  std::string head;
  std::string body;
  int bodySize = 0;
  char buff[BUFFER_SIZE];
  bool emptyLineReached = false;
  while (!emptyLineReached) {
    ssize_t reading = recv(clientSocket, buff, BUFFER_SIZE, 0);
    handle(reading, static_cast<ssize_t>(-1), "Couldn't receive a message from a socket!");
    for (size_t i = 0; i < reading; ++i) {
      if (!emptyLineReached && !head.empty() && head.back() == '\n' && buff[i] == '\n') {
        emptyLineReached = true;
        continue;
      }
      if (!emptyLineReached) {
        head += buff[i];
      } else {
        body += buff[i];
      }
    }
  }
  // TODO remove this line
  std::cout << "Empty line reached" << std::endl;
  // TODO generate http request from text
  Text headText = parse(head);
  request.method = headText[0][0];
  request.path = headText[0][1];
  request.version = headText[0][2];
  for (size_t i = 1; i < headText.size(); ++i) {
    std::string key = headText[i][0];
    key.pop_back();
    std::string value = headText[i][1];
    if (key == "Content-Length") {
      bodySize = std::stoi(value);
    }
    request.headers[std::move(key)] = std::move(value);
  }
  while (body.size() < bodySize) {
    ssize_t reading = recv(clientSocket, buff, BUFFER_SIZE, 0);
    for (size_t i = 0; i < reading && body.size() < bodySize; ++i) {
      body += buff[i];
    }
  }
  if (body.size() > bodySize) {
    body = body.substr(0, bodySize);
  }
  request.body = body;
  return request;
}

void http::Server::sendResponse(int clientSocket, const http::Response &response) {
  std::string responseString;
  responseString += response.version + ' ';
  responseString += response.statusNumber + ' ';
  responseString += response.statusInfo + '\n';
  for (const auto &header : response.headers) {
    responseString += header.first + ": " + header.second + '\n';
  }
  responseString += '\n' + response.body;
  ssize_t sending = send(clientSocket, responseString.c_str(), responseString.size(), 0);
  handle(sending, static_cast<ssize_t>(-1), "Couldn't send a message on a socket!");
}

void http::Server::answer(void *data) {
  auto translationUnit = reinterpret_cast<Translator *>(data);

  http::Request clientRequest = getRequest(translationUnit->clientSocket);

  // TODO combine response generation into one file
  std::pair<std::string, std::string> requestType = std::make_pair(clientRequest.method, clientRequest.path);
  auto finding = translationUnit->functionality->find(requestType);
  if (finding == translationUnit->functionality->end()) {
    // TODO generate error 404 HTTP Response
    http::Response resp;
    resp.version = "HTTP/1.0";
    resp.statusNumber = "404";
    resp.statusInfo = "Not Found";
    sendResponse(translationUnit->clientSocket, resp);
  }
  auto clientService = finding->second;
  http::Response clientResponse = clientService->doService(clientRequest);

  sendResponse(translationUnit->clientSocket, clientResponse);
  delete translationUnit;
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
    sockaddr_in clientAddress{};
    socklen_t clientAddressLen = sizeof(clientAddress);
    int clientSocket = accept4(serverSocket, reinterpret_cast<sockaddr *>(&clientAddress), &clientAddressLen, SOCK_CLOEXEC);
    handle(clientSocket, -1, "Couldn't accept a connection!");

    // TODO log print client info
    char *clientIP = inet_ntoa(clientAddress.sin_addr);
    int clientPort = ntohs(clientAddress.sin_port);
    std::cout << "Got connection from " << clientIP << ':' << clientPort << std::endl;

    auto translationUnit = new Translator;
    translationUnit->clientSocket = clientSocket;
    translationUnit->functionality = functionality.get();
    scheduler->run(answer, reinterpret_cast<void *>(translationUnit));
  }
}
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
