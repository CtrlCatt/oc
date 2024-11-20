#include "windows.h"
#include "stdio.h"

#define FILE_SIZE 256

int main() {
    HANDLE hFile, hMapFile; // hFile - дескриптор, указывающий на файл на диске. hMapFile - загружает данные в память.
    LPVOID pBuf; // pBuf - указыват на область памяти, ранее выделенной под созданный файл.

    // Create file mapping
    hFile = CreateFile(
        TEXT("shared_file"),             // Имя файла
        GENERIC_READ | GENERIC_WRITE,    // Режим: чтение и запись
        0,                               // Отключённый доступ для других процессов
        NULL,                            // Без защиты
        CREATE_ALWAYS,                   // Всегда создавать новый файл
        FILE_ATTRIBUTE_NORMAL,           // Обычные атрибуты
        NULL);                           // Без шаблонного файла

    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Could not create file: %d\n", GetLastError());
        return 1;
    }

    hMapFile = CreateFileMapping(
        hFile,                          // Файл, который нужно отобразить
        NULL,                           // Защита по умолчанию
        PAGE_READWRITE,                 // Разрешение на чтение и запись
        0,                              // Старшие 32 бита размера
        FILE_SIZE,                      // Размер файла в байтах (256)
        TEXT("SharedMemory"));          // Имя отображаемого файла

    if (hMapFile == NULL) {
        printf("Could not create file mapping object: %d\n", GetLastError());
        CloseHandle(hFile);
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
        CloseHandle(hFile);
        return 1;
    }

    // Input data
    printf("Enter string: ");
    char input[FILE_SIZE];
    fgets(input, FILE_SIZE, stdin);
    strcpy((char *)pBuf, input); // strcpy требует, чтобы её первый аргумент был указателем на строку (char *). ИЗГНАЛ СКВЕРНУ!!!!!!

    // Start child processes
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi1, pi2;
    si.cb = sizeof(si);

    CreateProcess(
        TEXT("child.exe"), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi1);

    WaitForSingleObject(pi1.hProcess, INFINITE);

    CreateProcess(
        TEXT("child_2.exe"), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi2);

    WaitForSingleObject(pi2.hProcess, INFINITE);

    // Output result
    printf("Result: %s\n", (char *)pBuf);

    // Clean up
    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
    CloseHandle(hFile);
    CloseHandle(pi1.hProcess);
    CloseHandle(pi1.hThread);
    CloseHandle(pi2.hProcess);
    CloseHandle(pi2.hThread);

    return 0;
}
