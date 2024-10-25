// ChildProcess.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdio.h"
#include "windows.h"

int main()
{
    HANDLE readHandle = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE writeHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD readedBytes, writedBytes;
    char str[256];

    ReadFile(readHandle, &str, sizeof(str), &readedBytes, NULL);

    char res[256];
    for (int i = 0; i < sizeof(str); i++) {
        res[i] = tolower(str[i]);
    }
    strcat(res, "\0");

    WriteFile(writeHandle, &res, sizeof(str), &writedBytes, NULL);

    return 0;
}