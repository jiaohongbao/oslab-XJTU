#include<stdio.h>
#include<pthread.h>

int global=2024;


void* threadFunction1(void* arg){
        for(int i=0;i<5001;i++){
                global++;
        }
        printf("global(thread1) = %d \n",global);
        return NULL;

}

void* threadFunction2(void* arg){
        for(int i=0;i<5001;i++){
                global-=2;
        }
        printf("global(thread2) = %d \n",global);
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

        printf("finally, global is: %d \n",global);

        return 0;
}

