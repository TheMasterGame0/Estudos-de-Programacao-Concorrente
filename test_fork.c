#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

int main(void) {
  int i = 0;
  int valor = 10;
  pid_t pid;
  for(i=0; i < 4; ){
    i++;
    pid = fork();
    if(pid == 0){//filho
      valor++;
    }else{//pai
      valor--;
      waitpid(pid,NULL,0); // Espera o filho terminar de executar
    }
  }
  printf("Processo %d, pai: %d; valor: %d; pid %d\n",getpid(),getppid(),valor,pid);
  return 0;
}