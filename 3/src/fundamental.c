#include "IO_operation.h"
#include "basic.h"
#include "command_line.h"
#include "fundamental.h"
#include "initialize.h"
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int bitmap_find_0(unsigned char bitmap[]) {
  for (int i = 0; i < BLOCK_SIZE; ++i) {
    unsigned char mask = 0b10000000;
    if (bitmap[i] == 0b11111111)
      continue;
    for (int j = 0; j < 8; ++j) {
      if (!(bitmap[i] & mask))
        return i * 8 + j;
      mask >>= 1;
    }
  }
  return -1;
}

void bitmap_neg_k(unsigned char bitmap[], int k) {
  int i = k / 8, j = k % 8;
  unsigned char mask = 0b10000000;
  for (int t = 0; t < j; ++t)
    mask >>= 1;
  bitmap[i] ^= mask;
}

void initialize_inode(void) {
  inode_buf.i_mode = FT_UNKNOWN;
  inode_buf.i_mode <<= 8;

  ((unsigned char *)(&(inode_buf.i_mode)))[1] = 0b00000110;
  inode_buf.i_blocks = 0;
  inode_buf.i_size = 0;
  inode_buf.i_atime = time(NULL);
  inode_buf.i_ctime = time(NULL);
  inode_buf.i_mtime = time(NULL);
  inode_buf.i_dtime = 0;
  memset(inode_buf.i_block, 0, sizeof(inode_buf.i_block));
  memset(inode_buf.i_pad, 0xff, sizeof(inode_buf.i_pad));
}

unsigned short new_inode(void) {
  load_group_desc();
  if (gdt.bg_free_inodes_count) {

    load_inode_bitmap();
    unsigned short i = bitmap_find_0(inode_bitmap);
    bitmap_neg_k(inode_bitmap, i);
    update_inode_bitmap();

    initialize_inode();
    update_inode_entry(i + 1);

    gdt.bg_free_inodes_count--;
    update_group_desc();
    return i + 1;
  } else {
    printf("There is no inode to be allocated!\n");
    return 0;
  }
}

void free_inode(unsigned short i) {

  load_inode_bitmap();
  bitmap_neg_k(inode_bitmap, i - 1);
  update_inode_bitmap();

  load_group_desc();
  gdt.bg_free_inodes_count++;
  update_group_desc();
}

unsigned short new_block(void) {
  load_group_desc();
  if (gdt.bg_free_blocks_count) {

    load_block_bitmap();
    unsigned short i = bitmap_find_0(block_bitmap);
    bitmap_neg_k(block_bitmap, i);
    update_block_bitmap();

    gdt.bg_free_blocks_count--;
    update_group_desc();
    return i;
  } else {
    printf("There is no block to be allocated!\n");
    return 0;
  }
}

void free_block(unsigned short i) {

  load_block_bitmap();
  bitmap_neg_k(block_bitmap, i);
  update_block_bitmap();

  load_group_desc();
  gdt.bg_free_blocks_count++;
  update_group_desc();
}

void new_dir_entry(char file_name[], unsigned char file_type) {
  memset(&dir_buf, 0, sizeof(dir_entry));
  dir_buf.inode = new_inode();
  dir_buf.name_len = strlen(file_name);
  dir_buf.rec_len = 7 + dir_buf.name_len;
  dir_buf.file_type = file_type;
  strcpy(dir_buf.name, file_name);

  load_inode_entry(dir_buf.inode);
  ((unsigned char *)(&(inode_buf.i_mode)))[0] = file_type;
  char *extension = strchr(file_name, '.');
  if (!extension || !strcmp(extension, ".exe") || !strcmp(extension, ".bin") ||
      !strcmp(extension, ".com"))
    ((unsigned char *)(&(inode_buf.i_mode)))[1] = 0b00000111;
  update_inode_entry(dir_buf.inode);

  if (file_type == FT_DIR) {

    load_group_desc();
    gdt.bg_used_dirs_count++;
    update_group_desc();

    unsigned short i = new_block();
    memset(Buffer, 0, sizeof(Buffer));
    dir_entry temp;

    memset(&temp, 0, sizeof(dir_entry));
    temp.inode = dir_buf.inode;
    temp.rec_len = 8;
    temp.name_len = 1;
    temp.file_type = FT_DIR;
    strcpy(temp.name, ".");
    ((dir_entry *)Buffer)[0] = temp;

    memset(&temp, 0, sizeof(dir_entry));
    temp.inode = current_dir;
    temp.rec_len = 9;
    temp.name_len = 2;
    temp.file_type = FT_DIR;
    strcpy(temp.name, "..");
    ((dir_entry *)(Buffer + 8))[0] = temp;

    update_block_entry(i);

    load_inode_entry(dir_buf.inode);
    inode_buf.i_blocks = 1;
    inode_buf.i_size = 17;
    inode_buf.i_block[0] = i;
    ((unsigned char *)(&(inode_buf.i_mode)))[1] = 0b00000110;
    update_inode_entry(dir_buf.inode);
  }
}

