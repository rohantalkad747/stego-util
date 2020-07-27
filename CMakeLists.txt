cmake_minimum_required(VERSION 3.16)
project(files C)

set(CMAKE_C_STANDARD 99)

add_executable(files cat.c stego.c)