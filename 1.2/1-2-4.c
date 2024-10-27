#include<sys/types.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/syscall.h>


void* threadFunction1(void* arg){
        system("~/1.2/system_call");

        printf("thread1 pid=%d,tid=%d \n",getpid(),syscall(SYS_gettid));
        return NULL;
}
void* threadFunction2(void* arg){
        system("~/1.2/system_call");

        printf("thread2 pid=%d,tid=%d \n",getpid(),syscall(SYS_gettid));
        return NULL;
}



int main(){
        pthread_t thread1,thread2;

        if(pthread_create(&thread1,NULL,threadFunction1,NULL)!=0){
                perror("Failed to create thread1. \n");
                return 1;
        }
        if(pthread_create(&thread2,NULL,threadFunction2,NULL)!=0){
                perror("Failed to create thread2. \n");
                return 2;
        }

        pthread_join(thread1,NULL);
        pthread_join(thread2,NULL);
        return 0;


}


