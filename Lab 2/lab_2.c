#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <time.h>
#include "timer.h"

#define MAX_VALUE 10
#define MIN_VALUE 1

typedef struct {
    int id, size, row;
    int nthreads;
    int* matrixA;
    int* matrixB;
    int* matrixC;
} thread_arguments;

int random_number(int min, int max);
int offset(int size, int row, int column);
int* allocate_space_to_matrix(int dimension);
void* multiply_matrix(void* arg);
void populate_matrix(int* matrix, int dimension);
void print_matrix(int* matrix, int dimension);
void free_matrix(int *matrix);
void multiply(int matrix_size, int MAX_THREADS);
void multiplica_matrizes(int* matriz_um, int* matriz_dois, int *resultado_sequencial, int tamanho);

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if(argc < 3) {
        printf("Faltam argumentos \n");
        return 1;
    }
    int matrix_size = atoi(argv[1]);
    int MAX_THREADS = atoi(argv[2]);
    
    multiply(matrix_size, MAX_THREADS);
    
    return 0;
}

void multiplica_matrizes(int* matriz_um, int* matriz_dois, int *resultado_sequencial, int tamanho) {
    for(int i=0; i<tamanho; i++) {
        for(int j=0; j<tamanho; j++) {
            for(int k=0; k<tamanho; k++) {
                resultado_sequencial[i*tamanho+j] += matriz_um[i*tamanho+k]*matriz_dois[k*tamanho+j];
            }
        }
    }
}

int *allocate_space_to_matrix(int dimension) {
    int *matrix = (int*)malloc(dimension * dimension * sizeof(int));
    if(matrix == NULL) {
        printf("Erro na alocação de espaço da matrix");
        exit(-1);
    }
    return matrix;
}

void populate_matrix(int* matrix, int dimension) {
    int i,j;
    for(i = 0; i < dimension; i++)
        for(j = 0; j < dimension; j++)
            *(matrix + i*dimension+j) = random_number(MIN_VALUE, MAX_VALUE);
}

void print_matrix(int* matrix, int dimension) {
    int i,j;
    for(i = 0; i < dimension; i++){
        for(j = 0; j < dimension; j++) {
            printf("%d ", *(matrix + i*dimension+j));
        }
        printf("\t\n");
    }
}

int offset(int size, int row, int column) {
    return size * row + column;
}

void* multiply_matrix(void* arg) {
    thread_arguments *args = (thread_arguments*) arg;

    for(int row = args->row; row < args->size; row+= args->nthreads) {
        for(int col = 0; col < args->size; col++) {
            for(int idx = 0; idx < args->size; idx++) {        
                int x = args->matrixA[offset(args->size, row, idx)];
                int y = args->matrixB[offset(args->size, idx, col)];
                int *z = &args->matrixC[offset(args->size, row, col)];
                *z += x * y;
                
            }
        }
    }
    pthread_exit(NULL);
}

int random_number(int min, int max) {
    int random = rand();
    return random % (max - min + 1 ) + min;
}

void free_matrix(int *matrix) {
    free(matrix);
}

void multiply(int matrix_size, int MAX_THREADS) {
    int row, nthreads;
    double start, finish, elapsed;
    double total_execution_time = 0.0;

    printf("Execução com dimensão %d e %d threads \n", matrix_size, MAX_THREADS);
    
    int* matrixA = allocate_space_to_matrix(matrix_size);
    populate_matrix(matrixA, matrix_size);
    
    int* matrixB = allocate_space_to_matrix(matrix_size);
    populate_matrix(matrixB, matrix_size);
    
    //resultado concorrente
    int* matrixC = allocate_space_to_matrix(matrix_size);
    
    //resultado sequencial
    int* matriz_sequencial = allocate_space_to_matrix(matrix_size);

    GET_TIME(start)
    multiplica_matrizes(matrixA, matrixB, matriz_sequencial, matrix_size);
    GET_TIME(finish);
    elapsed = finish - start;
    printf("Tempo total sequencial: %lf segundos \n", elapsed);

    GET_TIME(start);
    pthread_t *threads = (pthread_t*) malloc(MAX_THREADS * sizeof(pthread_t)) ;    
    thread_arguments *args = (thread_arguments*) malloc(MAX_THREADS * sizeof(thread_arguments));
    if (args == NULL) {
        printf("--ERRO: malloc()\n"); 
        exit(-1);
    }

    nthreads = 0;
    for(row = 0; row < MAX_THREADS; row++) {        
        args[nthreads].id = nthreads;            
        args[nthreads].nthreads = MAX_THREADS;            
        args[nthreads].size = matrix_size;            
        args[nthreads].row = row;
        args[nthreads].matrixA = matrixA;
        args[nthreads].matrixB = matrixB;
        args[nthreads].matrixC = matrixC;
        nthreads++;
    }

    nthreads = 0;
    for(row = 0; row < MAX_THREADS; row++) {
        if(pthread_create(&threads[nthreads], NULL, multiply_matrix, (void*)(&args[nthreads]))){
            printf("--ERRO: pthread_create()\n"); 
            exit(-1);
        }
        nthreads += 1;
    }

    // Aguarda a finalização das threads
    for(int i = 0; i < MAX_THREADS ; i++) {
        if(pthread_join(threads[i], NULL)) {
            printf("Erro: pthread_join");
            exit(-1);
        }
    }

    free(args);
    free(matrixA);
    free(matrixB);
    free(matrixC);
    GET_TIME(finish);
    
    elapsed = finish - start;
    // printf("Liberação de recursos: %lf segundos \n", elapsed);
    total_execution_time += elapsed;

    printf("Tempo total conconrrente: %lf \n\n", total_execution_time);
}