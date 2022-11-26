#define CATCH_CONFIG_MAIN
#include "../chip8.h"
#include <catch2/catch_test_macros.hpp>

chip8 chip8;

TEST_CASE("emulate_cycle should read correct opcode") {
  chip8.PC = 0;

  chip8.memory[0] = 0xD1;
  chip8.memory[1] = 0x25;

  chip8.emulate_cycle();

  REQUIRE(chip8.opcode == 0xD125);
}
