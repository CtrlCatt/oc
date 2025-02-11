#include "windows.h"
#include "stdio.h"
#include "string.h"

#define FILE_SIZE 256

//ОБЪЯВЛЕНИЕ Ф ЦИЙ ПОТОКОВ
DWORD WINAPI process_to_lower(LPVOID lpParam);
DWORD WINAPI replace_spaces_with_underscore(LPVOID lpParam);

int main() {
    HANDLE hFile, hMapFile; // hFile - дескриптор, указывающий на файл на диске. hMapFile - загружает данные в память.
    LPVOID pBuf; // pBuf - указывает на область памяти, ранее выделенной под созданный файл.

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
        printf("Could not create file: %lu\n", GetLastError());
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
        printf("Could not create file mapping object: %lu\n", GetLastError());
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
        printf("Could not map view of file: %lu\n", GetLastError());
        CloseHandle(hMapFile);
        CloseHandle(hFile);
        return 1;
    }

    // Input data
    printf("Enter string: ");
    char input[FILE_SIZE];
    fgets(input, FILE_SIZE, stdin);
    strcpy((char *)pBuf, input); // strcpy требует, чтобы её первый аргумент был указателем на строку (char *). ИЗГНАЛ СКВЕРНУ!!!!!!

    // Start child threads instead of processes
    HANDLE hThread1 = CreateThread(NULL, 0, process_to_lower, pBuf, 0, NULL);
    HANDLE hThread2 = CreateThread(NULL, 0, replace_spaces_with_underscore, pBuf, 0, NULL);

    if (hThread1 == NULL || hThread2 == NULL) {
        printf("Failed to create threads: %lu\n", GetLastError());
        return 1;
    }

    // Wait for threads to finish
    WaitForSingleObject(hThread1, INFINITE);
    WaitForSingleObject(hThread2, INFINITE);

    CloseHandle(hThread1);
    CloseHandle(hThread2);

    // Output result
    printf("Result: %s\n", (char *)pBuf);

    // Clean up
    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
    CloseHandle(hFile);

    return 0;
}
