/*
 * Problema do barbeiro dorminhoco.
 */ 
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N_CLIENTES 50
#define N_CADEIRAS 5

sem_t sem_cadeiras;
sem_t barbeiro;
sem_t corte;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void * f_barbeiro(void *v) {

  while(1) {
    //...Esperar/dormindo algum cliente sentar na cadeira do barbeiro (e acordar o barbeiro)
    sem_wait(&barbeiro);
      sleep(1); // Cortar o cabelo do cliente 
      printf("Barbeiro cortou o cabelo de um cliente\n"); 
    sem_post(&corte);  //...Liberar/desbloquear o cliente
  }
  pthread_exit(0);
}

void* f_cliente(void* v) {
  int id = *(int*) v;
  sleep(id%3);
  if(sem_trywait(&sem_cadeiras) == 0){ //conseguiu pegar uma cadeira de espera
    printf("Cliente %d entrou na barbearia \n",id);
    pthread_mutex_lock(&lock); // Pega a cadeira do barbeiro
      printf("Clinte %d vai cortar o cabelo \n", id);
      sem_post(&sem_cadeiras); // Libera a cadeira de espera
      sem_post(&barbeiro); // Acorda o barbeiro
      sem_wait(&corte); // Espera cortar
    pthread_mutex_unlock(&lock); // Libera cadeira do barbeiro
    printf("Cliente %d cortou o cabelo e foi embora \n",id);

  }else{//barbearia cheia
    printf("Barbearia cheia, cliente %d indo embora\n",id);
  }

  pthread_exit(0);
}

int main() {
  pthread_t thr_clientes[N_CLIENTES], thr_barbeiro;
  int i, id[N_CLIENTES];

  sem_init(&sem_cadeiras, 0, N_CADEIRAS);
  sem_init(&barbeiro, 1, 1);
  sem_init(&corte, 1, 1);

  for (i = 0; i < N_CLIENTES; i++) {
    id[i] = i;
    pthread_create(&thr_clientes[i], NULL, f_cliente, (void*) &id[i]);
  }

  pthread_create(&thr_barbeiro, NULL, f_barbeiro, NULL);
  
  for (i = 0; i < N_CLIENTES; i++) 
    pthread_join(thr_clientes[i], NULL);

  /* Barbeiro assassinado */
  
  return 0;
}

