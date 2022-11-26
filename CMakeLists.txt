cmake_minimum_required(VERSION 3.16)
project(chip8-emulator)

# main
add_executable(main main.cpp)
target_link_libraries(main SDL2)

# chip8 lib
add_library(chip8 STATIC chip8.cpp)
target_link_libraries(main chip8)

target_link_libraries(chip8 SDL2)

# tests
find_package(Catch2 3 REQUIRED)
add_executable(opcode_tests tests/opcode_tests.cpp)
add_executable(chip8_tests tests/chip8_tests.cpp)
target_link_libraries(opcode_tests PRIVATE Catch2::Catch2WithMain chip8)
target_link_libraries(chip8_tests PRIVATE Catch2::Catch2WithMain chip8)
