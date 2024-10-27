#include<sys/types.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>


int main(){
        /*
        execlp("/bin/sh","sh","-c","echo Process ID: $$",NULL);
                perror("execlp failed.");
                return 1;
        */
        pid_t pid=getpid();
        printf("System call Process ID: %d \n",pid);
        return 0;
}


