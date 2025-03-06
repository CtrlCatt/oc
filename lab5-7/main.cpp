#include <iostream>
#include <map>
#include <vector>
#include <windows.h>
#include <string>

#define MSG_SIZE 256

struct Message {
    int id;
    char text[MSG_SIZE];
};

class Node {
public:
    int id;
    HANDLE process;
    HANDLE inputWrite;
    HANDLE outputRead;
    bool available;
    std::vector<int> children;

    Node() : id(-1), process(NULL), inputWrite(NULL), outputRead(NULL), available(false) {} // Конструктор по умолчанию
    Node(int id, HANDLE process, HANDLE inputWrite, HANDLE outputRead) : id(id), process(process), inputWrite(inputWrite), outputRead(outputRead), available(true) {}
};

class Controller {
private:
    std::map<int, Node> nodes;

public:
    void createNode(int id, int parentId) {
        if (nodes.find(id) != nodes.end()) {
            std::cout << "Error: Already exists" << std::endl;
            return;
        }

        if (parentId != -1 && nodes.find(parentId) == nodes.end()) {
            std::cout << "Error: Parent not found" << std::endl;
            return;
        }

        STARTUPINFO si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        HANDLE inputRead, inputWrite;
        HANDLE outputRead, outputWrite;
        SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
        CreatePipe(&inputRead, &inputWrite, &sa, 0);
        CreatePipe(&outputRead, &outputWrite, &sa, 0);
        SetHandleInformation(inputWrite, HANDLE_FLAG_INHERIT, 0);
        SetHandleInformation(outputRead, HANDLE_FLAG_INHERIT, 0);
        si.hStdInput = inputRead;
        si.hStdOutput = outputWrite;
        si.dwFlags |= STARTF_USESTDHANDLES;

        std::string cmd = "worker.exe " + std::to_string(id);
        if (!CreateProcess(NULL, &cmd[0], NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
            std::cout << "Error: Could not create process" << std::endl;
            return;
        }

        nodes[id] = Node(id, pi.hProcess, inputWrite, outputRead);
        if (parentId != -1) {
            nodes[parentId].children.push_back(id);
        }
        std::cout << "Ok: " << pi.dwProcessId << std::endl;
    }

    void executeCommand(int id, const std::string& command) {
        if (nodes.find(id) == nodes.end()) {
            std::cout << "Error: " << id << ": Not found" << std::endl;
            return;
        }

        if (!nodes[id].available) {
            std::cout << "Error: " << id << ": Node is unavailable" << std::endl;
            return;
        }

        std::string fullCommand = command + "\n";
        DWORD written;
        WriteFile(nodes[id].inputWrite, fullCommand.c_str(), fullCommand.size(), &written, NULL);
        
        char buffer[MSG_SIZE];
        DWORD bytesRead;
        if (ReadFile(nodes[id].outputRead, buffer, sizeof(buffer), &bytesRead, NULL)) {
            buffer[bytesRead] = '\0';
            std::cout << buffer;
        }
    }

    void checkAvailability() {
        std::cout << "Node Status: " << std::endl;
        for (auto &[id, node] : nodes) {
            DWORD exitCode;
            if (GetExitCodeProcess(node.process, &exitCode) && exitCode == STILL_ACTIVE) {
                node.available = true;
                std::cout << "Node " << id << " is alive." << std::endl;
            } else {
                node.available = false;
                std::cout << "Node " << id << " is unavailable." << std::endl;
            }
        }
    }

    void terminateAllNodes() {
        std::cout << "Terminating all worker nodes..." << std::endl;
        for (auto &[id, node] : nodes) {
            TerminateProcess(node.process, 0);
            CloseHandle(node.process);
            CloseHandle(node.inputWrite);
            CloseHandle(node.outputRead);
            std::cout << "Node " << id << " terminated." << std::endl;
        }
        nodes.clear();
    }
};

void printHelp() {
    std::cout << "Available commands:" << std::endl;
    std::cout << "  create <id> <parent_id> - Create a new node" << std::endl;
    std::cout << "  exec <id> <name> [value] - Store or retrieve a value in a node" << std::endl;
    std::cout << "  pingall - Check the status of all nodes" << std::endl;
    std::cout << "  q - Quit the program and terminate all nodes" << std::endl;
    std::cout << "  help - Show available commands" << std::endl;
}

int main() {
    Controller controller;
    std::string command;

    std::cout << "Distributed System Controller Started" << std::endl;
    printHelp();

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);

        if (command.rfind("create", 0) == 0) {
            int id, parent;
            sscanf(command.c_str(), "create %d %d", &id, &parent);
            controller.createNode(id, parent);
        } else if (command.rfind("exec", 0) == 0) {
            int id;
            char name[MSG_SIZE];
            int value;
            int args = sscanf(command.c_str(), "exec %d %s %d", &id, name, &value);
            if (args == 3) {
                controller.executeCommand(id, "exec " + std::string(name) + " " + std::to_string(value));
            } else if (args == 2) {
                controller.executeCommand(id, "exec " + std::string(name));
            } else {
                std::cout << "Error: Invalid exec command format" << std::endl;
            }
        } else if (command == "pingall") {
            controller.checkAvailability();
        } else if (command == "q") {
            std::cout << "Shutting down..." << std::endl;
            controller.terminateAllNodes();
            break;
        } else if (command == "help") {
            printHelp();
        } else {
            std::cout << "Unknown command. Type 'help' for available commands." << std::endl;
        }
    }
    return 0;
}