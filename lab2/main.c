#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TOTAL_POINTS 1000000  // Общее количество точек для всех потоков

double pointsInCircle = 0.0;
int pointsPerThread;
CRITICAL_SECTION cs;
double radius;

// Функция для потоков
DWORD WINAPI MonteCarloThread(LPVOID lpParam) {
    double countInCircle = 0.0;
    for (int i = 0; i < pointsPerThread; i++) {
        double x = ((double)rand() / RAND_MAX) * (radius * 2.0) - radius;
        double y = ((double)rand() / RAND_MAX) * (radius * 2.0) - radius;
        if (x * x + y * y <= radius * radius) {
            countInCircle++;
        }
    }

    EnterCriticalSection(&cs);
    pointsInCircle += countInCircle;
    LeaveCriticalSection(&cs);

    return 0;
}

// Функция измерения времени выполнения
double measureExecutionTime(int numThreads) {
    pointsInCircle = 0.0;
    pointsPerThread = TOTAL_POINTS / numThreads;

    HANDLE *threads = (HANDLE*)malloc(numThreads * sizeof(HANDLE));
    InitializeCriticalSection(&cs);

    LARGE_INTEGER start, end, freq;
    QueryPerformanceFrequency(&freq); // Частота таймера
    QueryPerformanceCounter(&start);  // Старт

    for (int i = 0; i < numThreads; i++) {
        threads[i] = CreateThread(NULL, 0, MonteCarloThread, NULL, 0, NULL);
    }
    WaitForMultipleObjects(numThreads, threads, TRUE, INFINITE);
    
    QueryPerformanceCounter(&end);  // Конец

    for (int i = 0; i < numThreads; i++) {
        CloseHandle(threads[i]);
    }
    DeleteCriticalSection(&cs);
    free(threads);

    return (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;  // Время в секундах
}

int main() {
    printf("Enter radius: ");
    scanf("%lf", &radius);

    printf("\nThreads\tExecution Time (s)\tSpeedup\n");

    double time_1 = measureExecutionTime(1); // Время для 1 потока

    for (int numThreads = 1; numThreads <= 16; numThreads++) {
        double time_n = measureExecutionTime(numThreads);
        double speedup = time_1 / time_n;

        printf("%d\t%f\t\t%f\n", numThreads, time_n, speedup);
    }

    return 0;
}