#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
void disk_IO(long offset, void *buf, int rw_flag);

void load_group_desc(void);

void update_group_desc(void);

void load_inode_entry(unsigned short k);

void update_inode_entry(unsigned short k);

void load_block_entry(unsigned short i);

void update_block_entry(unsigned short i);

void load_block_bitmap(void);

void update_block_bitmap(void);

void load_inode_bitmap(void);

void update_inode_bitmap(void);