unsigned short is_open(unsigned short inode_num) {
  for (int i = 0; i < FOPEN_TABLE_MAX; ++i)
    if (fopen_table[i] == inode_num)
      return 1;
  return 0;
}

unsigned short access_file(unsigned short inode_num,
                           unsigned short (*func)(unsigned short)) {

  load_inode_entry(inode_num);
  unsigned short indirect_1 = 0, indirect_2 = 0;
  for (unsigned short i = 0; i < inode_buf.i_blocks; /**/) {
    if (i < 6) {

      load_block_entry(inode_buf.i_block[i]);
      unsigned short ret = func(inode_buf.i_block[i]);
      update_block_entry(inode_buf.i_block[i]);
      ++i;
      if (ret)
        return ret;
    } else if (i == 6) {

      load_block_entry(inode_buf.i_block[i]);
      unsigned short j;
      if (indirect_1 < BLOCK_SIZE / sizeof(unsigned short))
        j = ((unsigned short *)Buffer)[indirect_1];
      if (j == 0 || indirect_1 == BLOCK_SIZE / sizeof(unsigned short)) {
        ++i;
        indirect_1 = 0;
        continue;
      }
      ++indirect_1;
      load_block_entry(j);
      unsigned short ret = func(j);
      update_block_entry(j);
      if (ret)
        return ret;
    } else {

      load_block_entry(inode_buf.i_block[i]);
      unsigned short j, k;
      if (indirect_1 < BLOCK_SIZE / sizeof(unsigned short))
        j = ((unsigned short *)Buffer)[indirect_1];
      if (j == 0 || indirect_1 == BLOCK_SIZE / sizeof(unsigned short))
        break;
      load_block_entry(j);
      if (indirect_2 < BLOCK_SIZE / sizeof(unsigned short))
        k = ((unsigned short *)Buffer)[indirect_2];
      if (k == 0)
        break;
      if (indirect_2 == BLOCK_SIZE / sizeof(unsigned short)) {
        ++indirect_1;
        indirect_2 = 0;
        continue;
      }
      ++indirect_2;
      load_block_entry(k);
      unsigned short ret = func(k);
      update_block_entry(k);
      if (ret)
        return ret;
    }
  }
  return 0;
}

unsigned short search_free_dir_in_block(unsigned short block) {
  unsigned short current_pos = 0;

  memset(Buffer + BLOCK_SIZE, 0, BLOCK_SIZE);
  dir_entry temp = ((dir_entry *)Buffer)[0];
  do {
    unsigned short k = temp.rec_len - temp.name_len - 7;
    if (k >= dir_buf.rec_len) {
      temp.rec_len = temp.name_len + 7;
      ((dir_entry *)(Buffer + current_pos))[0].rec_len = temp.rec_len;
      dir_buf.rec_len = k;
      ((dir_entry *)(Buffer + current_pos + temp.rec_len))[0].inode =
          dir_buf.inode;
      ((dir_entry *)(Buffer + current_pos + temp.rec_len))[0].rec_len =
          dir_buf.rec_len;
      ((dir_entry *)(Buffer + current_pos + temp.rec_len))[0].name_len =
          dir_buf.name_len;
      ((dir_entry *)(Buffer + current_pos + temp.rec_len))[0].file_type =
          dir_buf.file_type;
      strcpy(((dir_entry *)(Buffer + current_pos + temp.rec_len))[0].name,
             dir_buf.name);
      return 1;
    }
    current_pos += temp.rec_len;
    temp = ((dir_entry *)(Buffer + current_pos))[0];
  } while (temp.inode);
  if (BLOCK_SIZE - current_pos > dir_buf.rec_len) {
    ((dir_entry *)(Buffer + current_pos))[0] = dir_buf;
    return 1;
  }
  return 0;
}

unsigned short search_in_block(unsigned short block) {
  unsigned short current_pos = 0;
  memset(Buffer + BLOCK_SIZE, 0, BLOCK_SIZE);
  dir_buf = ((dir_entry *)Buffer)[0];
  do {
    if (!strcmp(dir_buf.name, search_file_name))
      return dir_buf.inode;
    current_pos += dir_buf.rec_len;
    dir_buf = ((dir_entry *)(Buffer + current_pos))[0];
  } while (dir_buf.inode);
  return 0;
}

unsigned short search_file(char name[]) {
  strcpy(search_file_name, name);
  return access_file(current_dir, search_in_block);
}

unsigned short delete_in_block(unsigned short block) {
  unsigned short current_pos = 0, pre_pos = 0;

  memset(Buffer + BLOCK_SIZE, 0, BLOCK_SIZE);
  dir_buf = ((dir_entry *)Buffer)[0];
  do {
    if (!strcmp(dir_buf.name, search_file_name)) {
      ((dir_entry *)(Buffer + pre_pos))[0].rec_len += dir_buf.rec_len;
      return dir_buf.inode;
    }
    pre_pos = current_pos;
    current_pos += dir_buf.rec_len;
    dir_buf = ((dir_entry *)(Buffer + current_pos))[0];
  } while (dir_buf.inode);
  return 0;
}

