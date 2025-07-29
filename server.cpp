#include "config.h"
#include "logger.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

using namespace std;

void parseMessage(char *message, unordered_map<string, string> &messageData) {
  // Message format
  // action:key:value:key:value...
  int count = 0;
  string temp;
  string key;
  int len = strlen(message);
  for (int i = 0; i < len; i++) {
    if ((strncmp(&message[i], ":", 1) == 0) || (i == len - 1)) {
      if (count == 0) {
        // action
        messageData["action"] = temp;
      } else if (count % 2 == 0) {
        // value
        messageData[key] = temp;
        key = "";
      } else {
        // key
        key = temp;
      }

      count++;
      temp = "";
    } else {
      temp += message[i];
    }
  }
}

void handleClient(int readyFd, int epollfd) {
  char clientMessage[1024] = {0};
  int recvValue = recv(readyFd, clientMessage, 1023, 0);

  if (recvValue <= 0) {
    close(readyFd);
    epoll_ctl(epollfd, EPOLL_CTL_DEL, readyFd, nullptr);
    return;
  }

  if (recvValue > 0 && recvValue < 1024)
    clientMessage[recvValue] = '\0';
  else
    clientMessage[1023] = '\0';

  logger("clientMessage : ", clientMessage);

  string serverReply;
  unordered_map<string, string> messageData;
  parseMessage(clientMessage, messageData);

  auto action = messageData.find("action");

  if (action == messageData.end()) {
    logger("Couldn't find action for message : ", clientMessage);
    serverReply = "Invalid message";
  } else {
    logger("action : ", action->second);
    const char *registerUser = "register";
    const char *heartbeat = "heartbeat";

    if (strncmp(action->second.c_str(), registerUser, strlen(registerUser)) ==
        0) {
      auto user = messageData.find("username");
      logger("Registering username : ", user->second);

      // If username already exits, ask to choose another
      // else register & share passkey
    } else if (strncmp(action->second.c_str(), registerUser,
                       strlen(registerUser)) == 0) {
      auto user = messageData.find("username");
      logger("Updating IP for username : ", user->second);
      // Update IP against username
    } else {
      // Default
      logger("Not a valid action");
      serverReply = "Invalid message";
    }
  }

  write(readyFd, serverReply.c_str(), strlen(serverReply.c_str()));
}

void server() {
  cout << "Starting server" << endl;
  // Socket
  int socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
  cout << "Created socket with descriptor : " << socketDescriptor << endl;

  // Bind
  struct sockaddr_in add;
  add.sin_family = AF_INET;
  add.sin_port = htons(Config::TCP_SERVER_PORT);
  add.sin_addr.s_addr = INADDR_ANY;

  int bindResult = bind(socketDescriptor, (struct sockaddr *)&add, sizeof(add));
  cout << "bindResult : " << bindResult << endl;
  if (bindResult != 0) {
    throw "Error while binding to socket";
  }

  int yes = 1;
  if (setsockopt(socketDescriptor, SOL_SOCKET, SO_REUSEADDR, &yes,
                 sizeof(yes)) < 0) {
    cout << "setsockopt(SO_REUSEADDR) failed" << endl;
  }

  // Listen
  int listenResult = listen(socketDescriptor, 100);
  cout << "listenResult : " << listenResult << endl;
  if (listenResult != 0) {
    throw "Error while listening on socket";
  }

  // Accept & Process Clients
  // E-Poll
  int maxConnections = 1000;
  struct epoll_event ev;
  vector<struct epoll_event> events(maxConnections);

  // Create E-Poll instance
  int epollfd = epoll_create1(0);
  if (epollfd == -1) {
    perror("Epoll error");
    exit(EXIT_FAILURE);
  }

  ev.events = EPOLLIN;
  ev.data.fd = socketDescriptor;

  // Add socketDescriptor for monitoring
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, socketDescriptor, &ev) == -1) {
    perror("Epoll socketDescriptor error");
    exit(EXIT_FAILURE);
  }

  while (1) {
    int readyFds = epoll_wait(epollfd, &events[0], maxConnections, -1);
    if (readyFds == -1) {
      perror("epoll_wait error");
      exit(EXIT_FAILURE);
    }

    for (int i = 0; i < readyFds; i++) {
      if (events[i].data.fd == socketDescriptor) {
        // Accept Client Connection
        int clientDescriptor = accept(socketDescriptor, NULL, NULL);
        if (clientDescriptor < 0) {
          cout << "client accept error : " << clientDescriptor << endl;
          continue;
        }
        cout << "Got client with descriptor : " << clientDescriptor << endl;

        ev.events = EPOLLIN;
        ev.data.fd = clientDescriptor;

        int clientAdd =
            epoll_ctl(epollfd, EPOLL_CTL_ADD, clientDescriptor, &ev);

        if (clientAdd == -1) {
          perror("epoll_ctl client add error");
          close(clientDescriptor);
        }
      } else {
        handleClient(events[i].data.fd, epollfd);
      }
    }
  }
}
