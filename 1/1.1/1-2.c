#include<sys/types.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>




int global = 2004;
int main(){

        pid_t pid,pid1;

        pid=fork();
        if(pid<0){
                fprintf(stderr,"Fork Failed");
                return 1;
        }
        else if(pid==0){
                pid1=getpid();
                printf("child: pid = %d \n",pid);
                printf("child: pid1 = %d \n",pid1);
                global=24;
                printf("child's global = %d , add= %d \n",global,&global);


        }

        else{
                pid1=getpid();
                printf("parent: pid = %d \n",pid);
                printf("parent: pid1 = %d \n",pid1);
                wait(NULL);
                global=88;
                printf("parent's global = %d , add =%d \n",global,&global);


        }
        return 0;


}



