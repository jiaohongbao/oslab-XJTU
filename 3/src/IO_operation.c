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

void disk_IO(long offset, void *buf, int rw_flag) {
  fseek(fp, offset, SEEK_SET);
  if (rw_flag) {
    fread(buf, BLOCK_SIZE, 1, fp);
  } else {
    fwrite(buf, BLOCK_SIZE, 1, fp);
    fflush(fp);
  }
}

void load_group_desc(void) {
  disk_IO(GDT_START, gdt_Buffer, READ_DISK);
  gdt = ((group_desc *)gdt_Buffer)[0];
}

void update_group_desc(void) {
  memset(gdt_Buffer, 0xff, sizeof(gdt_Buffer));
  ((group_desc *)gdt_Buffer)[0] = gdt;
  disk_IO(GDT_START, gdt_Buffer, WRITE_DISK);
}

void load_inode_entry(unsigned short k) {
  --k;
  unsigned short i = k / 8, j = k % 8;
  disk_IO(INODE_TABLE_START + i * BLOCK_SIZE, inode_Buffer, READ_DISK);
  inode_buf = ((inode *)inode_Buffer)[j];
}

void update_inode_entry(unsigned short k) {
  --k;
  unsigned short i = k / 8, j = k % 8;
  disk_IO(INODE_TABLE_START + i * BLOCK_SIZE, inode_Buffer, READ_DISK);
  ((inode *)inode_Buffer)[j] = inode_buf;
  disk_IO(INODE_TABLE_START + i * BLOCK_SIZE, inode_Buffer, WRITE_DISK);
}

void load_block_entry(unsigned short i) {
  disk_IO(DATA_BLOCK_START + i * BLOCK_SIZE, Buffer, READ_DISK);
}

void update_block_entry(unsigned short i) {
  disk_IO(DATA_BLOCK_START + i * BLOCK_SIZE, Buffer, WRITE_DISK);
}

void load_block_bitmap(void) {
  disk_IO(BLOCK_BITMAP_START, block_bitmap, READ_DISK);
}

void update_block_bitmap(void) {
  disk_IO(BLOCK_BITMAP_START, block_bitmap, WRITE_DISK);
}

void load_inode_bitmap(void) {
  disk_IO(INODE_BITMAP_START, inode_bitmap, READ_DISK);
}

void update_inode_bitmap(void) {
  disk_IO(INODE_BITMAP_START, inode_bitmap, WRITE_DISK);
}
