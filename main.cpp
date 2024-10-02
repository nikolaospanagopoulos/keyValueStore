#include "KVstore.hpp"
#include "ThreadPool.hpp"
#include <arpa/inet.h>
#include <csignal>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

KVstore kvStore;

// Function to handle individual client connections
void handleClient(int clientSocket) {
  char buffer[BUFFER_SIZE];
  while (true) {
    // Clear the buffer
    memset(buffer, 0, BUFFER_SIZE);

    // Read the command from the client
    int bytesRead = read(clientSocket, buffer, BUFFER_SIZE - 1);
    if (bytesRead <= 0) {
      std::cerr << "Client disconnected or error reading from client"
                << std::endl;
      break;
    }

    std::string command(buffer);

    // Process the command and get the response
    std::string response = kvStore.processCommand(command);

    // Send the response back to the client
    send(clientSocket, response.c_str(), response.size(), 0);
  }

  close(clientSocket);
}

int main() {
  // Create a thread pool with 4 workers
  ThreadPool pool(4);

  // Create socket
  // SOCK_STREAM = tcp
  // AF_INET = ipv4
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == -1) {
    std::cerr << "Failed to create socket" << std::endl;
    return 1;
  }

  // Set server address and port
  sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  serverAddr.sin_port = htons(PORT);

  // Bind the socket to the IP/port
  if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) <
      0) {
    std::cerr << "Bind failed" << std::endl;
    close(serverSocket);
    return 1;
  }

  // Start listening for connections
  if (listen(serverSocket, 10) < 0) {
    std::cerr << "Failed to listen" << std::endl;
    close(serverSocket);
    return 1;
  }

  std::cout << "Server is listening on port " << PORT << std::endl;

  while (true) {
    // Accept a new client connection
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket =
        accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientSocket < 0) {
      std::cerr << "Failed to accept client connection" << std::endl;
      continue;
    }

    std::cout << "New client connected!" << std::endl;

    // Enqueue the client handling task to the thread pool
    pool.enqueue([clientSocket] { handleClient(clientSocket); });
  }

  close(serverSocket);
  return 0;
}
