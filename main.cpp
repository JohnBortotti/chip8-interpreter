#include <SDL2/SDL.h>
#include <fstream>
#include <iostream>

class chip8 {
public:
  unsigned short opcode;

  // RAM memory
  unsigned char memory[4096];

  // 16 registers (V0 to VF)
  unsigned char V[16];

  // index register "I"
  unsigned short I;

  // program counter
  unsigned short PC;

  // graphics matrix (64x32 pixels / bits)
  bool gfx[64][32];

  bool draw_flag;

  // preloaded fontset
  unsigned char fontset[80] = {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  // delay timer
  unsigned short delay_timer;

  // sound timer
  unsigned short sound_timer;

  // stack and stack_pointer (used to return from subroutines)
  unsigned short stack[16];
  unsigned short stack_pointer;

  unsigned char keypad[16];

  // SDL
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Surface *screenSurface;
  SDL_Rect sdl_rect;

  void initialize() {
    PC = 0x200;
    opcode = 0;
    I = 0x00;
    draw_flag = 0;

    initialize_SDL();

    clear_gfx();
    clear_memory();
    clear_registers();
    clear_stack();
    reset_timers();
    load_fontset();

    load_game("roms/TEST_OPCODE");
  }

  void debug_print() {
    system("clear");
    printf("Loaded ROM: %s \n \n", "TEST_OPCODE");

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
    for (int i = PC - 10; i < PC + 10; i++) {
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
  }

  void emulate_cycle() {
    opcode = memory[PC] << 8 | memory[PC + 1];

    unsigned short x = (opcode & 0x0F00) >> 8;
    unsigned short y = (opcode & 0x00F0) >> 4;
    unsigned short nnn = (opcode & 0x0FFF);
    unsigned short nn = (opcode & 0x00FF);
    unsigned short n = (opcode & 0x000F);

    switch (opcode & 0xF000) {
    case 0x0000: {
      switch (nn) {
      case 0xe0: {
        clear_gfx();
        draw_flag = 1;
        PC += 2;
        break;
      }
      case 0xEE: {
        PC = stack[stack_pointer];
        stack_pointer--;
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

        // loop bits of row (8 bits per sprite)
        for (int x_axis = 0; x_axis < 8; x_axis++) {

          // if current pixel is set to 1 (non zero)
          if ((row_pixels & (0x80 >> x_axis)) != 0) {

            // detect collision
            if (gfx[x + x_axis][y + y_axis] == 1) {
              // set collision flag
              V[0xF] = 1;
            }

            // XOR bit to draw the result
            gfx[x + x_axis][y + y_axis] ^= 1;
          }
        }
      }

      draw_flag = 1;
      PC += 2;
      break;
    }
    case 0xF000: {
      switch (nn) {
      case 0x15: {
        delay_timer = x;
        PC += 2;
        break;
      }
      case 0x29: {
        I = (V[x]) * 0x5;
        PC += 2;
        break;
      }
      case 0x33: {
        unsigned short x = V[x];

        // hundreds digits
        memory[I] = x / 10;
        // tens digits
        memory[I + 1] = (x / 10) % 10;
        // ones digit
        memory[I + 2] = (x % 100) % 10;
        PC += 2;
        break;
      }
      case 0x65: {
        for (int i = 0; i <= (x); i++) {
          V[i] = memory[I + i];
        }

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

    // after opcode execution
    // if draw_flag is set, then update display
    if (draw_flag == 1) {
      render_SDL_from_gfx();
      draw_flag = 0;
    }

    // [ ] update (set timers draw_flag,)
  }

private:
  void initialize_SDL() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);
    SDL_CreateWindowAndRenderer(900, 600, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    sdl_rect.x = 0;
    sdl_rect.y = 0;
    sdl_rect.w = 12;
    sdl_rect.h = 12;
  }

  void render_SDL_from_gfx() {
    int i, j;

    for (i = 0; i <= 32; i++) {
      for (j = 0; j <= 64; j++) {
        if (this->gfx[j][i] != 0) {
          SDL_RenderFillRect(renderer, &sdl_rect);
        }

        sdl_rect.x = sdl_rect.x + 12;
      }

      sdl_rect.x = 0;
      sdl_rect.y = sdl_rect.y + 12;
    }

    SDL_RenderPresent(renderer);
    // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    // SDL_RenderClear(renderer);
    // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  }

  void clear_gfx() {
    int i, j;

    for (i = 0; i <= 32; i++) {
      for (j = 0; j <= 64; j++) {
        this->gfx[j][i] = 0;
      }
    }
  }

  // load fontset values to memory
  void load_fontset() {
    for (int x = 0; x <= 80; x++) {
      memory[x] = fontset[x];
    }
  }

  void clear_memory() {
    for (unsigned char &x : memory) {
      x = 0;
    }
  }

  void reset_timers() {
    delay_timer = 0;
    sound_timer = 0;
  }

  void clear_registers() {
    for (unsigned char &x : V) {
      x = 0;
    }
  };

  void clear_stack() {
    for (unsigned short &x : stack) {
      x = 0;
    }

    stack_pointer = 0;
  }

  // open file, extract its bits and load into memory
  void load_game(std::string path) {
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
};

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

    // update game state, draw the current frame
  }

  return 0;
}
