#include "./chip8.h"
#include <SDL2/SDL.h>
#include <fstream>
#include <iostream>

void chip8::initialize(std::string rom_path) {
  PC = 0x200;
  opcode = 0;
  I = 0x00;
  draw_flag = 0;
 
  initialize_SDL();
  clear_keypad();
  clear_gfx();
  clear_memory();
  clear_registers();
  clear_stack();
  reset_timers();
  load_fontset();
  load_game(rom_path);
}

void chip8::debug_print() {
  system("clear");
  printf("Loaded ROM: %s \n \n", "IBM");

  printf("OPCODE: %X \n", opcode);
  printf("PC: %X \n \n", PC);

  printf("I: %X \n \n", I);

  printf("Draw flag: %d \n \n", draw_flag);

  printf("delay_timer: %d   sound_timer: %d \n \n", delay_timer, sound_timer);

  printf("stack pointer: %x \n", stack_pointer);
  for (int i = 0; i < 16; i++) {
    printf("%x ", stack[i]);
  }
  printf("\n \n");

  for (int i = 0; i < 16; i++) {
    printf("V%x: %x  ", i, V[i]);
  }
  printf("\n \n");

  printf("MEMORY: \n");
  for (int i = PC - 10; i < PC + 10; i += 2) {
    if (i == PC - 2) {
      printf("\x1b[41m"
             "%X%X - \n"
             "\x1b[0m",
             memory[i], memory[i + 1]);
    } else if (i == PC) {
      printf("\x1b[42m"
             "%X%X - \n"
             "\x1b[0m",
             memory[i], memory[i + 1]);
    } else {
      printf("%X%X \n", memory[i], memory[i + 1]);
    }
  }

  printf("\n \n");
}

void chip8::emulate_cycle() {
  opcode = memory[PC] << 8 | memory[PC + 1];

  unsigned short x = (opcode & 0x0F00) >> 8;
  unsigned short y = (opcode & 0x00F0) >> 4;
  unsigned short nnn = (opcode & 0x0FFF);
  unsigned short nn = (opcode & 0x00FF);
  unsigned short n = (opcode & 0x000F);

  switch (opcode & 0xF000) {
  case 0x0000: {
    switch (nn) {
    case 0xE0: {
      clear_gfx();
      draw_flag = 1;
      PC += 2;
      break;
    }
    case 0xEE: {
      stack_pointer--;
      PC = stack[stack_pointer];
      PC += 2;
      break;
    }
    default:
      printf("opcode desconhecido: %X\n", opcode);
      exit(0);
      break;
    }
    break;
  }
  case 0x1000: {
    PC = nnn;
    break;
  }
  case 0x2000: {
    // store the current PC on stack, so we can return after the subroutine
    stack[stack_pointer] = PC;
    stack_pointer++;
    PC = nnn;
    break;
  }
  case 0x3000: {
    if (V[x] == nn) {
      PC += 4;
    } else {
      PC += 2;
    }

    break;
  }
  case 0x4000: {
    if (V[x] != nn) {
      PC += 4;
    } else {
      PC += 2;
    }
    break;
  }
  case 0x5000: {
    if (V[x] == V[y]) {
      PC += 4;
    } else {
      PC += 2;
    }
    break;
  }
  case 0x6000:
    V[x] = nn;
    PC += 2;
    break;
  case 0x7000:
    V[x] += (nn);
    PC += 2;
    break;
  case 0x8000: {
    switch (n) {
    case 0x0: {
      V[x] = V[y];
      PC += 2;
      break;
    }
    case 0x1: {
      V[x] = (V[x] || V[y]);
      PC += 2;
      break;
    }
    case 0x2: {
      V[x] = (V[x] && V[y]);
      PC += 2;
      break;
    }
    case 0x3: {
      V[x] = (!V[x] != !V[y]);
      PC += 2;
      break;
    }
    case 0x4: {
      V[0xF] = (V[y] > (255 - V[x])) ? 1 : 0;
      V[x] += V[y];
      PC += 2;
      break;
    }
    case 0x5: {
      V[0xF] = (V[y] > V[x]) ? 0 : 1;
      V[x] -= V[y];
      PC += 2;
      break;
    }
    case 0x6: {
      V[0xF] = V[y] & 0b00000001 ? 1 : 0;
      V[x] = V[y] >> 1;
      PC += 2;
      break;
    }
    case 0x7: {
      V[y] > V[x] ? V[0xF] = 0 : V[0xF] = 1;
      V[x] -= V[y]; 
      PC += 2;
      break;
    }
    case 0xE: {
      V[0xF] = V[y] & 0b10000000 ? 1 : 0;
      V[x] = V[y] << 1;
      PC += 2;
      break;
    }
    default:
      printf("opcode desconhecido: %X\n", opcode);
      exit(0);
      break;
    }
    break;
  }
  case 0x9000: {
    if (V[x] != V[y]) {
      PC += 4;
    } else {
      PC += 2;
    }
    break;
  }
  case 0xA000:
    I = nnn;
    PC += 2;
    break;
  case 0xD000: {
    unsigned short row_pixels;

    // flag register (on this scope, it will be used as the "collision flag")
    V[0xF] = 0;

    // loop row (for height)
    for (int y_axis = 0; y_axis < n; y_axis++) {
      row_pixels = memory[I + y_axis];

      // loop bits of row (8 bits per row)
      for (int x_axis = 0; x_axis < 8; x_axis++) {

        // if current pixel is set to 1 (non zero)
        if ((row_pixels & (0x80 >> x_axis)) != 0) {

          // detect collision
          if (gfx[V[x] + x_axis][V[y] + y_axis] == 1) {
            // set collision flag
            V[0xF] = 1;
          }

          // XOR bit to draw the result
          gfx[V[x] + x_axis][V[y] + y_axis] ^= 1;
        }
      }
    }

    draw_flag = 1;
    PC += 2;
    break;
  }
  case 0xF000: {
    switch (nn) {
    case 0x07: {
      V[x] = delay_timer;
      PC += 2;
      break;
    }
    case 0x15: {
      delay_timer = x;
      PC += 2;
      break;
    }
    case 0x1E: {
      I += V[x];
      PC += 2;
      break;
    }
    case 0x29: {
      I = (V[x]) * 0x5;
      PC += 2;
      break;
    }
    case 0x33: {
      unsigned short z = V[x];

      // hundreds digits
      memory[I] = z / 10;
      // // tens digits
      memory[I + 1] = (z / 10) % 10;
      // // ones digit
      memory[I + 2] = (z % 100) % 10;
      PC += 2;
      break;
    }
    case 0x55: {
      for (int i=0; i<= (x); i++) {
        memory[I + i] = V[i];	     
      }

      I = I + x + 1;

      PC += 2;
      break;
    }
    case 0x65: {
      for (int i = 0; i <= (x); i++) {
        V[i] = memory[I + i];
      }

      I = I + x + 1;

      PC += 2;
      break;
    }
    default:
      printf("opcode desconhecido: %X\n", opcode);
      exit(0);
      break;
    }
    break;
  }
  default:
    printf("opcode desconhecido: %X\n", opcode);
    exit(0);
    break;
  }

  if (draw_flag == 1) {
    render_SDL_from_gfx();
    draw_flag = 0;
  }
}

