#include "IO_operation.h"
#include "basic.h"
#include "command_line.h"
#include "fundamental.h"
#include "initialize.h"
#include "main_function.h"
#include "user_interface.h"
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void main_function(void) {
  fp = fopen("./Ext2", "rb+");

  initialize_memory();

  if (fp == NULL) {
    fp = fopen("./Ext2", "wb+");
    initialize_disk();
  }

  printf("Password: ");
  char password[10];
  gets_s(password, 9);
  while (!login(password)) {
    printf("Error!\n");
    printf("Password: ");
    gets_s(password, 9);
  }

  printf("\nEXT2 file system is booting ...\n");
  printf("\nsccessful boot.\n");
  shell();
  fclose(fp);

  return;
}
