/* Disciplina: Computacao Concorrente */
/* Prof: Silvana Rossetto */
/* Aluno: Lucas Guimarães Batista */
/* Laboratorio: 9 */
/* Codigo: Uso de semaforos para implementar exclusao mutua e sincronizacao condicional. */
/***** Condicao logica da aplicacao: A thread 5 deve sempre ser a primeira a imprimir sua mensagem, e a thread 1, a ultima. ****/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

/* Variaveis globais */
#define NUM_THREADS 5
sem_t sem_1, sem_5;

/* Thread 1: Volte sempre! */
void * t1 (void * arg) {
  // Espera ser liberada por 3 threads, imprime e sai
  sem_wait(&sem_1);
  sem_wait(&sem_1);
  sem_wait(&sem_1);
  printf("Volte sempre!\n");
  pthread_exit(NULL);
}

/* Thread 2: "Fique a vontade." */
void * t2 (void * arg)  {
  // Espera ser liberada pela t5
  sem_wait(&sem_5);
  printf("Fique a vontade.\n");

  // Libera t1 e sai
  sem_post(&sem_1);
  pthread_exit(NULL);
}

/* Thread 3: "Sente-se por favor." */
void * t3 (void * arg)  {
  // Espera ser liberada pela t5
  sem_wait(&sem_5);
  printf("Sente-se por favor.\n");

  // Libera t1 e sai
  sem_post(&sem_1);
  pthread_exit(NULL);
}

/* Thread 4: "Aceita um copo d’agua?." */
void * t4 (void * arg)  {
  // Espera ser liberada pela t5
  sem_wait(&sem_5);
  printf("Aceita um copo d'agua?\n");

  // Libera t1 e sai
  sem_post(&sem_1);
  pthread_exit(NULL);
}

/* Thread 5: "Seja bem-vindo!" */
void * t5 (void * arg)  {
  printf("Seja bem-vindo!\n");

  // Libera as 3 threads e sai
  sem_post(&sem_5);
  sem_post(&sem_5);
  sem_post(&sem_5);
  pthread_exit(NULL);
}

int main (int argc, char *argv[]) {
  pthread_t tid_sistema[NUM_THREADS]; // Identificadores das threads no sistema
  
  // Inicializa semaforo
  sem_init(&sem_1, 0, 0);
  sem_init(&sem_5, 0, 0);

  // Cria as threads
  if (pthread_create(&tid_sistema[0], NULL, t1, NULL))
  {
    fprintf(stderr, "--ERRO: pthread_create\n");
    return 1;
  }

  if (pthread_create(&tid_sistema[1], NULL, t2, NULL))
  {
    fprintf(stderr, "--ERRO: pthread_create\n");
    return 1;
  }

  if (pthread_create(&tid_sistema[2], NULL, t3, NULL))
  {
    fprintf(stderr, "--ERRO: pthread_create\n");
    return 1;
  }

  if (pthread_create(&tid_sistema[3], NULL, t4, NULL)) 
  {
    fprintf(stderr, "--ERRO: pthread_create\n");
    return 1;
  }

  if (pthread_create(&tid_sistema[4], NULL, t5, NULL))
  {
    fprintf(stderr, "--ERRO: pthread_create\n");
    return 1;
  }

  // Espera threads terminarem
  for (int i = 0; i < NUM_THREADS; i++) {
    if (pthread_join(tid_sistema[i], NULL)) {
      fprintf(stderr, "--ERRO: pthread_join\n");
      return 1;
    }
  }

  // Desaloca variáveis e termina execução
  sem_destroy(&sem_1);
  sem_destroy(&sem_5);

  return 0;
}
