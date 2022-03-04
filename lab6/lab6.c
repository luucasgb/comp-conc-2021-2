#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int blocked_count = 0;
int N;
int *values;
pthread_mutex_t mutex;
pthread_cond_t cond;

void barreira (int nthreads, int id_thread) {
  pthread_mutex_lock(&mutex);

  // Ultima thread a chegar
  if (blocked_count == (nthreads - 1)) {
    printf("Thread %d foi a ultima a chegar. Liberando as demais em espera.\n", id_thread);
    pthread_cond_broadcast(&cond);
    blocked_count = 0;
  } 
  // Resto
  else {
    blocked_count++;
    printf("Thread %d aguardando na barreira...\n", id_thread);
    pthread_cond_wait(&cond, &mutex);
  }
  pthread_mutex_unlock(&mutex);
}

void * soma_elementos (void *arg) {
  long int id_thread = (long int) arg;
  int *sum;
  
  printf("Thread %ld executando...\n", id_thread);
  
  sum = (int *) malloc(sizeof(int));
  if (sum == NULL) 
  {
    fprintf(stderr, "--ERRO: malloc\n");
    exit(2);
  }
  *sum = 0;

  for (int i = 0; i < N; i++) 
  {
    for (int j = 0; j < N; j++)
      *sum += values[j]; 
    barreira(N, id_thread);
    values[id_thread] = rand() % 10;
    barreira(N, id_thread);
  }
  pthread_exit((void *) sum);
}

int main (int argc, char* argv[]) {
  pthread_t *tid_sistema;
  int *total;
  int *thread_value; 

  // Validação de entrada
  if (argc < 2) {
    printf("Digite: %s <numero de threads>\n", argv[0]);
    return 1;
  }
  N = atoi(argv[1]);

  // Alocação de variáveis
  total = (int *) malloc(sizeof(int) * N);
  if (total == NULL) {
    fprintf(stderr, "--ERRO: malloc\n");
    return 2;
  }
  values = (int *) malloc(sizeof(int) * N);
  if (values == NULL) {
    fprintf(stderr, "--ERRO: malloc\n");
    return 2;
  }
  tid_sistema = (pthread_t *) malloc(sizeof(pthread_t) * N);
  if (tid_sistema == NULL) {
    fprintf(stderr, "--ERRO: malloc\n");
    return 2;
  }

  // Preenchimento do vetor de acordo com a entrada
  for (int i = 0; i < N; i++)
    values[i] = rand() % 10;

  // Inicializa mutex e variável de condição
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);

  // Cria as threads
  for (long int i = 0; i < N; i++) 
  {
    if (pthread_create(tid_sistema+i, NULL, soma_elementos, (void *) i)) {
      fprintf(stderr, "--ERRO: pthread_create\n");
      return 3;
    }
  }

  // Aguardar o termino das threads
  for (int i = 0; i < N; i++) 
  {
    if (pthread_join(*(tid_sistema+i), (void**) &thread_value)) {
      fprintf(stderr, "--ERRO: pthread_join\n");
      return 3;
    }
    total[i] = *thread_value;
  }

  // Verificar corretude
  printf("\nSomas realizadas pelas threads:\n");
  for (int i = 0; i < N; i++) 
  {
    printf("Thread %d encontrou %d\n", i + 1, total[i]);

    // Verifica se a mesma soma foi encontrada por todas as threads
    if (total[i] != total[i + 1]) {
      // Não precisa acessar último indice
      if (i != N -1) {
        printf("--ERRO: Divergencia no indice %d: %d diferente de %d\n", i + 1, total[i], total[i + 1]);
        return 4;
      }
    }
  }
  printf("Soma realizada corretamente.\n");

  // Desaloca variaveis e termina
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond);
  free(values);
  free(thread_value);
  free(total);
  return 0;
}
