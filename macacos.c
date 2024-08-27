// Como executar: gcc ARQUIVo -o SAIDA

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MA 100 //macacos que andam de A para B
#define MB 100 //macacos que andam de B para A

pthread_mutex_t corda; 
pthread_mutex_t AB;
pthread_mutex_t BA;
pthread_mutex_t turno;
pthread_mutex_t gori;
int cAB = 0;
int cBA = 0;


void * macacoAB(void * a) {
    int i = *((int *) a);    
    while(1){
      //Procedimentos para acessar a corda
      pthread_mutex_lock(&gori);
        pthread_mutex_lock(&turno); 
          pthread_mutex_lock(&AB); // Garantir a contagem de um macaco entrar na corda por vez
          cAB = cAB + 1;
          if (cAB == 1) {
            pthread_mutex_lock(&corda); // Garantir o fluxo por apenas um tipo
          }
          pthread_mutex_unlock(&AB);
        pthread_mutex_unlock(&turno);
      pthread_mutex_unlock(&gori);

      // Está na corda
      printf("Macaco %d passando de A para B. Na corda: AB: %d. BA:%d \n", i, cAB, cBA );
      sleep(0.1);

	    //Procedimentos para quando sair da corda
      pthread_mutex_lock(&AB);
      cAB = cAB - 1;
      if (cAB == 0) {
        pthread_mutex_unlock(&corda); // Garantir o fluxo em só um sentido
      }
      pthread_mutex_unlock(&AB);

    }
    pthread_exit(0);
}

void * macacoBA(void * a) {
  int i = *((int *) a);    
  while(1){
    //Procedimentos para acessar a corda
    pthread_mutex_lock(&gori);
      pthread_mutex_lock(&turno);
        pthread_mutex_lock(&BA);
          cBA = cBA + 1;
          if (cBA == 1) {
            pthread_mutex_lock(&corda); // Garantir o fluxo em só um sentido
          }
        pthread_mutex_unlock(&BA);
      pthread_mutex_unlock(&turno);
    pthread_mutex_unlock(&gori);

    // Está na corda
    printf("Macaco %d passando de B para A. Na corda: AB: %d. BA:%d \n", i, cAB, cBA );
    sleep(0.1);

    //Procedimentos para quando sair da corda
    pthread_mutex_lock(&BA);
    cBA = cBA - 1;
    if (cBA == 0) {
      pthread_mutex_unlock(&corda); // Garantir o fluxo em só um sentido
    }
    pthread_mutex_unlock(&BA);
  }
  pthread_exit(0);
}

void * gorilaAB(void * a){
  while(1){
    //Procedimentos para acessar a corda
    pthread_mutex_lock(&gori);
        pthread_mutex_lock(&corda);
    pthread_mutex_unlock(&gori);

    printf("Gorila passado de A para B. Na corda: AB: %d. BA:%d \n",cAB, cBA);
    sleep(5);
    pthread_mutex_unlock(&corda);
    //Procedimentos para quando sair da corda
  }
    pthread_exit(0);
}

void * gorilaBA(void * a){
  while(1){
    //Procedimentos para acessar a corda
    pthread_mutex_lock(&gori);
        pthread_mutex_lock(&corda);
    pthread_mutex_unlock(&gori);

    printf("Gorila passado de B para A. Na corda: AB: %d. BA:%d \n",cAB, cBA);
    sleep(5);
    pthread_mutex_unlock(&corda);
    //Procedimentos para quando sair da corda
  }
    pthread_exit(0);
}


int main(int argc, char * argv[]){
  pthread_mutex_init(&corda, NULL);
  pthread_mutex_init(&turno, NULL);
  pthread_mutex_init(&gori, NULL);
  pthread_mutex_init(&AB, NULL);
  pthread_mutex_init(&BA, NULL);

  pthread_t macacos[MA+MB];
  int *id;
  int i = 0;

  for(i = 0; i < MA; i++){
    id = (int *) malloc(sizeof(int));
    *id = i;
    if(i%2 == 0){
      if(pthread_create(&macacos[i], NULL, &macacoAB, (void*)id)){
        printf("Não pode criar a thread %d\n", i);
        return -1;
      }
    }
  }
  for( i = 0; i< MB; i++){
    id = (int *) malloc(sizeof(int));
    *id = i;
    if(pthread_create(&macacos[i], NULL, &macacoBA, (void*)id)){
      printf("Não pode criar a thread %d\n", i);
      return -1;
    }
  }
  pthread_t g[3];
  pthread_create(&g[0], NULL, &gorilaAB, NULL);
  pthread_create(&g[1], NULL, &gorilaBA, NULL);
  pthread_create(&g[2], NULL, &gorilaAB, NULL);

  pthread_join(macacos[0], NULL);
  return 0;
}
