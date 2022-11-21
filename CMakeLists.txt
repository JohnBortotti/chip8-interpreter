cmake_minimum_required(VERSION 3.16)
project(chip8-emulator)

add_executable(main main.cpp)
target_link_libraries(main SDL2)

add_library(chip8 STATIC chip8.cpp)
target_link_libraries(main chip8)

target_link_libraries(chip8 SDL2)

find_package(Catch2 3 REQUIRED)
add_executable(test tests/opcode_tests.cpp)
target_link_libraries(test PRIVATE Catch2::Catch2WithMain chip8)
