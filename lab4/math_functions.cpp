#include <cmath>  // Для cos() и exp()
#include "math_functions.h"

// Рассчёт производной функции cos(x)
float Derivative(float A, float deltaX) {
    return (cos(A + deltaX) - cos(A - deltaX)) / (2 * deltaX);
}

// Рассчёт числа e (основание натурального логарифма) двумя методами
float E(int x) {
    float result = 1.0f;
    float term = 1.0f;
    for (int n = 1; n <= x; ++n) {
        term /= n;
        result += term;
    }
    return result;
}
