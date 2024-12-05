#include<sys/types.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<pthread.h>
#include<sys/syscall.h>
int main(){
        /*
        execlp("/bin/sh","sh","-c","echo Process ID: $$",NULL);
                perror("execlp failed.");
                return 1;
        */
        pid_t pid=getpid();
        printf("System call Process ID: %d \n",pid);
        //pthread_t tid=syscall(SYS_gettid);
        //printf("System call Thread ID: %d \n",tid);
        return 0;
}



