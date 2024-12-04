#include "IO_operation.h"
#include "basic.h"
#include "command_line.h"
#include "fundamental.h"
#include "initialize.h"
#include "user_interface.h"
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void shell(void) {
  char cmd[256] = "";
  while (1) {
    printf("[%s]# ", current_path);
    gets_s(cmd, 256);
    if (!strcmp(cmd, "format")) {
      format();
    } else if (!strcmp(cmd, "check")) {
      check_disk();
    } else if (!strcmp(cmd, "passwd")) {
      change_password();
    } else if (!strcmp(cmd, "ls")) {
      dir();
    } else if (!strncmp(cmd, "mkdir ", 6)) {
      mkdir(cmd + 6);
    } else if (!strncmp(cmd, "rmdir ", 6)) {
      rmdir(cmd + 6);
    } else if (!strncmp(cmd, "create ", 7)) {
      create(cmd + 7);
    } else if (!strncmp(cmd, "delete ", 7)) {
      delete (cmd + 7);
    } else if (!strncmp(cmd, "cd ", 3)) {
      cd(cmd + 3);
    } else if (!strncmp(cmd, "chmod ", 6)) {
      printf("modification: ");
      unsigned char change;
      scanf("%hhu", &change);
      getchar();
      attrib(cmd + 6, change);
    } else if (!strncmp(cmd, "open ", 5)) {
      open(cmd + 5);
    } else if (!strncmp(cmd, "close ", 6)) {
      close(cmd + 6);
    } else if (!strncmp(cmd, "read ", 5)) {
      read(cmd + 5);
    } else if (!strncmp(cmd, "write ", 6)) {
      write(cmd + 6);
    } else if (!strcmp(cmd, "quit")) {
      break;
    } else {
      printf("bash : %s : command not found.\n", cmd);
    }
  }
}
