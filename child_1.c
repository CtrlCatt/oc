// ChildProcess.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdio.h"
#include "windows.h"
#include "string.h"

int main()
{
    HANDLE readHandle = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE writeHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD readedBytes, writedBytes;

    char str[256];
    // Read
    ReadFile(readHandle, &str, sizeof(str), &readedBytes, NULL);

    strlwr(str);
    str[readedBytes] = '\0';

    // Write
    WriteFile(writeHandle, &str, sizeof(str) + 1, &writedBytes, NULL);

    return 0;
}