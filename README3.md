# lab 3 : EXT2 like file-system

Jiao hongbao

2024/11/27


---------------------------------------------------------

## realize functions

in this lab, we need to achieve a ext2-file-system, successfully receive some simple command, and response correctly.

### function realizing

to achieve this file-system, I deivide this problem into four layers, they are: **fundamental, IO_operation, command_line, userinterface**.

I define the `.h` files, to declare all functions we need in the `include/` directory, they are:

![headers](README3/1.png "headers")

I define the modulization files in the `src/` directory, there are:

![modulizations](README3/2.png "modulizations")

Then I define a `CMakeLists.txt`, to manage all these files.

![cmakelist](README3/3.png "CMakeLists.txt")


----------------------------------------------

finally, I build all this project in the `build/`

![ninja](README3/4.png "ninja")

---------------------------------------------------------

allow files' architecture is:

![tree1](report/3/tree1.png "tree1")

![tree2](report/3/tree2.png "tree2")



## result show

### login

![login](report/3/1.png)

### quit-log(relogin)

![relogin](report/3/2.png "relogin")


### ls 

![ls](README3/6.png)

### mkdir 

![mkdir](report/3/4.png "mkdir")

### create 

![create](report/3/5.png "create")

### rmdir(delete folder/directory)

![rmdir](report/3/6.png "rmdir")

### delete

![delete](README3/8.png)

### cd

![cd](README3/9.png)

### open close write read

![read and write](report/3/7.png "read and write")

### overwrite

![overwrite](report/3/8.png "overwrite")

### last access time and modify time 

![a and m](report/3/9.png)

![a and m 2](report/3/10.png)

last access time and modify time:

![time](report/3/13.png)

### `chmod` and protection 

![protection](report/3/10.png "protection")

![chmod](report/3/11.png "chmod")



### check and format 

![check and format](report/3/12.png "format")

![check](report/3/14.png "check")



### password

![password](report/3/2.png "change password")

relogin:

![password2](report/3/3.png "relogin")



### `quit` (quit this program) and `quit-log` (quit crruent user)

`quit`

![quit](report/3/16.png "quit")

`quit-log`

![quit-log](report/3/15.png "quit-log")







