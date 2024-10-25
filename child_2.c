#include "stdio.h"
#include "windows.h"

int main() {
    HANDLE readHandle = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE writeHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD readedBytes, writedBytes;

    char str[256];
    ReadFile(readHandle, &str, sizeof(str), &readedBytes, NULL);

    for (int i = 0; i < sizeof(str); i++)
    {
        if (str[i] == ' ')
            str[i] = '_';
        
    };
    strcat(str, "\0");
    // Write to standard output
    WriteFile(writeHandle, &str, sizeof(str), &writedBytes, NULL);

    return 0;
}