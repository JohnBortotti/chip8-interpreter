#include <SDL2/SDL.h>
#include <string>

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

  bool keypad[16];

  // SDL
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Surface *screenSurface;
  SDL_Rect sdl_rect;
  SDL_Event sdl_event;

  void initialize(std::string rom_path);
  void debug_print();
  void emulate_cycle();

private:
  void initialize_SDL();
  void render_SDL_from_gfx();
  void clear_gfx();
  void clear_keypad();
  void load_fontset();
  void clear_memory();
  void reset_timers();
  void clear_registers();
  void clear_stack();
  void load_game(std::string);
};
