cmake_minimum_required(VERSION 3.25.1)

project(INTERPRETER)

set(CMAKE_BUILD_TYPE Debug)

include_directories(src/include)

add_executable(prog src/c/main.c src/c/vm.c)
