#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#pragma comment(lib, "ws2_32.lib")

void receiveMessages(SOCKET serverSocket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(serverSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            std::cout << "Message: " << std::string(buffer, bytesReceived) << std::endl;
        }
    }
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5555);
    InetPtonA(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to server." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Enter username: ";
    std::string username;
    std::cin >> username;
    send(clientSocket, username.c_str(), username.length(), 0);

    std::thread(receiveMessages, clientSocket).detach();

    std::cin.ignore(); // Чтобы убрать лишний Enter
    while (true) {
        std::string command;
        std::getline(std::cin, command);
        command += "\n"; // Добавляем перевод строки
        send(clientSocket, command.c_str(), command.length(), 0);
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
