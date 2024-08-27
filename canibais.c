#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAXCANIBAIS 20

int comida = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t CO = PTHREAD_COND_INITIALIZER;
pthread_cond_t CA = PTHREAD_COND_INITIALIZER;

int PORCAO;

void *canibal(void*meuid);
void *cozinheiro(void*mid);


void main(argc, argv)
int argc;
char *argv[];
{
  int erro;
  int i, n, nco;
  int *id;

  pthread_t tid[MAXCANIBAIS];

  if(argc != 4){
    printf("erro na chamada do programa: jantar <#canibais> <#comida>\n");
    exit(1);
  }
  
  n = atoi (argv[1]); //número de canibais
  PORCAO = atoi (argv[2]); // quantidade de porções que o cozinheiro consegue preparar por vez
  nco = atoi (argv[3]); // número de cozinheiros
  printf("numero de canibais: %d -- quantidade de comida: %d -- numero de cozinheiros: %d\n", n, PORCAO, nco);

  if(n > MAXCANIBAIS){
    printf("o numero de canibais e' maior que o maximo permitido: %d\n", MAXCANIBAIS);
    exit(1);
  }
  
  for (i = 0; i < n; i++)  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, canibal, (void *) (id));

    if(erro){
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  for (i = 0; i < nco; i++)  {
    id = (int *) malloc(sizeof(int));
    *id = i+ MAXCANIBAIS;
    erro = pthread_create(&tid[i], NULL, cozinheiro, (void * ) (id));

    if(erro){
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }
  
  pthread_join(tid[0],NULL);

}

void * canibal (void* pi){
  
  while(1) {
    //pegar uma porção de comida
    pthread_mutex_lock(&mutex);
      while(comida == 0){
        pthread_cond_wait(&CA, &mutex);
      }
      comida -= 1;
      if(comida == 0){
        pthread_cond_signal(&CO);
      }
    pthread_mutex_unlock(&mutex);
    
    printf("%d: vou comer a porcao que peguei\n", *(int *)(pi));
    sleep(1);
  }
  
}

void *cozinheiro (void* i){ 

  while(1){
    pthread_mutex_lock(&mutex);
      //dormir enquanto tiver comida
      while(comida > 0){
        pthread_cond_wait(&CO, &mutex);
      }
    pthread_mutex_unlock(&mutex);

    // Permite um cozinheiro cozinhar por vez
    printf("cozinheiro %d: vou cozinhar\n", *((int *)(i)));
    sleep(2);
    
    pthread_mutex_lock(&mutex);
      //acordar os canibais
      comida += PORCAO;
      pthread_cond_broadcast(&CA);
    pthread_mutex_unlock(&mutex);
}

}
