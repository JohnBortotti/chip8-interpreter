#include "./chip8.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

void handle_inputs(SDL_Event event, chip8 &chip8) {
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT: {
      exit(0);
      break;
    }
    case SDL_KEYDOWN: {
      switch (event.key.keysym.sym) {
      case SDLK_1: {
        chip8.keypad[0x1] = 1;
        break;
      }
      case SDLK_2: {
        chip8.keypad[0x2] = 1;
        break;
      }
      case SDLK_3: {
        chip8.keypad[0x3] = 1;
        break;
      }
      case SDLK_4: {
        chip8.keypad[0xc] = 1;
        break;
      }
      case SDLK_q: {
        chip8.keypad[0x4] = 1;
        break;
      }
      case SDLK_w: {
        chip8.keypad[0x5] = 1;
        break;
      }
      case SDLK_e: {
        chip8.keypad[0x6] = 1;
        break;
      }
      case SDLK_r: {
        chip8.keypad[0xd] = 1;
        break;
      }
      case SDLK_a: {
        chip8.keypad[0x7] = 1;
        break;
      }
      case SDLK_s: {
        chip8.keypad[0x8] = 1;
        break;
      }
      case SDLK_d: {
        chip8.keypad[0x9] = 1;
        break;
      }
      case SDLK_f: {
        chip8.keypad[0xe] = 1;
        break;
      }
      case SDLK_z: {
        chip8.keypad[0xa] = 1;
        break;
      }
      case SDLK_x: {
        chip8.keypad[0x0] = 1;
        break;
      }
      case SDLK_c: {
        chip8.keypad[0xb] = 1;
        break;
      }
      case SDLK_v: {
        chip8.keypad[0xf] = 1;
        break;
      }
      default: {
        break;
      }
      }
      break;
    }
    default: {
      break;
    }
    }
  }
}

void play_mode(std::string rom_path) {
  chip8 chip8;
  chip8.initialize(rom_path);

  constexpr float cpu_rate = 1000; 
  constexpr float ticks_todo = cpu_rate / 60;
  constexpr float tick_ms = (1.0 / cpu_rate) * 1000.0;

  unsigned int acc = SDL_GetTicks();
  unsigned int last = SDL_GetTicks();

  float current_ticks = 0;

  while (true) {
    acc += SDL_GetTicks() - last;

    if (acc >= tick_ms) {
	current_ticks++;
        acc = 0;
    } else {
	last = SDL_GetTicks();
	continue;
    }

    SDL_Event event;
    handle_inputs(event, chip8);

    if (current_ticks < ticks_todo) {
        chip8.emulate_cycle();
    } else {
	current_ticks = 0;
    }

    last = SDL_GetTicks();

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
