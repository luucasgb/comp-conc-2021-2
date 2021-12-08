#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define NTHREADS 2
#define VECTOR_SIZE 10

typedef struct {
    int thread_id;
    int *vector;
} thread_arguments;

void* compute_square(void* arg);
void print_array (int vector[]);

int main(void) {

    
    pthread_t tid[NTHREADS];
    int thread;
    int *vector = malloc(sizeof(int) * VECTOR_SIZE);
    for(int i = 0; i < VECTOR_SIZE; i++){
        *(vector+i) = i;
    }
    thread_arguments *args;

    printf("Estado inicial do vetor de %d elementos:\n", VECTOR_SIZE);
    print_array(vector);
    printf("\n");

    for(thread = 0; thread < NTHREADS; thread++) {
        printf("-Alocando memoria e preenchendo argumentos para a thread %d\n", thread);
        args = malloc(sizeof(thread_arguments));
        if (args == NULL) {
            printf("--ERRO: malloc()\n"); 
            exit(-1);
        }

        args->thread_id = thread;
        args->vector = vector;

        printf("-Criando a thread %d\n", thread);

        if(pthread_create(&tid[thread], NULL, compute_square, (void *) args)) {
            printf("Erro: pthread_create");
            exit(-1);
        }
    }

    for(thread = 0; thread < NTHREADS; thread++) {
        if(pthread_join(tid[thread], NULL)) {
            printf("Erro: pthread_join");
            exit(-1);
        }
    }

    printf("\nEstado final do vetor de %d elementos \n", VECTOR_SIZE);
    print_array(vector);
    printf("\n");

    printf("Terminando execucao\n");

    return 0;
}

void* compute_square(void* arg) {
    
    thread_arguments *args = (thread_arguments*) arg;
    int thread_id = args->thread_id;       
    
    for(int i = thread_id; i < VECTOR_SIZE; i += NTHREADS) {     
        args->vector[i] *= args->vector[i];
    }

    free(args);
    pthread_exit(NULL);
}

void print_array (int vector[]) {
    printf("[");
    for(int i = 0; i < VECTOR_SIZE; i++) {
        printf(" %d ", vector[i]);
    }
    printf("]\n");
}
