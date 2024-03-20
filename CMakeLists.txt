cmake_minimum_required(VERSION 3.28)

project(
  Chip8Interpreter
  VERSION 1.0
  LANGUAGES CXX)

add_library(Chip8 include/chip8.hpp src/chip8.cpp)

add_executable(Main src/main.cpp)

target_link_libraries(Main PRIVATE Chip8)