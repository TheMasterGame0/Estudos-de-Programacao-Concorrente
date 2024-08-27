#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define PR 30 //número de produtores
#define CN 5 // número de consumidores
#define N 5  //tamanho do buffer

void * produtor (void * meuid);
void * consumidor (void * meuid);

//pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 
// pthread_cond_t produtorCond = PTHREAD_COND_INITIALIZER;
// pthread_cond_t consumidorCond = PTHREAD_COND_INITIALIZER;
sem_t sem;

int cont = 0;

void main(argc, argv)
int argc;
char *argv[];
{
  int erro;
  int i, n, m;
  int *id;

  pthread_t tid[PR];
  sem_init(&sem,0,1);

  for (i = 0; i < PR; i++)
  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, produtor, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  pthread_t tCid[CN];

  for (i = 0; i < CN; i++)
  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tCid[i], NULL, consumidor, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  pthread_join(tid[0],NULL);
}

void * produtor (void* pi)
{
  while(1)
  { 
    sleep(1); // tempo para produzir

    sem_wait(&sem);
      if (cont < N){
        printf("Inserindo item produzido. Buffer %d \n", cont);
        cont += 1;
      }
    sem_post(&sem);
  }
  pthread_exit(0);
  
}

void * consumidor (void* pi)
{
  while(1)
  {

    sem_wait(&sem);
      if (cont > 0){
        printf("Pegando item para consumir. Buffer %d \n", cont);
        cont -= 1;
      }
    sem_post(&sem);

    sleep(0.5); // tempo para consumir
  }
  pthread_exit(0);
}

// void * produtor (void* pi)
// {
//   while(1)
//   { 
//     sleep(1); // tempo para produzir

//     pthread_mutex_lock(&lock);
//       while (cont == N){
//         pthread_cond_wait(&produtorCond, &lock);
//       }

//       printf("Inserindo item produzido. Buffer %d \n", cont);
//       cont += 1;

//       if (cont == 1){
//         pthread_cond_broadcast(&consumidorCond);
//       }
//     pthread_mutex_unlock(&lock);
//   }
//   pthread_exit(0);
  
// }

// void * consumidor (void* pi)
// {
//   while(1)
//   {
//     pthread_mutex_lock(&lock);
//       while (cont == 0){
//         pthread_cond_wait(&consumidorCond, &lock); // Condição para dormir
//       }

//       printf("Pegando item para consumir. Buffer %d \n", cont);
//       cont -= 1;

//       if (cont == N-1){
//         pthread_cond_broadcast(&produtorCond);  // Acordar produtores
//       }
//     pthread_mutex_unlock(&lock);

//     sleep(0.5); // tempo para consumir
//   }
//   pthread_exit(0);
// }
