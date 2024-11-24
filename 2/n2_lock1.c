#include<unistd.h>
#include<signal.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<time.h>
#include<stdio.h>
int pid1 = -1,pid2 = -1;


int main(){
  
  int fd[2];
  char InPipe[4000];
  char c1 = '1',c2 = '2';
  pipe(fd);

  while(pid1 == -1){
    pid1 = fork();
  }
  if(pid1 == 0){
    close(fd[0]);
    //lockf(fd[1],1,0);

    for(int i=0;i<2000;i++){
      write(fd[1],&c1,1);
    }

    sleep(5);
    //lockf(fd[1],0,0);
    close(fd[1]);
  }
  else{
    while(pid2 == -1){
      pid2 = fork();
    }
    if(pid2 == 0){
      close(fd[0]);
      lockf(fd[1],1,0);
      for(int i=0;i<2000;i++){
        write(fd[1],&c2,1);
      }
      sleep(5);
      lockf(fd[1],0,0);
      close(fd[1]);
    }
    else{
      wait(NULL);
      wait(NULL);

      close(fd[1]);

      int read_content = read(fd[0],InPipe,4000);
      InPipe[read_content] = '\0';
      printf("\n %s \n", InPipe);
      close(fd[0]);
      exit(0);
    }
  }


  exit(0);
}
