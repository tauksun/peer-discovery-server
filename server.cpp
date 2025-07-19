#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

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

  // Listen
  int listenResult = listen(socketDescriptor, 100);
  cout << "listenResult : " << listenResult << endl;

  // Accept
  struct sockaddr_in clientSock;
  int clientDescriptor = accept(socketDescriptor, NULL, NULL);
  cout << "Got client with descriptor : " << clientDescriptor << endl;

  string clientMessage;
  recv(clientDescriptor, &clientMessage, 1023, 0);

  string serverReply = "Hey there !!!";
  write(clientDescriptor, &serverReply, sizeof(serverReply));
}
