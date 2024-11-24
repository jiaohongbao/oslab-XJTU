#include<stdio.h>
#include<string.h>
#include<stdlib.h>









#define PROCESS_NAME_LEN 32
#define MIN_SLICE 10
#define DEFAULT_MEM_SIZE 1024
#define DEFAULT_MEM_START 0
#define MA_FF 1 
#define MA_BF 2
#define MA_WF 3

int mem_size = DEFAULT_MEM_SIZE;
int ma_algorithm = MA_FF;
static int pid = 0;
int flag = 0;
int num_free_blocks = 0;
int current_size_free = 0;






struct free_block_type{
  int size;
  int start_addr;
  struct free_block_type* next;
};

struct free_block_type* free_block_head=NULL;


struct allocated_block{
  int pid;
  int size;
  int start_addr;
  char process_name[PROCESS_NAME_LEN];
  struct allocated_block* next;
};

struct allocated_block* allocated_block_head=NULL;


struct free_block_type* init_free_block(int mem_size){
  struct free_block_type* fb;
  fb = (struct free_block_type*)malloc(sizeof(struct free_block_type));
  if(fb == NULL){
    printf("No mem (no the problem of this program. \n)");
    return NULL;
  }
  fb->size = mem_size;
  fb->start_addr = DEFAULT_MEM_START;
  fb->next = NULL;
  num_free_blocks = 1;
  return fb;
}

void display_menu(){
  printf("\n");
  printf("1 - Set memory size (default = %d) \n",DEFAULT_MEM_START);
  printf("2 - Select memory allocation algorithm\n");
  printf("3 - New process\n");
  printf("4 - Terminate a process\n");
  printf("5 - Display memory usage\n");
  printf("0 - Exit\n");
}


int set_mem_size(){
  int size;
  if(flag != 0){
    printf("Can't set memory size again\n");
    return 0;
  }
  printf("Total memory size = ");
  scanf("%d",&size);
  
  if(size > 0){
    mem_size = size;
    current_size_free = size;
    free_block->size=mem_size;
  }
  flag = 1;
  return 1;//????????????
}

void set_algorithm(){
  int algorithm;
  printf("\t1 - First Fit\n");
  printf("\t2 - Best Fit\n");
  printf("\t3 - Worst Fit\n");
  scanf("%d",&algorithm);
  if(algorithm >= 1 && algorithm <=3 ){
    ma_algorithm = algorithm;
  }
  rearrange(ma_algorithm);

}

rearrange(int algorithm){
  switch(algorithm){
    case MA_FF : rearrange_FF();break;
    case MA_BF : rearrange_BF();break;
    case MA_WF : rearrange_WF();break;
  }
}

/*void exchange12(struct free_block_type* p1,struct free_block_type* p2,struct free_block_type* p3,struct free_block_type* head)*/

void rearrange_FF(){
  struct free_block_type* free_block_head = free_block;
  struct free_block_type* p1 = NULL, p2 = NULL,p3 = NULL;

  if(free_block_head == NULL){
    return;
  }
  
  for(int i=0;i<num_free_blocks-1;i++){
    p1 = free_block_head;
    p2 = p1 -> next;
    p3 = p2 -> next;
    
    if(p1 == head && p1 -> start_addr >= p2 -> start_addr){
      head -> next = p2 -> next;
      p2 -> next = head;
      head = p2;
      p1 = head;
      p2 = p1 -> next;
    }
    while(p2 -> next != NULL){
      if(p2 -> start_addr >= p3 -> start_addr){
        p2 -> next = p3 -> next;
        p1 -> next = p3;
        p3 -> next = p2;
        p1 = p3;
        p3 = p2 -> next;

      }
      else{
        p1 = p2;
        p2 = p3;
        p3 = p3 -> next;
      }
    }
  }
  free_block = head;
  return;
}

void rearrange_BF(){
  struct free_block_type* free_block_head = free_block;
  struct free_block_type* p1 = NULL, p2 = NULL,p3 = NULL;

  if(free_block_head == NULL){
    return;
  }
  for(int i=0;i<num_free_blocks-1;i++){
    p1 = free_block_head;
    p2 = p1 -> next;
    p3 = p2 -> next;
    
    if(p1 == head && p1 -> size >= p2 -> size){
      head -> next = p2 -> next;
      p2 -> next = head;
      head = p2;
      p1 = head;
      p2 = p1 -> next;
    }
    while(p2 -> next != NULL){
      if(p2 -> size >= p3 -> size){
        p2 -> next = p3 -> next;
        p1 -> next = p3;
        p3 -> next = p2;
        p1 = p3;
        p3 = p2 -> next;

      }
      else{
        p1 = p2;
        p2 = p3;
        p3 = p3 -> next;
      }
    }
  }
  free_block = head;
  return;


}


void rearrange_WF(){

struct free_block_type* free_block_head = free_block;
  struct free_block_type* p1 = NULL, p2 = NULL,p3 = NULL;

  if(free_block_head == NULL){
    return;
  }
  for(int i=0;i<num_free_blocks-1;i++){
    p1 = free_block_head;
    p2 = p1 -> next;
    p3 = p2 -> next;
    
    if(p1 == head && p1 -> size <= p2 -> size){
      head -> next = p2 -> next;
      p2 -> next = head;
      head = p2;
      p1 = head;
      p2 = p1 -> next;
    }
    while(p2 -> next != NULL){
      if(p2 -> size <= p3 -> size){
        p2 -> next = p3 -> next;
        p1 -> next = p3;
        p3 -> next = p2;
        p1 = p3;
        p3 = p2 -> next;

      }
      else{
        p1 = p2;
        p2 = p3;
        p3 = p3 -> next;
      }
    }
  }
  free_block = head;
  return;



}

