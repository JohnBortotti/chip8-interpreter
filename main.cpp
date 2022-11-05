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

  // stack and stack_pointer
  unsigned short stck[16];
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

    load_game("roms/PONG");
  }

  void emulate_cycle() {
    opcode = memory[PC] << 8 | memory[PC + 1];

    printf("executing opcode: %x \n", opcode);

    switch (opcode & 0xF000) {
    case 0x6000:
      V[opcode & 0x0F00] = opcode & 0x00FF;
      PC += 2;
      break;
    case 0xA000:
      I = opcode & 0x0FFF;
      PC += 2;
      break;
    case 0xD000: {
      unsigned short x = V[(opcode & 0x0F00) >> 8];
      unsigned short y = V[(opcode & 0x00F0) >> 4];
      unsigned short height = opcode & 0x000F;
      unsigned short row_pixels;

      // flag register (on this scope, it will be used as the "collision flag")
      V[0xF] = 0;

      // loop row (for height)
      for (int y_axis = 0; y_axis < height; y_axis++) {
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

      draw_flag = true;
      PC += 2;
      break;
    }
    default:
      printf("opcode desconhecido: %X\n", opcode);
      exit(0);
      break;
    }

    if (draw_flag == true) {
      render_SDL_from_gfx();
      draw_flag = false;
    }

    // [ ] update (set timers draw_flag,)
  }

private:
  void initialize_SDL() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);
    SDL_CreateWindowAndRenderer(900, 600, 0, &window, &renderer);
    SDL_RenderClear(renderer);
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
    for (unsigned short &x : stck) {
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

  for (;;) {
    chip8.emulate_cycle();
  }

  SDL_Quit();

  return 0;
}
