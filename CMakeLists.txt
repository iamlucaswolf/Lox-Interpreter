cmake_minimum_required(VERSION 3.13)
project(Lox_Interpreter)

set(CMAKE_CXX_STANDARD 17)

include_directories( ./include ./src)

add_executable(lox src/main.cpp src/scanner.cpp src/token.cpp src/ast-tools.cpp)