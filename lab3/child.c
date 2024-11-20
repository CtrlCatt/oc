#include "windows.h"
#include "stdio.h"
#include "ctype.h"

#define FILE_SIZE 256

int main() {
    HANDLE hMapFile;
    LPVOID pBuf;

    hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        TEXT("SharedMemory"));

    if (hMapFile == NULL) {
        printf("Could not open file mapping object: %d\n", GetLastError());
        return 1;
    }

    pBuf = MapViewOfFile(
        hMapFile,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        FILE_SIZE);

    if (pBuf == NULL) {
        printf("Could not map view of file: %d\n", GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }

    char *data = (char *)pBuf;
    for (int i = 0; data[i]; i++) {
        data[i] = tolower(data[i]);
    }

    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);

    return 0;
}
