#include "stdio.h"
#include "windows.h"

int main() {
    char buffer[256];
    DWORD dwRead, dwWritten;

    // Read from standard input
    if (!ReadFile(GetStdHandle(STD_INPUT_HANDLE), &buffer, sizeof(buffer), &dwRead, NULL))
        return -1;

    for (int i = 0; i < sizeof(buffer); i++)
    {
        if (buffer[i] == ' ')
            buffer[i] = '_';
        
    };
    buffer[dwRead] = '\0';
    // Write to standard output
    if (!WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), &buffer, sizeof(buffer) + 1, &dwWritten, NULL))
        return -1;

    return 0;
}