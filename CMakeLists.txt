cmake_minimum_required(VERSION 3.13)
project(Lox_Interpreter)

set(CMAKE_CXX_STANDARD 17)

add_executable(lox main.cpp scanner.cpp scanner.h token.cpp token.h)
