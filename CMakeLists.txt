cmake_minimum_required(VERSION 2.6)
project(TP3)

SET(CMAKE_C_COMPILER "gcc")
SET(CMAKE_C_FLAGS "-Wall -Wextra -std=c99 -D_GNU_SOURCE")

include_directories(src)

add_executable(TP-DIST.out
    src/TP-DIST.c
)

add_executable(TP-DIST-CL.out
    src/TP-DIST-CL.c
)
