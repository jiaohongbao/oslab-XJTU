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



## result show

### login

![login](README3/5.png)

### ls 

![ls](README3/6.png)

### create 

![create](README3/7.png)

### delete

![delete](README3/8.png)

### cd

![cd](README3/9.png)

### open close write read

![close and open](README3/10.png)

### check and format 

![check and format](README3/11.png)

### password

![password](README3/12.png)

### time

![time](README3/13.png)

### protection

![protection](README3/14.png)







