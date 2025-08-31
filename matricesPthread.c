#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

typedef struct {
    int N, M;
    int **A;
    int **B;
    int **R;
    int next_row;
    int next_col;
    pthread_mutex_t mutex;
} TaskData;

TaskData task;

void* worker(void* arg) {
    while (1) {
        int i, j;

        // Tomar la siguiente celda
        pthread_mutex_lock(&task.mutex);
        if (task.next_row >= task.M) {
            pthread_mutex_unlock(&task.mutex);
            break; 
        }
        i = task.next_row;
        j = task.next_col;

        // Actualizar la posición para el siguiente hilo
        task.next_col++;
        if (task.next_col >= task.M) {
            task.next_col = 0;
            task.next_row++;
        }
        pthread_mutex_unlock(&task.mutex);

        int sum = 0;
        for (int k = 0; k < task.N; k++) {
            sum += task.B[i][k] * task.A[k][j];
        }
        task.R[i][j] = sum;
    }
    return NULL;
}

int main() {
    int N, M, runs = 100;
    int MAX_THREADS = 4; 
    printf("Ingrese N y M: ");
    scanf("%d %d", &N, &M);

    srand(time(NULL));
    double tiempos[100];

    for (int r = 0; r < runs; r++) {
        clock_t start = clock();

        // Generar A: N x M
        int **A = malloc(N * sizeof(int *));
        for (int i = 0; i < N; i++) {
            A[i] = malloc(M * sizeof(int));
            for (int j = 0; j < M; j++) A[i][j] = rand() % 5 + 1;
        }

        // Generar B: M x N
        int **B = malloc(M * sizeof(int *));
        for (int i = 0; i < M; i++) {
            B[i] = malloc(N * sizeof(int));
            for (int j = 0; j < N; j++) B[i][j] = rand() % 5 + 1;
        }

        // R: M x M
        int **R = malloc(M * sizeof(int *));
        for (int i = 0; i < M; i++) {
            R[i] = calloc(M, sizeof(int));
        }

        task.N = N;
        task.M = M;
        task.A = A;
        task.B = B;
        task.R = R;
        task.next_row = 0;
        task.next_col = 0;
        pthread_mutex_init(&task.mutex, NULL);

        pthread_t threads[MAX_THREADS];
        for (int t = 0; t < MAX_THREADS; t++) {
            pthread_create(&threads[t], NULL, worker, NULL);
        }

        // Esperar hilos
        for (int t = 0; t < MAX_THREADS; t++) {
            pthread_join(threads[t], NULL);
        }

        pthread_mutex_destroy(&task.mutex);

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

    // Calcular promedio y desviación estándar
    double suma = 0;
    for (int i = 0; i < runs; i++) suma += tiempos[i];
    double promedio = suma / runs;

    double var = 0;
    for (int i = 0; i < runs; i++) var += (tiempos[i] - promedio) * (tiempos[i] - promedio);
    double desviacion = sqrt(var / runs);

    printf("Promedio de tiempo: %f segundos\n", promedio);
    printf("Desviacion estandar: %f segundos\n", desviacion);

    return 0;
}
