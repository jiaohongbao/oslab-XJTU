#include"basic.h"

group_desc gdt;
inode inode_buf;
dir_entry dir_buf;
unsigned char block_bitmap[BLOCK_SIZE];
unsigned char inode_bitmap[BLOCK_SIZE];
unsigned char gdt_Buffer[BLOCK_SIZE];
unsigned char inode_Buffer[BLOCK_SIZE];
unsigned char Buffer[BLOCK_SIZE * 2];

FILE *fp = NULL;
char current_path[256];
unsigned short current_dir;
unsigned short fopen_table[FOPEN_TABLE_MAX];
char search_file_name[EXT2_NAME_LEN];
int flag = 1;
