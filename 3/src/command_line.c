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


void cwrite(char name[]){
  
  close(name);

  delete(name);

  create(name);

  open(name);

  write(name);
}

void stopWrite() { flag = 0; }
void write(char name[]) {
  unsigned short i = search_file(name);
  if (i == 0)
    printf("The file does not exist.\n");
  else {
    load_inode_entry(i);
    unsigned char permission = ((unsigned char *)(&(inode_buf.i_mode)))[1];
    if (permission == 2 || permission == 6 || permission == 7) {
      if (is_open(i)) {

        unsigned short pos = 0, j = 0;
        if (inode_buf.i_blocks == 0) {
          j = new_block();
          inode_buf.i_blocks = 1;
          inode_buf.i_block[0] = j;
          update_inode_entry(i);
          load_block_entry(j);
          pos = 0;
        } else if (inode_buf.i_blocks <= 6) {
          j = inode_buf.i_block[inode_buf.i_blocks - 1];
          load_block_entry(j);
          pos = inode_buf.i_size % BLOCK_SIZE;
          if (pos == 0) {
            j = add_file_block(i);
            load_block_entry(j);
          }
        } else if (inode_buf.i_blocks == 7) {
          load_block_entry(inode_buf.i_block[6]);
          pos = inode_buf.i_size / BLOCK_SIZE - 6;
          j = ((unsigned short *)Buffer)[pos];
          load_block_entry(j);
          pos = inode_buf.i_size % BLOCK_SIZE;
          if (pos == 0) {
            j = add_file_block(i);
            load_block_entry(j);
          }
        } else {
          load_block_entry(inode_buf.i_block[7]);
          pos = (inode_buf.i_size / BLOCK_SIZE - 6 - 256) / 256;
          load_block_entry(((unsigned short *)Buffer)[pos]);
          pos = (inode_buf.i_size / BLOCK_SIZE - 6 - 256) % 256;
          j = ((unsigned short *)Buffer)[pos];
          load_block_entry(j);
          pos = inode_buf.i_size % BLOCK_SIZE;
          if (pos == 0) {
            j = add_file_block(i);
            load_block_entry(j);
          }
        }

        unsigned short new_size = 0;
        char ch = getchar();
        signal(SIGQUIT, stopWrite);
        while (flag /* ch != EOF */) {
          Buffer[pos] = ch;
          pos++;
          new_size++;
          ch = getchar();
          if (pos == BLOCK_SIZE) {
            update_block_entry(j);
            j = add_file_block(i);
            load_block_entry(j);
            pos = 0;
          }
        }
        flag = 1;
        Buffer[pos] = '\0';
        update_block_entry(j);
        inode_buf.i_size += new_size - 1;
        inode_buf.i_atime = time(NULL);
        inode_buf.i_mtime = time(NULL);
        update_inode_entry(i);
      } else
        printf("The file does not open.\n");
    } else
      printf("You do not have permission to write this file.\n");
  }
}

void check_disk(void) {
  load_group_desc();
  printf("Volume Name: %s\n", gdt.bg_volume_name);
  printf("Block Size: %dBytes\n", BLOCK_SIZE);
  printf("Free Block: %u\n", gdt.bg_free_blocks_count);
  printf("Free Inode: %u\n", gdt.bg_free_inodes_count);
  printf("Directories: %u\n", gdt.bg_used_dirs_count);
}

void format(void) {
  initialize_memory();
  initialize_disk();
  printf("format acomplished.\n");
  check_disk();
}

unsigned short login(char password[]) {
  load_group_desc();
  return !(strcmp(gdt.bg_password, password));
}

void change_password(void) {
  printf("Current password: ");
  char password[10];
  gets_s(password, 9);
  if (login(password)) {
    printf("New password(no more than 9): ");
    gets_s(password, 9);
    char confirm[10];
    printf("Confirm password: ");
    gets_s(confirm, 9);
    if (!strcmp(password, confirm)) {
      load_group_desc();
      strcpy(gdt.bg_password, password);
      update_group_desc();
      printf("Password has changed sccessfully.\n");
    } else {
      printf("The two password not match.\n");
      return;
    }
  } else
    printf("Password error.\n");
}

void dir(void) {
  printf("name      ");
  printf("type    ");
  printf("mode    ");
  printf("size(Bytes)   ");
  printf("creat time                ");
  printf("access time               ");
  printf("modify time               \n");
  access_file(current_dir, print_dir);
  load_inode_entry(current_dir);
  inode_buf.i_atime = time(NULL);
  update_inode_entry(current_dir);
}

void mkdir(char name[]) {
  unsigned short i = search_file(name);
  if (i) {
    printf("A directory with the same name exists.\n");
  } else {
    new_dir_entry(name, FT_DIR);

    if (!access_file(current_dir, search_free_dir_in_block)) {

      unsigned short j = add_file_block(i);
      memset(Buffer, 0, sizeof(Buffer));
      ((dir_entry *)Buffer)[0] = dir_buf;
      update_block_entry(j);
    }
    load_inode_entry(current_dir);
    inode_buf.i_size += (7 + strlen(name));
    update_inode_entry(current_dir);
  }
}

