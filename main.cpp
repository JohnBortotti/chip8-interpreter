#include "./chip8.h"
#include <SDL2/SDL.h>

int main() {
  chip8 chip8;

  chip8.initialize();
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

  return 0;
}
