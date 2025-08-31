#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int main() {
    int N, M, runs = 100;
    printf("Ingrese N y M: ");
    scanf("%d %d", &N, &M);

    srand(time(NULL));

    double tiempos[100];

    for (int r = 0; r < runs; r++) {
        clock_t start = clock();

        // Matriz A: N x M
        int **A = (int **)malloc(N * sizeof(int *));
        for (int i = 0; i < N; i++) {
            A[i] = (int *)malloc(M * sizeof(int));
            for (int j = 0; j < M; j++) {
                A[i][j] = rand() % 5 + 1;
            }
        }

        // Matriz B: M x N
        int **B = (int **)malloc(M * sizeof(int *));
        for (int i = 0; i < M; i++) {
            B[i] = (int *)malloc(N * sizeof(int));
            for (int j = 0; j < N; j++) {
                B[i][j] = rand() % 5 + 1;
            }
        }

        // Matriz R: M x M
        int **R = (int **)malloc(M * sizeof(int *));
        for (int i = 0; i < M; i++) {
            R[i] = (int *)calloc(M, sizeof(int));
        }

        // Multiplicación: R = B * A
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < M; j++) {
                int sum = 0;
                for (int k = 0; k < N; k++) {
                    sum += B[i][k] * A[k][j];
                }
                R[i][j] = sum;
            }
        }

        clock_t end = clock();
        tiempos[r] = (double)(end - start) / CLOCKS_PER_SEC;

        // Liberar memoria
        for (int i = 0; i < N; i++) free(A[i]);
        free(A);

        for (int i = 0; i < M; i++) free(B[i]);
        free(B);

        for (int i = 0; i < M; i++) free(R[i]);
        free(R);
    }

    // Calcular promedio
    double suma = 0;
    for (int i = 0; i < runs; i++) suma += tiempos[i];
    double promedio = suma / runs;

    // Calcular desviación estándar
    double var = 0;
    for (int i = 0; i < runs; i++) {
        var += (tiempos[i] - promedio) * (tiempos[i] - promedio);
    }
    double desviacion = sqrt(var / runs);

    printf("Promedio de tiempo: %f segundos\n", promedio);
    printf("Desviacion estandar: %f segundos\n", desviacion);

    return 0;
}
