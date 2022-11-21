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
}
