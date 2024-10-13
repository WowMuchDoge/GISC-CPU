cmake_minimum_required(VERSION 3.25.1)

project(ASSEMBLER)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(src/include)

add_executable(assembler src/c/main.c src/c/assembler.c src/c/eval.c src/c/scanner.c src/c/error.c src/c/table.c)
