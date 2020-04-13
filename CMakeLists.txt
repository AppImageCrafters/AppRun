project(AppImageExecWrapper)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS "-fprofile-arcs -ftest-coverage")

add_subdirectory(src)

enable_testing()
add_subdirectory(test)