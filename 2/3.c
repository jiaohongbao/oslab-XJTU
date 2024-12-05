#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct page {
  unsigned int frame;
  bool flag;
  unsigned int counter;
} page;

page *page_table = NULL;
unsigned int *reference = NULL;
unsigned int table_size = 10;
unsigned int frame_size = 5;
unsigned int ref_size = 20;
unsigned int belady[12] = {1, 2, 3, 4, 1, 2, 5, 5, 5, 5, 5, 5};
unsigned int belady2[13] = {1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 1, 2, 3};

void initial_page_table() {
  page_table = (page *)malloc(table_size * sizeof(page));
  if (!page_table) {
    printf("page_table: malloc failed!\n");
    exit(0);
  }
  for (unsigned int i = 0; i < table_size; i++) {
    page_table[i].flag = false;
    page_table[i].counter = 0;
  }
}

void initial_reference() {
  reference = (unsigned int *)malloc(ref_size * sizeof(unsigned int));
  if (!reference) {
    printf("reference: malloc failed!\n");
    exit(0);
  }
  // srand((unsigned int)time(0));
  for (unsigned int i = 0; i < ref_size; i++)
    reference[i] = rand() % table_size;
}

void print_reference() {
  printf("\nreference:\n");
  for (unsigned int i = 0; i < ref_size; i++) {
    printf("%u ", reference[i]);
  }
  printf("\n");
}

void release() {
  if (page_table)
    free(page_table);
  if (reference && reference != belady)
    free(reference);
}

void Belady(void) {
  ref_size = 12;
  frame_size = 3;
  table_size = 6;
  reference = belady;
}
void Belady2(void) {
  ref_size = 13;
  frame_size = 4;
  table_size = 6;
  reference = belady2;
}

int FIFO(void) {

  unsigned int diseffect = 0;
  unsigned int front = 0;
  unsigned int rear = 0;
  unsigned int queue_size = frame_size + 1;
  unsigned int *queue =
      (unsigned int *)malloc(queue_size * sizeof(unsigned int));
  if (!queue) {
    printf("queue: malloc failed!\n");
    exit(0);
  }

  for (unsigned int i = 0; i < ref_size; i++) {
    printf("Page %u arrives ", reference[i]);

    if (!page_table[reference[i]].flag) {
      printf("and not exist.\n");
      diseffect++;
      if ((rear + 1) % queue_size == front) {

        page_table[queue[front]].flag = false;
        front = (front + 1) % queue_size;
      }

      page_table[reference[i]].flag = true;
      page_table[reference[i]].frame = rear;
      queue[rear] = reference[i];
      rear = (rear + 1) % queue_size;

      printf("frame: ");
      for (unsigned int k = front; k != rear; k = (k + 1) % queue_size)
        printf("|%u| ", queue[k]);
      printf("\n");
    } else {

      printf("and already in.\n");
    }
  }

  free(queue);
  return diseffect;
}

int find_min(unsigned int *frame) {
  unsigned int min = UINT_MAX, frame_num = 0;
  for (unsigned int i = 0; i < frame_size; i++) {
    if (page_table[frame[i]].counter < min) {
      min = page_table[frame[i]].counter;
      frame_num = i;
    }
  }
  return frame_num;
}

unsigned int LRU(void) {
  unsigned int diseffect = 0;
  unsigned int clock = 0;
  unsigned int full = 0;
  unsigned int *frame =
      (unsigned int *)malloc(frame_size * sizeof(unsigned int));
  if (!frame) {
    printf("frame: malloc failed!\n");
    exit(0);
  }

  for (unsigned int i = 0; i < ref_size; i++) {
    printf("Page %u arrives ", reference[i]);

    if (clock == UINT_MAX) {
      unsigned int min;
      unsigned int *temp =
          (unsigned int *)malloc(frame_size * sizeof(unsigned int));
      for (unsigned int i = 0; i < frame_size; i++) {
        min = find_min(frame);
        temp[i] = frame[min];
        page_table[frame[min]].counter = UINT_MAX;
      }
      for (unsigned int i = 0; i < frame_size; i++)
        page_table[temp[i]].counter = i;
      free(temp);
      clock = frame_size;
    }
    page_table[reference[i]].counter = clock;
    clock++;

    if (!page_table[reference[i]].flag) {
      printf("and not exist.\n");
      diseffect++;
      if (full < frame_size) {

        page_table[reference[i]].flag = true;
        page_table[reference[i]].frame = full;
        frame[full] = reference[i];
        full++;
      } else {
        unsigned int replace = find_min(frame);

        page_table[frame[replace]].flag = false;

        page_table[reference[i]].flag = true;
        page_table[reference[i]].frame = replace;
        frame[replace] = reference[i];
      }

      printf("frame: ");
      for (unsigned int k = 0; k < full; k++)
        printf("|%u| ", frame[k]);
      printf("\n");
    } else {

      printf("and already in.\n");
    }
  }

  free(frame);
  return diseffect;
}

char set_parameter(void) {

  printf("\noptions: 1.FIFO      2.LRU      3.Belady      4.Belady2\n");
  char choice = getchar();
  while (getchar() != '\n')
    ;
  if (choice == '3') {
    Belady();
    return choice;
  }
  if (choice == '4') {
    Belady2();
    // printf("!\n");
    print_reference();
    return choice;
  }

  unsigned int size = 0;

  printf("size of the page_table: ");
  if (scanf("%u", &size) == 1 && size > 0 && size <= 128)
    table_size = size;
  else {
    table_size = 10;
    printf("default = 10\n");
  }
  while (getchar() != '\n')
    ;

  printf("size of the frame: ");
  if (scanf("%u", &size) == 1 && size > 0 && size <= 32)
    frame_size = size;
  else {
    frame_size = 4;
    printf("default = 4\n");
  }
  while (getchar() != '\n')
    ;

  printf("size of the reference: ");
  if (scanf("%u", &size) == 1 && size >= 0 && size <= 100)
    ref_size = size;
  else {
    ref_size = 20;
    printf("default = 20\n");
  }
  while (getchar() != '\n')
    ;

  return choice;
}

int main(void) {

  printf("page replacement\n");

  char choice1 = set_parameter();
  if (choice1 != '3' && choice1 != '4') {
    initial_reference();
    // printf("fun");
  }

  unsigned int diseffect;

  while (true) {
    print_reference();
    initial_page_table();

    switch (choice1) {
    case '1':
      printf("\nFIFO:\n\n");
      diseffect = FIFO();
      //  printf("fifo");

      break;
    case '2':
      printf("\nLRU:\n\n");
      diseffect = LRU();
      // printf("lru");
      break;
    case '3':
      printf("\nFIFO(Belady):\n\n");
      diseffect = FIFO();
      break;
    case '4':
      printf("\nFIFO(Belady2):\n\n");
      diseffect = FIFO();
      break;
    default:
      printf("\ndefault = FIFO:\n\n");
      diseffect = FIFO();
    }

    printf("\nPage fault: %d\n", diseffect);
    printf("hit ratio: %.2f%%\n", (1.0 - (float)diseffect / ref_size) * 100.0);
    printf("page fault ratio %.2f%%\n", ((float)diseffect / ref_size) * 100);
    break;
  }

  return 0;
}
