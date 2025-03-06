#include <iostream>
#include <unordered_map>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

struct Client {
    std::string username;
    SOCKET socket;
};

std::unordered_map<std::string, Client> clients;

void sendMessage(const std::string& recipient, const std::string& message) {
    if (clients.find(recipient) != clients.end()) {
        send(clients[recipient].socket, message.c_str(), message.length(), 0);
    }
}

void handleClient(SOCKET clientSocket) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived <= 0) {
        closesocket(clientSocket);
        return;
    }

    std::string username(buffer, bytesReceived);
    clients[username] = {username, clientSocket};
    std::cout << username << " connected." << std::endl;

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            std::cout << username << " disconnected." << std::endl;
            closesocket(clientSocket);
            clients.erase(username);
            return;
        }

        std::string command(buffer, bytesReceived);
        std::istringstream iss(command);
        std::string action, recipient, message;
        iss >> action >> recipient;
        std::getline(iss, message);
        message = message.substr(1); // Убираем пробел перед сообщением

        if (action == "send" && !recipient.empty() && !message.empty()) {
            sendMessage(recipient, username + ": " + message);
        }
    }
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5555);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, SOMAXCONN);

    std::cout << "Server started on port 5555..." << std::endl;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        std::thread(handleClient, clientSocket).detach();
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
