cmake_minimum_required(VERSION 3.28)

project(
  Chip8Interpreter
  VERSION 1.0
  LANGUAGES CXX)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_library(Chip8 include/chip8.hpp include/interpreter.hpp src/chip8.cpp src/interpreter.cpp)
target_include_directories(Chip8 PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(Main src/main.cpp)

target_link_libraries(Main PRIVATE ${SDL2_LIBRARIES} Chip8)