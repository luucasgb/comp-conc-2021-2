/* Disciplina: Computacao Concorrente */
/* Prof: Silvana Rossetto */
/* Aluno: Lucas Guimarães Batista */
/* Laboratorio: 10 */
/* Codigo: Implementar uma variacao na implementacao do problema produtor/consumidor usando semaforos. */
/***** A cada execucao de um produtor, ele preenche o buffer inteiro. O consumidor consome um item de cada vez.  ****/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5
int num_prod, num_cons, buffer[N];
int contador = 0, entrada = 0, saida = 0;
sem_t sem_prod;
sem_t sem_cons;

// Imprime o buffer
void Imprime (int n) {
  int i;
  for (i = 0; i < n; i++)
    printf("%d ", buffer[i]);
  puts("");
}

// Insere elementos no buffer até enchê-lo e libera seu uso para a instância adequada
void Insere (int item, int id) {
  // Espera ser liberado pelos produtores
  sem_wait(&sem_prod);

  // Preenche o buffer e atualiza variaveis de estado
  buffer[entrada] = item;
  entrada = (entrada + 1) % N;
  contador++;
  printf("Thread produtora %d inseriu %d\n", id, item);
  Imprime(N);
  
  // Sinaliza consumidores mantendo lock de produtores
  if (contador == N) {
    sem_post(&sem_cons);
  }

  // Sinaliza produtores mantendo lock de consumidores
  else {
    sem_post(&sem_prod);
  }
}

void Retira (int id) {
  int item;

  // Espera ser liberado pelos consumidores
  sem_wait(&sem_cons);

  // Imprime mensagem na tela
  printf("Thread consumidora %d quer consumir buffer\n", id);

  // Consome o buffer e atualiza variáveis de estado
  item = buffer[saida];
  saida = (saida + 1) % N;
  contador--;
  printf("Thread consumidora %d consumiu %d\n", id, item);
  Imprime(N);

  // Sinaliza threads produtoras, as liberando
  sem_post(&sem_prod);
}

// Threads produtores
void * produtor (void * args) {
  long int id_thread = (long int) args;
  printf("Thread produtora %ld\n", id_thread);
  while (1) {
    Insere(id_thread, id_thread);
    sleep(1);
  }
  free(args);
  pthread_exit(NULL);
}

// Threads consumidores
void * consumidor (void * args) {
  long int id_thread = (long int) args;
  printf("Thread consumidora %ld\n", id_thread);
  while (1) {
    Retira(id_thread);
    sleep(1); // faz o processamento do item
  }
  free(args);
  pthread_exit(NULL);
}

int main (int argc, char* argv[]) {
  // Avaliação de chamada do programa
  if (argc < 3) {
    printf("Digite: %s <numero de threads produtoras> <numero de threads consumidoras>\n", argv[0]);
    return 1;
  }

  // Define variáveis de execução
  num_prod = atoi(argv[1]);
  num_cons = atoi(argv[2]);
  pthread_t tid_sistema[num_prod + num_cons];
  sem_init(&sem_prod, 0, 1);
  sem_init(&sem_cons, 0, 0);

  // Inicializa buffer
  for (int i = 0; i < N; i++)
    buffer[i] = 0;

  // Cria threads produtoras
  for (long i = 0; i < num_prod; i++) {
    if (pthread_create(&tid_sistema[i], NULL, produtor, (void *) i)) {
      fprintf(stderr, "--ERRO: pthread_create\n");
      return 2;
    }
  }

  // Cria threads consumidoras
  for (long i = 0; i < num_cons; i++) {
    if (pthread_create(&tid_sistema[i], NULL, consumidor, (void *) i)) {
      fprintf(stderr, "--ERRO: pthread_create\n");
      return 2;
    }
  }

  // Espera fim da execução
  for (int i=0; i<(num_prod + num_cons); i++) {
    if (pthread_join(tid_sistema[i], NULL)) {
      fprintf(stderr, "--ERRO: pthread_join\n");
      return 2;
    }
  }
  
  // Termina
  sem_destroy(&sem_prod);
  sem_destroy(&sem_cons);
  return 0;
}
