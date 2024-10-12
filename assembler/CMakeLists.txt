cmake_minimum_required(VERSION 3.25.1)

project(ASSEMBLER)

set(CMAKE_BUILD_TYPE Debug)

include_directories(src/include)

add_executable(assembler src/c/main.c src/c/assembler.c src/c/eval.c src/c/scanner.c)