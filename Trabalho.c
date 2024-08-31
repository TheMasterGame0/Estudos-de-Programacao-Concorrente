#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define NUMCLIENTES 50
#define NUMCOZINHEIROS 2
#define NUMALIMENTOS 5
#define VALORAVISO 4 // Qual o valor deve ser usado como base para avisar os cozinheiros

int NComida[] = {13, 10, 8, 15, 10}; // Porções em cada posição do buffet
// int totalClientes = NUMCLIENTES;  // Usado para testes
// pthread_mutex_t lockControle = PTHREAD_MUTEX_INITIALIZER; // Usado para testes

pthread_t clientes[NUMCLIENTES];
pthread_t cozinheiros[NUMCOZINHEIROS];

sem_t buffet[NUMALIMENTOS][2];
pthread_mutex_t pegar[NUMALIMENTOS] = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cozinhar[NUMALIMENTOS] = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t vazio[NUMALIMENTOS];
pthread_cond_t cozinha = PTHREAD_COND_INITIALIZER;

void * cliente(void *arg);
void * cozinheiro(void *arg);

int main(int argc, char **argv){
  int i, j;
  srand(time(NULL));
  int *id;

  // Inicializar o buffet com a sua quantidade de quantas pessoas podem estar por posição (2), em filas distintas    
  for(i=0; i < NUMALIMENTOS; i++){ 
    for(j=0; j < 2; j++){
      sem_init(&buffet[i][j],0,1);
    }
  }

  printf("%d - %d - %d - %d - %d\n", NComida[0], NComida[1], NComida[2], NComida[3], NComida[4]);

  for(i = 0; i < NUMCOZINHEIROS ;i++){
    id = (int *) malloc(sizeof(int));
    *id = i;
    pthread_create(&(cozinheiros[i]),NULL,cozinheiro, (void*) (id));
  };

  for(i = 0; i < NUMCLIENTES ;i++){
    id = (int *) malloc(sizeof(int));
    *id = i;
    pthread_create(&(clientes[i]),NULL,cliente, (void*) (id));
  }
  pthread_join(cozinheiros[0],NULL);
}

void * cliente(void *arg){
  int id = *((int *) arg);
  int lado = id%2;
  for(int i = 0; i < NUMALIMENTOS;i++){
    sem_wait(&buffet[i][lado]); // Aguarda sua vez para posição no buffet da fila do cliente
      printf("Cliente %d entrou na posição %d da fila %d \n", id, i, lado);
      pthread_mutex_lock(&pegar[i]); // Garantir uma pessoa pegando uma comida por vez
        int comidaDisponivel = NComida[i];
        int porcao = rand()%4; // Gera um valor inteiro aleatório de 0 a 3

        while (comidaDisponivel - porcao < 0){
          //Avisar cozinheiros e esperar repor a comida
          //printf("Cliente %d está esperando repor a comida na posição %d \n", id, i);
          pthread_cond_signal(&cozinha);            // Acorda o cozinheiro
          pthread_cond_wait(&vazio[i], &pegar[i]);  // Espera a reposição
          comidaDisponivel = NComida[i];            // Verifica se realmente foi abastecido
        }

        if(comidaDisponivel - porcao <= VALORAVISO){
          //Avisar cozinheiros 
          pthread_cond_signal(&cozinha);            // Acorda o cozinheiro
        } 

        //printf("Cliente %d pegando a comida em %d \n", id, i);
        sleep(0.5); // Tempo necessário para o cliente se servir
        NComida[i] -= porcao;
        

      pthread_mutex_unlock(&pegar[i]);
      printf("Cliente %d saiu da posição %d da fila %d \n", id, i, lado);
    sem_post(&buffet[i][lado]); // Libera sua vez na posição do buffet
  }
  // Usado para testes
  // pthread_mutex_lock(&lockControle);
  //   totalClientes -= 1;
  //   printf("Clientes restantes: %d\n", totalClientes);
  //   printf("%d - %d - %d - %d - %d\n", NComida[0], NComida[1], NComida[2], NComida[3], NComida[4]);
  // pthread_mutex_unlock(&lockControle);
}

void * cozinheiro(void *arg){
  int id = *((int *) arg);
  while (1){
    pthread_mutex_lock(&lock);
      pthread_cond_wait(&cozinha, &lock); // Espera ser acordado para cozinhar
      printf("Cozinheiro %d acordou\n", id);
    pthread_mutex_unlock(&lock);

    for(int i = 0; i < NUMALIMENTOS; i++){
      int comidaDisponivel = NComida[i];
      if(comidaDisponivel <= VALORAVISO){
        if(pthread_mutex_trylock(&cozinhar[i]) == 0){
          printf("Cozinheiro %d está indo cozinhar %d\n", id, i);
          int porcao = rand()%3 + 4;       // Gera um valor inteiro aleatório de 4 a 7
          sleep(0.25*porcao);              // Tempo para cozinhar a porção 
          pthread_mutex_lock(&pegar[i]);   // Garantir que apenas o cozinheiro está pegando a comida
            NComida[i] += porcao;
          pthread_mutex_unlock(&pegar[i]);
        pthread_mutex_unlock(&cozinhar[i]);
        }
      }
    }

    for(int i = 0; i < NUMALIMENTOS; i++){
      pthread_cond_broadcast(&vazio[i]);
    }
  }
}