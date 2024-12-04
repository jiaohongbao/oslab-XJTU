cmake_minimum_required(VERSION 3.11)

project(EXT2_SYSTEM VERSION 1.0)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SRC_FILES
    src/command_line.c
    src/initialize.c 
    src/fundamental.c 
    src/IO_operation.c 
    src/user_interface.c 
    src/basic.c
    src/main_function.c
    main.c 
  )
add_executable(EXT2_SYSTEM ${SRC_FILES})


  
