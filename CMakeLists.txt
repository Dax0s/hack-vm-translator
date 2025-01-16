cmake_minimum_required(VERSION 3.30)
project(VMTranslator C)

set(CMAKE_C_STANDARD 11)

add_executable(VMTranslator src/main.c
        src/Parser.c
        src/Parser.h)
