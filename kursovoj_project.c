#define _CRT_SECURE_NO_WARNINGS
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define M_PI 3.14 // ��������� PI
#define n 10 // ���-�� ��������
#define k 1000 //����� ����������

double f1(double x);
double f2(double x);
double f3(double x);

double* mass_shag(double xmin, double dx);
double* mass_rand(double xmin, double xmax);

int write_to_file(const char* filename, double* values, int count, double (*func)(double));

double* find_minimum(double (*func)(double), double* values, int N);  // ���������� ������������ ��������
double* find_maximum(double (*func)(double), double* values, int N);  // ���������� ������������� ��������
double* lin_search(double (*func)(double), double target, double xmin, double xmax, double dx);  // �������� �����

int main() {
    setlocale(LC_CTYPE, "RUS"); //������

    int choice; //����� ������������
    double xmin, dx, xmax, value; // ���������� ��� �������� ���������� ��������, ����, ��������� �������� � ���������� ��������
    double (*func)(double) = NULL; // ��������� �� ������� ��� ����������
    double* values = NULL; // ��������� �� ������ ��������

    while (1) {
        printf("�������� �������:\nf1-1\nf2-2\nf3-3\n��� ���������� - 0\n");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            func = f1; //���� ������ f1, ��������������� ��������� �� ������� f1
            break;
        case 2:
            func = f2; //���� ������ f2, ��������������� ��������� �� ������� f2
            break;
        case 3:
            func = f3; //���� ������ f3, ��������������� ��������� �� ������� f3
            break;
        case 0:
            printf("��������� ���������\n"); //���� ������ 0, ���������� ���������
            return 0;
        default:
            printf("������������ ����\n");
            continue;
        }

        printf("�������� �����:\n1 - ���� ��������\n2 - ��������� ��������\n3 - ��������� ��������\n"); //���� ��� ������ ������
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            values = (double*)malloc(sizeof(double)); //��������� ������ ��� ������ ��������
            if (values == NULL) { //�������� ���������� ��������� ������
                printf("������ ��������� ������\n");
                return 1;
            }
            printf("������� ��������: ");
            scanf("%lf", &value);
            values[0] = value; //���������� ���������� �������� � ������ values
            break;
        case 2:
            printf("������� ��������� �������� (xmin): ");
            scanf("%lf", &xmin);
            printf("������� ��� (dx): ");
            scanf("%lf", &dx);
            values = mass_shag(xmin, dx); //��������� ������� ��������
            if (values == NULL) { //�������� ���������� ��������� ������
                printf("������ ��������� ������\n");
                return 1;
            }
            break;
        case 3:
            printf("������� ��������� �������� (xmin): ");
            scanf("%lf", &xmin);
            printf("������� �������� �������� (xmax): ");
            scanf("%lf", &xmax);
            values = mass_rand(xmin, xmax); //��������� ������� ��������� ��������
            if (values == NULL) { //�������� ���������� ��������� ������
                printf("������ ��������� ������\n");
                return 1;
            }
            break;
        default:
            printf("������������ ����\n");
            continue;
        }

        if (write_to_file("output.cvs", values, (choice == 1) ? 1 : n, func) != 0) { //������ ����������� � ���� � �������� ���������� ������
            printf("������ ������ � ����\n");
        }

        // �������������� ��������
        int additional_choice;
        printf("�������� �������������� ��������:\n1 - ����� ����������� � ������������ ��������\n2 - �������� �����\n0 - �����\n");
        scanf("%d", &additional_choice);

        switch (additional_choice) {
        case 1:
        {
            double* min_values = find_minimum(func, values, (choice == 1) ? 1 : n);
            double* max_values = find_maximum(func, values, (choice == 1) ? 1 : n);
            if (min_values != NULL && max_values != NULL) {
                printf("����������� ��������: %lf\n", min_values[0]);
                printf("������������ ��������: %lf\n", max_values[0]);
                free(min_values);
                free(max_values);
            }
            else {
                printf("������ ��������� ������\n");
            }
        }
        break;
        case 2:
        {
            double target;
            printf("������� ������� �������� �������: ");
            scanf("%lf", &target);
            double* result = lin_search(func, target, xmin, xmax, 0.01);
            if (result != NULL) {
                printf("��������� �������� ���������: %lf\n", result[0]);
                free(result);
            }
            else {
                printf("�������� �� �������\n");
            }
        }
        break;
        case 0:
            break;
        default:
            printf("������������ ����\n");
            break;
        }

        free(values); //������������ ���������� ������ ��� ������� ��������
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

double f3(double x) { //������������ �����
    double h = x / k;//��� ��������������
    double sum = 0.5 * (sin(0) + sin(x));//����� ��������
    for (int i = 1; i < k; i++) {
        sum += sin(i * h);
    }
    return sum * h / x;//����� ��������, ��� ����� �������� ������� ���������� �� ��� �������������� h � ������� �� x
}

// ������� ��������� ������� �������� � �������� �����
double* mass_shag(double xmin, double dx) {
    double* mass = (double*)malloc(n * sizeof(double)); //�������� ������ ��� ������� ��������
    if (mass == NULL) { //�������� ���������� ��������� ������
        return NULL;
    }
    for (int i = 0; i < n; i++) { //���������� ������� ����������
        mass[i] = xmin + i * dx;
    }
    return mass; //����������� ������� ��������
}

// ������� ��������� ������� ��������� ��������
double* mass_rand(double xmin, double xmax) {
    double* array = (double*)malloc(n * sizeof(double)); //��������� ������ ��� ������� ��������
    if (array == NULL) { //�������� ���������� ��������� ������
        return NULL;
    }
    int count = 0;
    while (count < n) { //���� ��� ��������� ���������� ��������
        double num = xmin + (double)rand() / RAND_MAX * (xmax - xmin); // ���������� ��������� ��������
        int f = 1;
        for (int i = 0; i < count; i++) { //�������� ������������ ��������
            if (fabs(array[i] - num) < 1e-6) {
                f = 0;
                break;
            }
        }
        if (f) { //���� �������� ���������, ���������� ��� � ������
            array[count] = num;
            count++;
        }
    }
    return array; //����������� ������� ��������
}

// ������� ������ ����������� � ����
int write_to_file(const char* filename, double* values, int count, double (*func)(double)) {
    FILE* outputFile = fopen(filename, "w"); //�������� ����� ��� ������
    if (outputFile == NULL) { //������� ���������� �������� �����
        return 1; //������ �������� �����
    }

    fprintf(outputFile, "�������� ��� ��������� �������:\n");
    fprintf(outputFile, "| %-10s | %-10s |\n", "x", "f(x)");

    for (int i = 0; i < count; i++) {
        fprintf(outputFile, "| %-10.2lf | %-10.2lf |\n", values[i], func(values[i]));
    }

    fclose(outputFile); //�������� �����
    return 0; //�������� ������
}

// ������� ��� ���������� ������������ �������� �������
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
    return min_values;  // ���������� ������������ ��������
}

// ������� ��� ���������� ������������� �������� �������
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
    return max_values;  // ���������� ������������� ��������
}

// ������� ��� ��������� ������ �������� ��������� ��� �������� �������� �������
double* lin_search(double (*func)(double), double target, double xmin, double xmax, double dx) {
    double* result = (double*)malloc(sizeof(double));
    if (result == NULL) {
        return NULL;
    }
    for (double x = xmin; x <= xmax; x += dx) {
        if (fabs(func(x) - target) < 1e-2) {  // ���������� �����������
            result[0] = x;
            return result;
        }
    }
    free(result);
    return NULL;  // ���� �� �������
}
