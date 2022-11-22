#define CATCH_CONFIG_MAIN
#include "../chip8.h"
#include <catch2/catch_test_macros.hpp>

chip8 chip8;

TEST_CASE("opcode 0x00E0 should clear gfx bitmap") {
  chip8.PC = 0;
  chip8.gfx[1][2] = 1;
  chip8.gfx[14][3] = 1;

  chip8.memory[0] = 0x00;
  chip8.memory[1] = 0xE0;

  chip8.emulate_cycle();

  REQUIRE(chip8.gfx[1][2] == 0);
  REQUIRE(chip8.gfx[14][3] == 0);
  REQUIRE(chip8.PC == 2);
}

TEST_CASE("opcode 0x00EE should return from procedure and pop stack") {
  chip8.PC = 0;

  chip8.stack_pointer = 4;
  chip8.stack[4] = 42;

  chip8.memory[0] = 0x00;
  chip8.memory[1] = 0xEE;

  chip8.emulate_cycle();

  REQUIRE(chip8.stack_pointer == 3);
  REQUIRE(chip8.PC == 44);
}

TEST_CASE("opcode 0x1NNN should set PC to NNN") {
  chip8.PC = 0;

  chip8.memory[0] = 0x16;
  chip8.memory[1] = 0x66;

  chip8.emulate_cycle();

  REQUIRE(chip8.PC == 0x666);
}

TEST_CASE("opcode 0x2NNN should execute procedure at NNN and push to stack") {
  chip8.PC = 3;
  chip8.stack_pointer = 0;

  chip8.memory[3] = 0x26;
  chip8.memory[4] = 0x66;

  chip8.emulate_cycle();

  REQUIRE(chip8.stack_pointer == 1);
  REQUIRE(chip8.stack[0] == 3);
  REQUIRE(chip8.PC == 0x666);
}

TEST_CASE("opcode 0x3XNN should skip next if V[x] == nn") {
  chip8.PC = 0;
  chip8.V[1] = 3;

  chip8.memory[0] = 0x31;
  chip8.memory[1] = 0x03;

  chip8.emulate_cycle();

  REQUIRE(chip8.PC == 4);

  chip8.memory[4] = 0x31;
  chip8.memory[5] = 0x02;

  chip8.emulate_cycle();

  REQUIRE(chip8.PC == 6);
}