void chip8::initialize_SDL() {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);
  SDL_CreateWindowAndRenderer(768, 384, 0, &window, &renderer);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  sdl_rect.x = 0;
  sdl_rect.y = 0;
  sdl_rect.w = 12;
  sdl_rect.h = 12;
}

void chip8::render_SDL_from_gfx() {
  int i, j;

  sdl_rect.y = 0;
  sdl_rect.x = 0;

  for (i = 0; i < 32; i++) {
    for (j = 0; j < 64; j++) {
      if (this->gfx[j][i] != 0) {
        SDL_RenderFillRect(renderer, &sdl_rect);
      }
      sdl_rect.x = sdl_rect.x + 12;
    }

    sdl_rect.x = 0;
    sdl_rect.y = sdl_rect.y + 12;
  }

  SDL_RenderPresent(renderer);
}

void chip8::clear_gfx() {
  int i, j;

  for (i = 0; i <= 32; i++) {
    for (j = 0; j <= 64; j++) {
      this->gfx[j][i] = 0;
    }
  }
}

void chip8::clear_keypad() {
  for (int i = 0; i <= 16; i++) {
    this->keypad[i] = 0;
  }
}

// load fontset values to memory
void chip8::load_fontset() {
  for (int x = 0; x <= 80; x++) {
    memory[x] = fontset[x];
  }
}

void chip8::clear_memory() {
  for (unsigned char &x : memory) {
    x = 0;
  }
}

void chip8::reset_timers() {
  delay_timer = 0;
  sound_timer = 0;
}

void chip8::clear_registers() {
  for (unsigned char &x : V) {
    x = 0;
  }
};

void chip8::clear_stack() {
  for (unsigned short &x : stack) {
    x = 0;
  }

  stack_pointer = 0;
}

// open file, extract its bits and load into memory
void chip8::load_game(std::string path) {
  std::ifstream f(path, std::ios::binary | std::ios::in);

  if (!f.is_open()) {
    throw std::invalid_argument("invalid ROM path");
  }

  // holder char
  char c;
  // start on memory address
  int adrs = 0x200;

  for (int i = 0x200; f.get(c); i++) {
    if (adrs >= 4096) {
      throw std::invalid_argument("file size too big");
    }
    memory[i] = (uint8_t)c;
    adrs++;
  }
}