unsigned short free_file_block(unsigned short block) {
  free_block(block);
  return 0;
}

unsigned short print_file(unsigned short block) {
  for (unsigned short i = 0; i < BLOCK_SIZE; ++i) {
    if (Buffer[i] == 0)
      return 1;
    putchar(Buffer[i]);
  }
  return 0;
}

unsigned short print_dir(unsigned short block) {
  inode inode_temp = inode_buf;
  unsigned short current_pos = 0;

  memset(Buffer + BLOCK_SIZE, 0, BLOCK_SIZE);
  dir_buf = ((dir_entry *)Buffer)[0];
  do {
    load_inode_entry(dir_buf.inode);
    printf("%-10s", dir_buf.name);
    switch (dir_buf.file_type) {
    case FT_DIR:
      printf("<DIR>   ");
      break;
    case FT_REG_FILE:
      printf("<FILE>  ");
      break;
    default:
      printf("unknown ");
    }
    switch (((unsigned char *)(&(inode_buf.i_mode)))[1]) {
    case 2:
      printf("__w__    ");
      break;
    case 4:
      printf("r____    ");
      break;
    case 6:
      printf("r_w__    ");
      break;
    case 7:
      printf("r_w_x    ");
      break;
    default:
      printf("error  ");
    }
    printf("%-12hu", inode_buf.i_size);
    time_t temp;
    char time_str[26];
    temp = inode_buf.i_ctime;
    strcpy(time_str, ctime(&temp));
    time_str[24] = '\0';
    printf("%s  ", time_str);
    temp = inode_buf.i_atime;
    strcpy(time_str, ctime(&temp));
    time_str[24] = '\0';
    printf("%s  ", time_str);
    temp = inode_buf.i_mtime;
    strcpy(time_str, ctime(&temp));
    time_str[24] = '\0';
    printf("%s  \n", time_str);
    current_pos += dir_buf.rec_len;
    dir_buf = ((dir_entry *)(Buffer + current_pos))[0];
  } while (dir_buf.inode);
  inode_buf = inode_temp;
  return 0;
}

unsigned short add_file_block(unsigned short inode_num) {
  unsigned short i = 0;
  load_inode_entry(inode_num);
  if (inode_buf.i_blocks < 6) {
    i = new_block();
    inode_buf.i_blocks++;
    inode_buf.i_block[inode_buf.i_blocks - 1] = i;
  } else if (inode_buf.i_blocks == 6) {
    i = new_block();
    inode_buf.i_blocks++;
    inode_buf.i_block[inode_buf.i_blocks - 1] = i;
    i = new_block();
    memset(Buffer, 0, sizeof(Buffer));
    ((unsigned short *)Buffer)[0] = i;
    update_block_entry(inode_buf.i_block[inode_buf.i_blocks - 1]);
  } else if (inode_buf.i_blocks == 7) {
    load_block_entry(inode_buf.i_block[6]);
    unsigned short j;
    for (j = 0; j < 256 && ((unsigned short *)Buffer)[j]; ++j)
      ;
    if (j == 256) {
      i = new_block();
      inode_buf.i_blocks++;
      inode_buf.i_block[inode_buf.i_blocks - 1] = i;
      i = new_block();
      memset(Buffer, 0, sizeof(Buffer));
      ((unsigned short *)Buffer)[0] = i;
      update_block_entry(inode_buf.i_block[inode_buf.i_blocks - 1]);
      unsigned short k = new_block();
      memset(Buffer, 0, sizeof(Buffer));
      ((unsigned short *)Buffer)[0] = k;
      update_block_entry(i);
      i = k;
    } else {
      i = new_block();
      ((unsigned short *)Buffer)[j] = i;
    }
    update_block_entry(inode_buf.i_block[6]);
  } else {
    load_block_entry(inode_buf.i_block[7]);
    unsigned short j, k;
    for (j = 0; j < 256 && ((unsigned short *)Buffer)[j]; ++j)
      ;
    --j;
    load_block_entry(j);
    for (k = 0; k < 256 && ((unsigned short *)Buffer)[k]; ++k)
      ;
    if (k < 256) {
      i = new_block();
      ((unsigned short *)Buffer)[k] = i;
      update_block_entry(j);
    } else {
      if (j < 255) {
        load_block_entry(inode_buf.i_block[7]);
        ++j;
        unsigned short temp = new_block();
        ((unsigned short *)Buffer)[j] = temp;
        update_block_entry(inode_buf.i_block[7]);
        memset(Buffer, 0, sizeof(Buffer));
        i = new_block();
        ((unsigned short *)Buffer)[0] = i;
        update_block_entry(temp);
      } else
        printf("The file has reached the maximum capacity!\n");
    }
  }
  update_inode_entry(inode_num);
  return i;
}

char *gets_s(char *buffer, int num) {
  if (fgets(buffer, num, stdin) != 0) {
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n')
      buffer[len - 1] = '\0';
    return buffer;
  }
  return NULL;
}
