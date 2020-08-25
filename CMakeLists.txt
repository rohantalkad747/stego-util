cmake_minimum_required(VERSION 3.16)
project(stego C)

set(CMAKE_C_STANDARD 99)

add_executable(stego include/stego.h src/stego.c src/cli_driver.c)

add_executable(test include/minunit.h include/stego.h src/stego.c test/test.c)