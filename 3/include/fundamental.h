#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int bitmap_find_0(unsigned char bitmap[]);

void bitmap_neg_k(unsigned char bitmap[], int k);

void initialize_inode(void);

unsigned short new_inode(void);

void free_inode(unsigned short i);

unsigned short new_block(void);

void free_block(unsigned short i);

void new_dir_entry(char file_name[], unsigned char file_type);

unsigned short is_open(unsigned short inode_num);

unsigned short access_file(unsigned short inode_num,
                           unsigned short (*func)(unsigned short));

unsigned short search_free_dir_in_block(unsigned short block);

unsigned short search_in_block(unsigned short block);

unsigned short search_file(char name[]);

unsigned short delete_in_block(unsigned short block);

unsigned short free_file_block(unsigned short block);

unsigned short print_file(unsigned short block);

unsigned short print_dir(unsigned short block);

unsigned short add_file_block(unsigned short inode_num);

char *gets_s(char *buffer, int num);
