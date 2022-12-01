#include "./chip8.h"
#include <SDL2/SDL.h>

void play_mode(std::string rom_path) {
  chip8 chip8;
  chip8.initialize(rom_path);

  while (true) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        exit(0);
      }

      chip8.emulate_cycle();
    }
  }
}

void debug_mode(std::string rom_path) {
  chip8 chip8;
  chip8.initialize(rom_path);
  chip8.debug_print();

  while (true) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        exit(0);
      }

      if (event.type == SDL_KEYDOWN) {
        chip8.emulate_cycle();
        chip8.debug_print();
      }
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Invalid args, use: main {rom_path} {debug_bit} \n");
    exit(0);
  }

  if (std::string(argv[2]) == "0") {
    play_mode(argv[1]);
  } else {
    debug_mode(argv[1]);
  }
  return 0;
}
