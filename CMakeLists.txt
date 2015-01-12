cmake_minimum_required(VERSION 2.6)
project(TP3)

SET(CMAKE_C_COMPILER "gcc")
SET(CMAKE_C_FLAGS "-Wall -Wextra -std=c99 -D_GNU_SOURCE")

include_directories(src)

add_executable(TP-DIST.out
    src/TP-DIST.c
    src/host_tools.c
    src/host_tools.h
    src/socket_tools.c
    src/socket_tools.h
    src/message_tools.c
    src/message_tools.h
    src/message_linked_list.c
    src/message_linked_list.h
)

add_executable(TP-DIST-CL.out
    src/TP-DIST-CL.c
)
