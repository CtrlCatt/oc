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

void workerProcess(int id) {
    std::map<std::string, int> data;
    std::string command;
    std::cout << "Worker node " << id << " started." << std::endl;
    std::cout.flush();

    while (true) {
        std::getline(std::cin, command);
        if (command.rfind("exec", 0) == 0) {
            char name[MSG_SIZE];
            int value;
            int args = sscanf(command.c_str(), "exec %s %d", name, &value);
            if (args == 2) {
                data[name] = value;
                std::cout << "Ok:" << id << " Stored: " << name << " = " << value << std::endl;
                std::cout.flush();
            } else if (args == 1) {
                if (data.find(name) != data.end()) {
                    std::cout << "Ok:" << id << " Retrieved: " << name << " = " << data[name] << std::endl;
                } else {
                    std::cout << "Ok:" << id << ": '" << name << "' not found" << std::endl;
                }
                std::cout.flush();
            } else {
                std::cout << "Error: Invalid exec command format" << std::endl;
                std::cout.flush();
            }
        } else if (command == "exit") {
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        int id = std::stoi(argv[1]);
        workerProcess(id);
        return 0;
    }
    std::cout << "Error: Worker must be started with an ID" << std::endl;
    return 1;
}
