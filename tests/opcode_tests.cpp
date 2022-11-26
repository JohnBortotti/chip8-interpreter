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

TEST_CASE("opcode 0x4XNN should skip next if V[X] != nn") {
  chip8.PC = 0;
  chip8.V[1] = 4;

  chip8.memory[0] = 0x41;
  chip8.memory[1] = 0x04;

  chip8.emulate_cycle();

  REQUIRE(chip8.PC == 2);

  chip8.memory[2] = 0x41;
  chip8.memory[3] = 0x17;

  chip8.emulate_cycle();

  REQUIRE(chip8.PC == 6);
}

TEST_CASE("opcode 0x5XY0 should skip next if V[x] == V[y]") {
  chip8.PC = 0;
  chip8.V[1] = 4;
  chip8.V[2] = 1;
  chip8.V[5] = 4;

  chip8.memory[0] = 0x51;
  chip8.memory[1] = 0x20;

  chip8.emulate_cycle();

  REQUIRE(chip8.PC == 2);

  chip8.memory[2] = 0x51;
  chip8.memory[3] = 0x50;

  chip8.emulate_cycle();

  REQUIRE(chip8.PC == 6);
}

TEST_CASE("opcode 0x6XNN should set V[x] = NN") {
  chip8.PC = 0;
  chip8.V[1] = 0;

  chip8.memory[0] = 0x61;
  chip8.memory[1] = 0x42;

  chip8.emulate_cycle();

  REQUIRE(chip8.V[1] == 0x42);
  REQUIRE(chip8.PC == 2);
}

TEST_CASE("opcode 0x7XNN should increment V[x] += nn") {
  chip8.PC = 0;
  chip8.V[3] = 2;

  chip8.memory[0] = 0x73;
  chip8.memory[1] = 0x07;

  chip8.emulate_cycle();

  REQUIRE(chip8.V[3] == 9);
  REQUIRE(chip8.PC == 2);
}

TEST_CASE("opcode 0x9XY0 should skip if V[x] != V[y]") {
  chip8.PC = 0;
  chip8.V[1] = 4;
  chip8.V[2] = 1;
  chip8.V[5] = 4;

  chip8.memory[0] = 0x91;
  chip8.memory[1] = 0x20;

  chip8.emulate_cycle();

  REQUIRE(chip8.PC == 4);

  chip8.memory[2] = 0x91;
  chip8.memory[3] = 0x50;

  chip8.emulate_cycle();

  REQUIRE(chip8.PC == 6);
}

TEST_CASE("opcode 0xANNN should set I = NNN") {
  chip8.PC = 0;
  chip8.I = 0;

  chip8.memory[0] = 0xA1;
  chip8.memory[1] = 0x66;

  chip8.emulate_cycle();

  REQUIRE(chip8.I == 0x166);
  REQUIRE(chip8.PC == 2);
}

TEST_CASE("opcode 0xDXYN should set correct gfx bitmap and render pixels") {
  chip8.PC = 0;
  chip8.I = 11;

  chip8.V[0xF] = 0;
  chip8.V[3] = 2;
  chip8.V[7] = 2;

  chip8.memory[0] = 0xD3;
  chip8.memory[1] = 0x72;

  chip8.memory[11] = 0b11000000;
  chip8.memory[12] = 0b00000011;

  chip8.gfx[2][2] = 1;

  chip8.emulate_cycle();

  REQUIRE(chip8.V[0xF] == 1);
  REQUIRE(chip8.PC == 2);
  REQUIRE(chip8.I == 11);

  REQUIRE(chip8.gfx[2][2] == 0);
  REQUIRE(chip8.gfx[3][2] == 1);
  REQUIRE(chip8.gfx[5][2] == 0);

  REQUIRE(chip8.gfx[2][3] == 0);
  REQUIRE(chip8.gfx[8][3] == 1);
}

TEST_CASE("opcode 0xFX15 should set delay_timer = x") {
  chip8.PC = 0;
  chip8.delay_timer = 0;

  chip8.memory[0] = 0xF3;
  chip8.memory[1] = 0x15;

  chip8.emulate_cycle();

  REQUIRE(chip8.delay_timer == 3);
  REQUIRE(chip8.PC == 2);
}

TEST_CASE("opcode 0xFX29 should set I to sprite corresponding of V[x]") {
  chip8.PC = 0;
  chip8.V[6] = 4;

  chip8.memory[0] = 0xF6;
  chip8.memory[1] = 0x29;

  chip8.emulate_cycle();

  REQUIRE(chip8.PC == 2);
  REQUIRE(chip8.I == 20);
}

TEST_CASE("opcode 0xFX33 should set binary-ecimal of V[x] at I I+1 and I+2") {
  chip8.PC = 0;
  chip8.V[9] = 237;
  chip8.I = 10;

  chip8.memory[0] = 0xF9;
  chip8.memory[1] = 0x33;

  chip8.emulate_cycle();

  REQUIRE(chip8.PC == 2);
  REQUIRE(chip8.memory[chip8.I] == 23);
  REQUIRE(chip8.memory[chip8.I + 1] == 3);
  REQUIRE(chip8.memory[chip8.I + 2] == 7);
}

TEST_CASE("opcode 0xFX65 should fill V[0] to V[x] with values starting at I") {
  chip8.PC = 0;
  chip8.V[0] = 0;
  chip8.V[1] = 0;
  chip8.V[2] = 0;

  chip8.I = 6;

  chip8.memory[6] = 42;
  chip8.memory[7] = 47;
  chip8.memory[8] = 51;

  chip8.memory[0] = 0xF2;
  chip8.memory[1] = 0x65;

  chip8.emulate_cycle();

  REQUIRE(chip8.V[0] == 42);
  REQUIRE(chip8.V[1] == 47);
  REQUIRE(chip8.V[2] == 51);

  REQUIRE(chip8.I == 9);

  REQUIRE(chip8.PC == 2);
}
