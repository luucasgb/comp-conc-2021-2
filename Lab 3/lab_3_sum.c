#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

// dimensao do vetor de entrada
long int dim;

// numero de threads
int nthreads;

// vetor de entrada com dimensao = dim
int *vetor;

//fluxo das threads


// fluxo principal
int main(int argc, char const *argv[])
{
    // recebe os parametros de entrada (dimensao do vetor, numero de threads)
    if (argc < 3) {
        fprintf(stderr, "Digite: %s <dimensao do vetor> <numero de threads>\n", argv[0]);
        return 1;
    }

    dim = atoi(argv[1]);
    nthreads = atoi(argv[2]);

    // aloca o vetor de entrada
    vetor = (int*) malloc(sizeof(int)*dim);
    if(vetor == NULL) {
        fprintf(stderr, "Erro--malloc\n");
        return 2;
    }

    // preenche o vetor de entrada
    for (long int i = 0; i < dim; i++) {
        vetor[i] = i % 1000;
    }
    
    // soma sequencial dos elementos 
    long int somaSeq = 0;
    GET_TIME(inicio);
    for (long int i = 0; i < dim; i++) {
        somaSeq += vetor[i];
    }
    GET_TIME(fim);
    printf("Tempo sequencial: %lf\n", fim - inicio);

    // soma concorrente dos elementos
    long int somaConc = 0;
    // criar as threads
    
    // aguardar o termino das threads
    
    // computar o valor final
    
    // exibir os resultados

    // libera as areas de memoria alocadas
    free(vetor);

    return 0;
}
