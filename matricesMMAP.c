#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <time.h>
#include <math.h>

typedef struct {
    int N, M;
    int **A;
    int **B;
    int *R;         
    int next_cell;  
} TaskData;

int main() {
    int N, M, runs = 100;  
    int num_procs = 4;  

    printf("Ingrese N y M: ");
    scanf("%d %d", &N, &M);

    srand(time(NULL));
    double tiempos[100];

    for (int r = 0; r < runs; r++) {
        clock_t start = clock();

        // Crear matrices A y B
        int **A = malloc(N * sizeof(int *));
        for (int i = 0; i < N; i++) {
            A[i] = malloc(M * sizeof(int));
            for (int j = 0; j < M; j++) A[i][j] = rand() % 5 + 1;
        }

        int **B = malloc(M * sizeof(int *));
        for (int i = 0; i < M; i++) {
            B[i] = malloc(N * sizeof(int));
            for (int j = 0; j < N; j++) B[i][j] = rand() % 5 + 1;
        }

        // R en memoria compartida
        int *R = mmap(NULL, M*M*sizeof(int),
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        for (int i = 0; i < M*M; i++) R[i] = 0;

        // Variable compartida para la próxima celda
        int *next_cell = mmap(NULL, sizeof(int),
                              PROT_READ | PROT_WRITE,
                              MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        *next_cell = 0;

        pid_t pids[num_procs];
        for (int p = 0; p < num_procs; p++) {
            if ((pids[p] = fork()) == 0) {
                while (1) {
                    int idx = __sync_fetch_and_add(next_cell, 1);
                    if (idx >= M*M) break;
                    int i = idx / M;
                    int j = idx % M;
                    int sum = 0;
                    for (int k = 0; k < N; k++) {
                        sum += B[i][k] * A[k][j];
                    }
                    R[i*M + j] = sum;
                }
                _exit(0);
            }
        }

        // Esperar a todos los procesos
        for (int p = 0; p < num_procs; p++) wait(NULL);

        clock_t end = clock();
        tiempos[r] = (double)(end - start) / CLOCKS_PER_SEC;

        // Liberar memoria
        for (int i = 0; i < N; i++) free(A[i]);
        free(A);
        for (int i = 0; i < M; i++) free(B[i]);
        free(B);
        munmap(R, M*M*sizeof(int));
        munmap(next_cell, sizeof(int));
    }

    // Calcular promedio y desviación estándar
    double suma = 0;
    for (int i = 0; i < runs; i++) suma += tiempos[i];
    double promedio = suma / runs;

    double var = 0;
    for (int i = 0; i < runs; i++) var += (tiempos[i]-promedio)*(tiempos[i]-promedio);
    double desviacion = sqrt(var/runs);

    printf("Promedio de tiempo: %f segundos\n", promedio);
    printf("Desviacion estandar: %f segundos\n", desviacion);

    return 0;
}
