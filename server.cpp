#include <algorithm>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

using namespace std;

void handleClient(int readyFd, vector<int> *clientRemove) {
  char clientMessage[1024] = {0};
  int recvValue = recv(readyFd, clientMessage, 1023, 0);

  if (recvValue <= 0) {
    clientRemove->push_back(readyFd);
    return;
  }

  if (recvValue > 0 && recvValue < 1024)
    clientMessage[recvValue] = '\0';
  else
    clientMessage[1023] = '\0';

  if ((strcmp(clientMessage, "x") != 0)) {
    string serverReply = clientMessage;
    write(readyFd, serverReply.c_str(), serverReply.size());
  } else {
    string serverReply = "Closing connection";
    write(readyFd, serverReply.c_str(), serverReply.size());
    clientRemove->push_back(readyFd);
  }
}

void server() {
  cout << "Starting server" << endl;
  // Socket
  int socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
  cout << "Created socket with descriptor : " << socketDescriptor << endl;

  // Bind
  struct sockaddr_in add;
  add.sin_family = AF_INET;
  add.sin_port = htons(4345);
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
  vector<int> connections;
  fd_set readfds;

  while (1) {
    int maxFd = socketDescriptor;
    FD_ZERO(&readfds);
    FD_SET(socketDescriptor, &readfds);

    for (int i = 0; i < connections.size(); i++) {
      if (connections[i] > maxFd) {
        maxFd = connections[i];
      }
      FD_SET(connections[i], &readfds);
    }

    // Blocks here till any file descriptor is ready
    int selectReturn = select(maxFd + 1, &readfds, NULL, NULL, NULL);
    if (selectReturn < 0) {
      cout << "Error in select, selectReturn : " << selectReturn << endl;
      break;
    }

    if (FD_ISSET(socketDescriptor, &readfds)) {
      // Accept Client Connection
      int clientDescriptor =
          accept(socketDescriptor, NULL, NULL); // will block here
      if (clientDescriptor < 0) {
        cout << "client accept error : " << clientDescriptor << endl;
        continue;
      }
      cout << "Got client with descriptor : " << clientDescriptor << endl;
      connections.push_back(clientDescriptor);
    }

    // Check & Handle client descriptor
    vector<int> clientRemove;
    for (int i = 0; i < connections.size(); i++) {
      if (FD_ISSET(connections[i], &readfds)) {
        handleClient(connections[i], &clientRemove);
      }
    }

    // Remove disconnected / closing clients
    for (int i = 0; i < clientRemove.size(); i++) {
      close(clientRemove[i]);
      connections.erase(
          std::remove(connections.begin(), connections.end(), clientRemove[i]),
          connections.end());
    }
  }
}
