#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "timer.h"

// dimensao do vetor de entrada
long long int dim;

// numero de threads
int nthreads;

// vetor de entrada com dimensao = dim
double *vetor;

// limite superior
int limiteSuperior;

// limite inferior
int limiteInferior;

//fluxo das threads
void * tarefa (void * arg) {
    // id da thread
    long int id = (long int) arg;

    // tamanho do bloco de cada thread
    long int tamBloco = dim / nthreads;

    // elemento inicial do bloco da thread
    long int inicio = id * tamBloco;

    // elemento final (nao processado) do bloco da thread
    long int fim = ((id == nthreads - 1) ? dim : inicio + tamBloco);

    // contador dos elementos de cada bloco
    long int contadorLocal = 0;

    for (long int i = inicio; i < fim; i++) {
        if (vetor[i] > limiteInferior && vetor[i] < limiteSuperior)
            contadorLocal++;
    }
    
    pthread_exit((void * ) contadorLocal);
}

// fluxo principal
int main(int argc, char const *argv[])
{
    // recebe os parametros de entrada (dimensao do vetor, numero de threads)
    if (argc < 3) {
        fprintf(stderr, "Digite: %s <dimensao do vetor> <numero de threads>\n", argv[0]);
        return 1;
    }

    // variaveis do fluxo principal
    dim = atoll(argv[1]);
    nthreads = atoi(argv[2]);
    pthread_t * tid;
    long int retorno;

    printf("Insira o limite inferior: ");
    scanf("%d", &limiteInferior);

    printf("Insira o limite superior: ");
    scanf("%d", &limiteSuperior);

    // aloca o vetor de entrada
    vetor = (double*) malloc(sizeof(double)*dim);
    if(vetor == NULL) {
        fprintf(stderr, "Erro--malloc\n");
        return 2;
    }

    // preenche o vetor de entrada
    for (long int i = 0; i < dim; i++) {
        vetor[i] = (float) (rand() % 1000);
    }
    
    // contagem sequencial dos elementos 
    long int contagemSequencial = 0;
    double inicio, fim;
    GET_TIME(inicio);
    for (long int i = 0; i < dim; i++) {
        if (vetor[i] > limiteInferior && vetor[i] < limiteSuperior)
            contagemSequencial++;
    }

    // tomada de tempo sequencial
    GET_TIME(fim);
    printf("Tempo sequencial: %lf\n", fim - inicio);
    
    // soma concorrente dos elementos
    long int contagemConcorrente = 0;
    tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if (tid == NULL) {
        fprintf(stderr, "ERRO--malloc");
        return 2;
    }

    GET_TIME(inicio);
    // criar as threads
    for (long int i = 0; i < nthreads; i++) {
        if (pthread_create(tid + i, NULL, tarefa, (void *) i)) {
            fprintf(stderr, "ERRO--pthread_create\n");
            return 3;
        }
    }

    // aguardar o termino das threads
    for (long int i = 0; i < nthreads; i++) {
        if (pthread_join(*(tid + i), (void**) &retorno)) {
            fprintf(stderr, "ERRO--pthread_join\n");
            return 4;
        }
        contagemConcorrente += retorno;
        // contagemConcorrente++;
    }

    // tomada de tempo concorrente
    GET_TIME(fim);
    printf("Tempo concorrente: %lf\n", fim - inicio);

    // imprime resultados
    printf("Contagem sequencial: \t%ld\n", contagemSequencial);
    printf("Contagem concorrente: \t%ld\n", contagemConcorrente);

    // libera as areas de memoria alocadas
    free(vetor);
    free(tid);

    return 0;
}
