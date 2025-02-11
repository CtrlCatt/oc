#include "windows.h"
#include "stdio.h"
#include "ctype.h"

#define FILE_SIZE 256

// Функция потока для перевода текста в нижний регистр
DWORD WINAPI process_to_lower(LPVOID lpParam) {
    char *data = (char *)lpParam;
    for (int i = 0; data[i]; i++) {
        data[i] = tolower((unsigned char)data[i]);
    }
    return 0;
}

