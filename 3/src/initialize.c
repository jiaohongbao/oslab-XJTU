#include "IO_operation.h"
#include"basic.h"
#include "command_line.h"
#include "fundamental.h"
#include "initialize.h"
#include "user_interface.h"
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void initialize_memory(void) {

  memset(fopen_table, 0, sizeof(fopen_table));

  strcpy(gdt.bg_volume_name, VOLUME_NAME);
  strcpy(gdt.bg_password, "ext2");
  gdt.bg_block_bitmap = 1;
  gdt.bg_inode_bitmap = 2;
  gdt.bg_inode_table = 3;
  gdt.bg_free_blocks_count = DATA_BLOCK_COUNT;
  gdt.bg_free_inodes_count = INODE_COUNT;
  gdt.bg_used_dirs_count = 0;
  current_dir = 1;
  strcpy(current_path, "root");
}

void initialize_disk(void) {
  memset(Buffer, 0, sizeof(Buffer));
  for (int i = 0; i < DISK_SIZE; ++i) {
    disk_IO(i * BLOCK_SIZE, Buffer, WRITE_DISK);
  }
  update_group_desc();

  new_dir_entry("root", FT_DIR);
}
