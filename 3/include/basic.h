#ifndef GLOBAL_DEFINITIONS_H
#define GLOBAL_DEFINITIONS_H

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <time.h>

// Constants and macros
#define VOLUME_NAME "EXT2FS"
#define EXT2_NAME_LEN 255
#define FOPEN_TABLE_MAX 16
#define BLOCK_SIZE 512
#define DATA_BLOCK_COUNT 4096
#define INODE_COUNT 4096
#define DISK_SIZE 4611
#define READ_DISK 1
#define WRITE_DISK 0
#define GDT_START 0
#define BLOCK_BITMAP_START 512
#define INODE_BITMAP_START 1024
#define INODE_TABLE_START 1536
#define DATA_BLOCK_START (1536 + 512 * 512)

// Enumerations
enum {
    FT_UNKNOWN,
    FT_REG_FILE,
    FT_DIR,
    FT_CHRDEV,
    FT_BLKDEV,
    FT_FIFO,
    FT_SOCK,
    FT_SYMLINK,
};

// Struct definitions
typedef struct group_desc {
    char bg_volume_name[10];
    unsigned short bg_block_bitmap;
    unsigned short bg_inode_bitmap;
    unsigned short bg_inode_table;
    unsigned short bg_free_blocks_count;
    unsigned short bg_free_inodes_count;
    unsigned short bg_used_dirs_count;
    char bg_password[10];
} group_desc;

typedef struct inode {
    unsigned short i_mode;
    unsigned short i_blocks;
    unsigned int i_size;
    unsigned int i_atime;
    unsigned int i_ctime;
    unsigned int i_mtime;
    unsigned int i_dtime;
    unsigned short i_block[8];
    char i_pad[24];
} inode;

typedef struct dir_entry {
    unsigned short inode;
    unsigned short rec_len;
    unsigned char name_len;
    unsigned char file_type;
    char name[EXT2_NAME_LEN];
} dir_entry;

// Global variables
extern group_desc gdt;
extern inode inode_buf;
extern dir_entry dir_buf;
extern unsigned char block_bitmap[BLOCK_SIZE];
extern unsigned char inode_bitmap[BLOCK_SIZE];
extern unsigned char gdt_Buffer[BLOCK_SIZE];
extern unsigned char inode_Buffer[BLOCK_SIZE];
extern unsigned char Buffer[BLOCK_SIZE * 2];

extern FILE *fp;
extern char current_path[256];
extern unsigned short current_dir;
extern unsigned short fopen_table[FOPEN_TABLE_MAX];
extern char search_file_name[EXT2_NAME_LEN];
extern int flag;

#endif // GLOBAL_DEFINITIONS_H
