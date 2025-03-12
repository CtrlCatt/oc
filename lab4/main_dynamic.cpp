#include <iostream>
#include <chrono>  // Для замеров времени
#include <windows.h>  // Для LoadLibrary и GetProcAddress

typedef float (*DerivativeFunc)(float, float);
typedef float (*EFunc)(int);

int main() {
    HMODULE hLib = LoadLibraryW(L"libmathfuncs.dll");
    if (!hLib) {
        std::cerr << "Failed to load the library." << std::endl;
        return 1;
    }

    auto Derivative = (DerivativeFunc)GetProcAddress(hLib, "Derivative");
    auto E = (EFunc)GetProcAddress(hLib, "E");

    if (!Derivative || !E) {
        std::cerr << "Failed to retrieve function addresses." << std::endl;
        FreeLibrary(hLib);
        return 1;
    }

    int command;
    while (true) {
        std::cout << "Enter command (1, 2, or 3), or 0 to exit: ";
        std::cin >> command;

        if (command == 0) break;

        if (command == 1) {
            float A, deltaX;
            std::cout << "Enter A and deltaX: ";
            std::cin >> A >> deltaX;

            auto start = std::chrono::high_resolution_clock::now();
            float result = Derivative(A, deltaX);
            auto end = std::chrono::high_resolution_clock::now();

            std::cout << "Derivative of cos(x) at point A: " << result << std::endl;
            std::cout << "Execution time: "
                      << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
                      << " μs" << std::endl;
        } 
        else if (command == 2) {
            int x;
            std::cout << "Enter x for e calculation: ";
            std::cin >> x;

            auto start = std::chrono::high_resolution_clock::now();
            float result = E(x);
            auto end = std::chrono::high_resolution_clock::now();

            std::cout << "Value of e: " << result << std::endl;
            std::cout << "Execution time: "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
                      << " ms" << std::endl;
        } 
        else if (command == 3) {
            const int iterations = 10000000;  // Количество вызовов
            const float A = 1.0f, deltaX = 0.001f;
            const int x = 100;

            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < iterations; ++i) {
                Derivative(A, deltaX);
                E(x);
            }
            auto end = std::chrono::high_resolution_clock::now();

            std::cout << "Executed " << iterations << " calls to Derivative and E." << std::endl;
            std::cout << "Total execution time: "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
                      << " ms" << std::endl;
        } 
        else {
            std::cout << "Invalid command. Try again." << std::endl;
        }
    }

    FreeLibrary(hLib);
    return 0;
}
