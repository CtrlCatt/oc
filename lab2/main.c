#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TOTAL_POINTS 1000000  // Общее количество точек для всех потоков

// Глобальные переменные
double pointsInCircle = 0.0;       // Количество точек внутри окружности
int pointsPerThread;          // Количество точек на один поток
CRITICAL_SECTION cs;          // Критическая секция для синхронизации
double radius;                // Радиус окр.

// Функция, которую будет выполнять каждый поток
DWORD WINAPI MonteCarloThread(LPVOID lpParam) {
    double countInCircle = 0.0;
    int i;
    double x, y;

    for (i = 0; i < pointsPerThread; i++) {
        x = ((double)rand() / RAND_MAX) * (radius*2.0) - radius;
        y = ((double)rand() / RAND_MAX) * (radius*2.0) - radius;
        if (x * x + y * y <= radius*radius) {
            countInCircle++;
        }
    }

    EnterCriticalSection(&cs);
    pointsInCircle += countInCircle;
    LeaveCriticalSection(&cs);

    return 0;
}

int main() {

    double a;
    int b;
    printf("enter radius\n");
    scanf("%lf", &a);
    printf("and number of threads\n");
    scanf("%d", &b);
    

    radius = a;
    int maxThreads = b;
    pointsPerThread = TOTAL_POINTS / maxThreads;

    HANDLE *threads = malloc(maxThreads * sizeof(HANDLE));
    InitializeCriticalSection(&cs);

    for (int i = 0; i < maxThreads; i++) {
        threads[i] = CreateThread(NULL, 0, MonteCarloThread, NULL, 0, NULL);
    }

    WaitForMultipleObjects(maxThreads, threads, TRUE, INFINITE);

    double estimatedArea =(4.0 * (radius * radius)) * (pointsInCircle / TOTAL_POINTS);
    printf("Estimated area of the circle: %lf\n", estimatedArea);

    for (int i = 0; i < maxThreads; i++) {
        CloseHandle(threads[i]);
    }
    DeleteCriticalSection(&cs);
    free(threads);

    return 0;
}