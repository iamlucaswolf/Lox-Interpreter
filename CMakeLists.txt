cmake_minimum_required(VERSION 3.13)
project(Lox_Interpreter)

set(CMAKE_CXX_STANDARD 17)

include_directories( ./src)

add_executable(lox src/main.cpp src/scanner/scanner.cpp src/data/token.cpp src/utility/ast-tools.cpp src/parser/parser.cpp src/parser/ParserError.cpp src/interpreter/Interpreter.cpp src/interpreter/Interpreter.h src/interpreter/LoxObject.h src/interpreter/LoxObject.cpp src/data/statement.h src/data/statement.cpp src/data/expression.cpp src/interpreter/Environment.cpp src/interpreter/Environment.h src/interpreter/RuntimeError.cpp src/interpreter/RuntimeError.h src/interpreter/Callable.cpp src/interpreter/Callable.h src/resolver/Resolver.cpp src/resolver/Resolver.h)