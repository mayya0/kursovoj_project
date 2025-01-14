#define _CRT_SECURE_NO_WARNINGS
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define M_PI 3.14 // константа PI
#define n 10 // кол-во значений
#define k 1000 //число интервалов

double f1(double x);
double f2(double x);
double f3(double x);

double* mass_shag(double xmin, double dx);
double* mass_rand(double xmin, double xmax);

int write_to_file(const char* filename, double* values, int count, double (*func)(double));

double* find_minimum(double (*func)(double), double* values, int N);  // нахождение минимального значения
double* find_maximum(double (*func)(double), double* values, int N);  // нахождение максимального значения
double* lin_search(double (*func)(double), double target, double* values, int N);  // линейный поиск

int main() {
    setlocale(LC_CTYPE, "RUS"); //локаль

    int choice; //выбор пользователя
    double xmin, dx, xmax, value; // переменные для хранения начального значения, шага, конечного значения и введенного значения
    double (*func)(double) = NULL; // указатель на функцию для вычислений
    double* values = NULL; // указатель на массив значений

    while (1) {
        printf("Выберите функцию:\nf1-1\nf2-2\nf3-3\nДля завершения - 0\n");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            func = f1; //если выбран f1, устанавливается указатель на функцию f1
            break;
        case 2:
            func = f2; //если выбран f2, устанавливается указатель на функцию f2
            break;
        case 3:
            func = f3; //если выбран f3, устанавливается указатель на функцию f3
            break;
        case 0:
            printf("Программа завершена\n"); //если выбран 0, завершение программы
            return 0;
        default:
            printf("Некорректный ввод\n");
            continue;
        }

        printf("Выберите режим:\n1 - Ввод значения\n2 - Генерация значений\n3 - Случайные значения\n"); //меню для выбора режима
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            values = (double*)malloc(sizeof(double)); //выделение памяти для одного значения
            if (values == NULL) { //проверка успешности выделения памяти
                printf("Ошибка выделения памяти\n");
                return 1;
            }
            printf("Введите значение: ");
            scanf("%lf", &value);
            values[0] = value; //сохранение введенного значения в массив values
            break;
        case 2:
            printf("Введите начальное значение (xmin): ");
            scanf("%lf", &xmin);
            printf("Введите шаг (dx): ");
            scanf("%lf", &dx);
            values = mass_shag(xmin, dx); //генерация массива значений
            if (values == NULL) { //проверка успешности выделения памяти
                printf("Ошибка выделения памяти\n");
                return 1;
            }
            break;
        case 3:
            printf("Введите начальное значение (xmin): ");
            scanf("%lf", &xmin);
            printf("Введите конечное значение (xmax): ");
            scanf("%lf", &xmax);
            values = mass_rand(xmin, xmax); //генерация массива случайных значений
            if (values == NULL) { //проверка успешности выделения памяти
                printf("Ошибка выделения памяти\n");
                return 1;
            }
            break;
        default:
            printf("Некорректный ввод\n");
            continue;
        }

        if (write_to_file("output.cvs", values, (choice == 1) ? 1 : n, func) != 0) { //запись результатов в файл и проверка успешности записи
            printf("Ошибка записи в файл\n");
        }

        // Дополнительные операции
        int additional_choice;
        printf("Выберите дополнительную операцию:\n1 - Найти минимальное и максимальное значение\n2 - Линейный поиск\n0 - Выход\n");
        scanf("%d", &additional_choice);

        switch (additional_choice) {
        case 1:
        {
            double* min_values = find_minimum(func, values, (choice == 1) ? 1 : n);
            double* max_values = find_maximum(func, values, (choice == 1) ? 1 : n);
            if (min_values != NULL && max_values != NULL) {
                printf("Минимальное значение: %lf\n", min_values[0]);
                printf("Максимальное значение: %lf\n", max_values[0]);
                free(min_values);
                free(max_values);
            }
            else {
                printf("Ошибка выделения памяти\n");
            }
        }
        break;
        case 2:
        {
            double target;
            printf("Введите целевое значение функции: ");
            scanf("%lf", &target);
            double* result = lin_search(func, target, values, (choice == 1) ? 1 : n);
            if (result != NULL) {
                printf("Найденное значение аргумента: %lf\n", result[0]);
                free(result);
            }
            else {
                printf("Значение не найдено\n");
            }
        }
        break;
        case 0:
            break;
        default:
            printf("Некорректный ввод\n");
            break;
        }

        free(values); //освобождение выделенной памяти для массива значений
    }

    return 0;
}

