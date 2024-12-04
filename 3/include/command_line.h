
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


unsigned short login(char password[]);

void change_password(void);

void dir(void);

void mkdir(char name[]);


void rmdir(char name[]);

void create(char name[]);

void delete(char name[]);

void cd(char path[]);

void attrib(char name[], unsigned char change);


void open(char name[]);

void close(char name[]);

void read(char name[]);

void stopWrite();

void write(char name[]);

void check_disk(void);

void format(void);


