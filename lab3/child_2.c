#include "windows.h"
#include "stdio.h"

#define FILE_SIZE 256

// Функция потока для замены пробелов на "_"
DWORD WINAPI replace_spaces_with_underscore(LPVOID lpParam) {
    char *data = (char *)lpParam;
    for (int i = 0; data[i]; i++) {
        if (data[i] == ' ') {
            data[i] = '_';
        }
    }
    return 0;
}