void rmdir(char name[]) {
  if ((!strcmp(name, ".")) || (!strcmp(name, ".."))) {
    printf("bash: %s :command not found.\n", name);
    return;
  }
  unsigned short i = search_file(name);
  if (i) {
    load_inode_entry(i);
    if (((unsigned char *)(&(inode_buf.i_mode)))[0] != FT_DIR) {
      printf("command not found.\n");
      return;
    }
    if (inode_buf.i_size != 17) {
      printf("Cannot delete non empty directory!\n");
      return;
    }

    access_file(current_dir, delete_in_block);
    access_file(i, free_file_block);
    load_inode_entry(i);
    inode_buf.i_dtime = time(NULL);
    update_inode_entry(i);
    free_inode(i);

    load_inode_entry(current_dir);
    inode_buf.i_size -= (7 + strlen(name));
    update_inode_entry(current_dir);

    load_group_desc();
    gdt.bg_used_dirs_count--;
    update_group_desc();
  } else {
    printf("The directory does not exist.\n");
  }
}

void create(char name[]) {
  unsigned short i = search_file(name);
  if (i) {
    printf("A file with the same name exists.\n");
  } else {
    new_dir_entry(name, FT_REG_FILE);

    if (!access_file(current_dir, search_free_dir_in_block)) {

      unsigned short j = add_file_block(i);
      memset(Buffer, 0, sizeof(Buffer));
      ((dir_entry *)Buffer)[0] = dir_buf;
      update_block_entry(j);
    }
    load_inode_entry(current_dir);
    inode_buf.i_size += (7 + strlen(name));
    update_inode_entry(current_dir);
  }
}

void delete(char name[]) {
  unsigned short i = search_file(name);
  if (i) {
    load_inode_entry(i);
    if (((unsigned char *)(&(inode_buf.i_mode)))[0] != FT_REG_FILE) {
      printf("bash : %s : command not found.\n", name);
      return;
    }
    if (is_open(i))
      printf("The file is in use! Please close it first.\n");
    else {
      access_file(current_dir, delete_in_block);

      access_file(i, free_file_block);
      load_inode_entry(i);
      inode_buf.i_dtime = time(NULL);
      update_inode_entry(i);
      free_inode(i);

      load_inode_entry(current_dir);
      inode_buf.i_size -= (7 + strlen(name));
      update_inode_entry(current_dir);
    }
  } else {
    printf("The file does not exist.\n");
  }
}

void cd(char path[]) {
  if (!strcmp(path, "") || !strcmp(path, "~")) {
    current_dir = 1;
    strcpy(current_path, "root");
  } else if (!strcmp(path, ".")) {
    ;
  } else if (!strcmp(path, "..")) {
    load_inode_entry(current_dir);
    load_block_entry(inode_buf.i_block[0]);
    current_dir = ((dir_entry *)(Buffer + 8))[0].inode;
    for (int k = strlen(current_path); k >= 0; --k) {
      if (current_path[k] == '/') {
        current_path[k] = '\0';
        break;
      }
    }
  } else {
    unsigned short i = search_file(path);
    if (i) {
      load_inode_entry(i);
      if (((unsigned char *)(&(inode_buf.i_mode)))[0] != FT_DIR) {
        printf("No such directory exist.\n");
        return;
      }
      current_dir = i;
      strcat(current_path, "/");
      strcat(current_path, path);
    } else {
      printf("No such directory exist.\n");
    }
  }
}

void attrib(char name[], unsigned char change) {
  unsigned short i = search_file(name);
  if (i == 0)
    printf("The file does not exist.\n");
  else {
    if (change == 2 || change == 4 || change == 6 || change == 7 ||change == 5 || change ==1) {
      load_inode_entry(i);
      ((unsigned char *)(&(inode_buf.i_mode)))[1] = change;
      inode_buf.i_atime = time(NULL);
      inode_buf.i_mtime = time(NULL);
      update_inode_entry(i);
    } else
      printf("illigal change.\n");
  }
}

void open(char name[]) {
  unsigned short inode_num = search_file(name);
  if (inode_num == 0)
    printf("The file does not exist.\n");
  else {
    if (is_open(inode_num))
      printf("The file has opened.\n");
    else {
      load_inode_entry(inode_num);
      unsigned char permission = ((unsigned char *)(&(inode_buf.i_mode)))[1];
      if (permission == 4 || permission == 6 || permission == 7) {
        inode_buf.i_atime = time(NULL);
        update_inode_entry(inode_num);
        for (unsigned short i = 0; i < FOPEN_TABLE_MAX; ++i) {
          if (fopen_table[i] == 0) {
            fopen_table[i] = inode_num;
            return;
          }
        }
        printf("The number of files opened has reached the maximum.\n");
      } else
        printf("You do not have permission to open this file.\n");
    }
  }
}

void close(char name[]) {
  unsigned short inode_num = search_file(name);
  if (inode_num == 0)
    printf("The file does not exist.\n");
  else {
    if (is_open(inode_num)) {
      load_inode_entry(inode_num);
      inode_buf.i_atime = time(NULL);
      update_inode_entry(inode_num);
      for (unsigned short i = 0; i < FOPEN_TABLE_MAX; ++i) {
        if (fopen_table[i] == inode_num) {
          fopen_table[i] = 0;
          return;
        }
      }
    } else
      printf("The file does not open.\n");
  }
}

void read(char name[]) {
  unsigned short i = search_file(name);
  if (i == 0)
    printf("The file does not exist.\n");
  else {
    load_inode_entry(i);
    unsigned char permission = ((unsigned char *)(&(inode_buf.i_mode)))[1];
    if (permission == 4 || permission == 6 || permission == 7) {
      if (is_open(i)) {
        access_file(i, print_file);
        inode_buf.i_atime = time(NULL);
        update_inode_entry(i);
      } else
        printf("The file does not open.\n");
    } else
      printf("You do not have permission to read this file.\n");
  }
}