void new_process(){
  struct allocated_block* ab;
  int size; int ret;
  ab=(struct allocated_block*)malloc(sizeof(struct allocated_block));
  if(!ab) exit(-5);

  ab->next = NULL;
  pid++;
  sprintf(ab->process_name, "PROCESS-%02d", pid);
  ab->pid = pid;
  printf("memory for %s:",ab->process_name);
  scanf("%d", &size);
  if(size > 0) ab->size = size;
  ret = allocate_mem(ab);

  if((ret == 1) && (allocated_block_head == NULL)){
    allocated_block_head = ab;
  }
  else if(ret == 1){
    ab->next = allocated_block_head;
    allocated_block_head = ab;
    return 2;
  }
  else if(ret == -1){
    printf("Allocation fail\n");
    free(ab);
    return -1;
  }
  return 3;
}



int allocate_mem(struct allocated_block *ab){
  struct free_block_type *fbt, *pre; 
  int request_size=ab->size;
  fbt = pre = free_block;
  


  //////////////////////////////////////////////////////////////////////
  int ret;
  if(fbt == NULL){
    return -1;
  }

  while(f != NULL){

    if(f -> size >= ab->size){
      ret = allocate(pre,f,ab);
      rearrange(ma_algorithm);
      return ret;

    }
    pre = fbt;
    fbt = pre -> next;
  }
  if(f ==NULL && current_size_free > ab -> size){
    ret = mem_retrench(ab);
  }
  else{
    ret = -2;
  }
  return ret;


}

int allocate(struct free_block_type* pre, struct free_block_type* allocate_free_block, struct allocated_block* ab){
  ab -> start_addr = allocate_free_block -> size;
  if(allocate_free_block -> size - ab ->size < MIN_SLICE){
    if(pre != NULL){
      pre->next = allocate_free_block -> next;

    }
    else{
      free_block = allocate_free_block -> next;
    }
    free(allocate_free_block);
    num_free_blocks--;
  }
  else{
    allocate_free_block -> start_addr += ab -> size;
    allocate_free_block -> size -= ab ->size;
  }
  current_size_free -= ab -> size;
  if(current_size_free == 0){
    free_block = NULL;
  }
  return 1;
}

int mem_retrench(struct allocated_block* ab){
  struct allocated_block* allocated_work, allocated_pre = allocated_block_head;
  struct free_block_type* free_r, *free_work = free_block ->next;

  if(allocated_pre == NULL){
    return -1;
  }
  allocated_pre->start_addr=0;
  allocated_work = allocated_pre->next;
  while(allocated_work != NULL){
    allocated_work->start_addr = allocated_pre->start_addr + allocated_pre->size;
    allocated_pre = allocated_work;
    allocated_work = allocated_work->next;
  }
  free_block->start_addr = allocated_pre->start_addr + allocated_pre->size;
  free_block->size = current_size_free;
  free_block->next = NULL;
  free_r = free_work->next;
  while(free_r != NULL){
    free(free_work);
    free_work = free_r;
    free_r = free_r->next;
    if(free_r == NULL){
      free(free_work);
    }
  }
  num_free_blocks = 1;
  allocate(NULL, free_block,ab);
  return 1;

}

void kill_process(){
  struct allocated_block *ab; 
  int pid;
  printf("Kill Process, pid="); scanf("%d", &pid); ab=find_process(pid); 
  if(ab!=NULL){
    free_mem(ab); 
    dispose(ab);}
  }
}

int free_mem(struct allocated_block *ab){
  int algorithm = ma_algorithm;
  struct free_block_type *fbt, *pre, *work;
  fbt=(struct free_block_type*) malloc(sizeof(struct free_block_type));
  if(!fbt) return -1;











  //////////////////////////////////////////////////////////////////////
  return 1;
}
int dispose(struct allocated_block *free_ab){
  struct allocated_block *pre, *ab;
  if(free_ab == allocated_block_head) {
    allocated_block_head = allocated_block_head->next; free(free_ab);
    return 1;
  }
  pre = allocated_block_head;
  ab = allocated_block_head->next; 
  while(ab!=free_ab){ pre = ab; ab = ab->next; } pre->next = ab->next;
  free(ab);

  return 2;   
}
void display_mem_usage(){
  struct free_block_type *fbt=free_block;
  struct allocated_block *ab=allocated_block_head;
  if(fbt==NULL) return(-1); printf("----------------------------------------------------------\n");
  printf("Free Memory:\n");
  printf("%20s %20s\n", " start_addr", " size"); 
  while(fbt!=NULL){
    printf("%20d %20d\n", fbt->start_addr, fbt->size); fbt=fbt->next;
  }

  printf("\nUsed Memory:\n");
  printf("%10s %20s %10s %10s\n", "PID", "ProcessName", "start_addr", " size"); 
  while(ab!=NULL){
    printf("%10d %20s %10d %10d\n", ab->pid, ab->process_name, ab->start_addr, ab->size);
    ab=ab->next;
  } 
  printf("----------------------------------------------------------\n"); 
  return 0;
}


int main(){
  char choice;
  pid = 0;
  free_block = init_free_block(mem_size);
  while(1){
    display_menu();
    fflush(stdin);
    choice = getchar();
    switch(choice){
      case '1' : set_mem_size(); break;
      case '2' : set_algorithm();flag = 1;break;
      case '3' : new_process();flag = 1; break;
      case '4' : kill_process();flag = 1;break;
      case '5' : display_menu_usage(); flag = 1;break;
      case '0' : do_exit();exit(0);
      default : break;
    }
  }





  exit(0);

}


























