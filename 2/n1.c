#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include<time.h>
int flag = 1;
pid_t pid1 = -1, pid2 = -1;
void parent() {
    if(pid1>0 && pid2>0)
    {
    printf("\nget SIG.\n");
    kill(pid1,16);
    kill(pid2,17);
    }
}
void alrm_handler() {
    printf("\nget ALARM\n");
        kill(pid1, 16);
        kill(pid2, 17);
}
void child1(){
    printf("\nChild process1 is killed by parent!!\n");
    flag=0;
}
void child2(){
    printf("\nChild process2 is killed by parent!!\n");
    flag=0;
}
void child_handler(int signum) {
    if (signum == SIGUSR1) {
        //printf("\nChild1 process is ready to receive signals.\n");
    }
    else if(signum == SIGUSR2){
         //printf("\nChild2 process is ready to receive signals.\n");
    }
}
 
int main() {
    signal(SIGUSR1, child_handler);
    signal(SIGUSR2, child_handler);
    signal(SIGINT, parent);
    signal(SIGQUIT, parent);
    while (pid1 == -1){
        pid1 = fork();
    }
 
    if (pid1 > 0){
        while (pid2 == -1){
            pid2 = fork();
        }
 
        if (pid2 > 0){
          //pause();
          //pause();
          //alarm(5);
          signal(SIGALRM, alrm_handler);
          alarm(5);
          wait(NULL);
          wait(NULL);            
          printf("\nParent process is killed!!\n");
        }
        else {
          //sleep(1);
          signal(17,child2);
          kill(getppid(),SIGUSR2);  
          while(flag)pause();
          return 0;
        }
    }
    else{
      //sleep(2);
      signal(16,child1);
      kill(getppid(),SIGUSR1);
      while(flag)pause();
      return 0;
    }
    return 0;
}