double f1(double x) {
    return (1 - exp(x - 1)) / (1 + pow(x, 2) * abs(M_PI - sin(x / 3)));
}

double f2(double x) {
    if (x > M_PI) {
        return pow(fabs(3.0 / 7.0 / x), 1.0 / 3);
    }
    else if (x >= -M_PI && x <= M_PI) {
        return 1 - pow(cos(fabs(x)), 3);
    }
    else {
        return exp(-pow(x, 2));
    }
}

double f3(double x) { //интегральный синус
    double h = x / k;//шаг интегрирования
    double sum = 0.5 * (sin(0) + sin(x));//метод трапеций
    for (int i = 1; i < k; i++) {
        sum += sin(i * h);
    }
    return sum * h / x;//метод трапеций, где сумма значений функции умножается на шаг интегрирования h и делится на x
}

// Функция генерации массива значений с заданным шагом
double* mass_shag(double xmin, double dx) {
    double* mass = (double*)malloc(n * sizeof(double)); //выделене памяти для массива значений
    if (mass == NULL) { //проверка успешности выделения памяти
        return NULL;
    }
    for (int i = 0; i < n; i++) { //заполнение массива значениями
        mass[i] = xmin + i * dx;
    }
    return mass; //возвращение массива значений
}

// Функция генерации массива случайных значений
double* mass_rand(double xmin, double xmax) {
    double* array = (double*)malloc(n * sizeof(double)); //выделение памяти для массива значений
    if (array == NULL) { //проверка успешности выделения памяти
        return NULL;
    }
    int count = 0;
    while (count < n) { //цикл для генерации уникальных значений
        double num = xmin + (double)rand() / RAND_MAX * (xmax - xmin); // Генерируем случайное значение
        int f = 1;
        for (int i = 0; i < count; i++) { //проверка уникальности значения
            if (fabs(array[i] - num) < 1e-6) {
                f = 0;
                break;
            }
        }
        if (f) { //если значение уникально, добавление его в массив
            array[count] = num;
            count++;
        }
    }
    return array; //возвращение массива значений
}

// Функция записи результатов в файл
int write_to_file(const char* filename, double* values, int count, double (*func)(double)) {
    FILE* outputFile = fopen(filename, "w"); //открытие файла для записи
    if (outputFile == NULL) { //проерка успешности открытия файла
        return 1; //ошибка открытия файла
    }

    fprintf(outputFile, "Значения для выбранной функции:\n");
    fprintf(outputFile, "| %-10s | %-10s |\n", "x", "f(x)");

    for (int i = 0; i < count; i++) {
        fprintf(outputFile, "| %-10.2lf | %-10.2lf |\n", values[i], func(values[i]));
    }

    fclose(outputFile); //закрытие файла
    return 0; //успешная запись
}

// Функция для нахождения минимального значения функции
double* find_minimum(double (*func)(double), double* values, int N) {
    double* min_values = (double*)malloc(N * sizeof(double));
    if (min_values == NULL) {
        return NULL;
    }
    double min = func(values[0]);
    for (int i = 1; i < N; i++) {
        if (func(values[i]) < min) {
            min = func(values[i]);
        }
    }
    min_values[0] = min;
    return min_values;  // нахождение минимального значения
}

// Функция для нахождения максимального значения функции
double* find_maximum(double (*func)(double), double* values, int N) {
    double* max_values = (double*)malloc(N * sizeof(double));
    if (max_values == NULL) {
        return NULL;
    }
    double max = func(values[0]);
    for (int i = 1; i < N; i++) {
        if (func(values[i]) > max) {
            max = func(values[i]);
        }
    }
    max_values[0] = max;
    return max_values;  // нахождение максимального значения
}

// Функция для линейного поиска значения аргумента при заданном значении функции
double* lin_search(double (*func)(double), double target, double* values, int N) {
    double* result = (double*)malloc(sizeof(double));
    if (result == NULL) {
        return NULL;
    }
    for (int i = 0; i < N; i++) {
        if (fabs(func(values[i]) - target) < 1e-2) {  // допустимая погрешность
            result[0] = values[i];
            return result;
        }
    }
    free(result);
    return NULL;  // если не найдено
}
