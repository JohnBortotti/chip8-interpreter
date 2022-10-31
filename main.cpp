#include <iostream>
#include <SDL2/SDL.h>

class chip8
{
public:
    unsigned short opcode;

    // 4KB RAM memory
    unsigned char memory[4096];

    // 16 8-bit (1-byte) registers
    // V0 to VF
    unsigned char V[16];

    // index register "I"
    unsigned short I;

    // program counter
    unsigned short PC;

    // graphics matrix (64x32 pixels / bits)
    unsigned char gfx[64 * 32];

    // delay and sound timers (1-byte)
    unsigned short delay_timer;
    unsigned short sound_timer;

    // stack and stack_pointer
    unsigned short stck[16];
    unsigned short stack_pointer;

    unsigned char keypad[16];

    void initialize()
    {
        // [ ] reset registers
        // [ ] load game
    }

    void emulate_cycle()
    {
        // [ ] fetch
        // [ ] decode
        // [ ] execute

        // [ ] update (set timers, pc, draw_flag, etc)
    }
};

int main()
{
    chip8 chip8;
    SDL_Window *window;
    SDL_Surface *screenSurface;

    // setup SDL engine
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0)
    {
        std::cout << "Erro: " << SDL_GetError() << std::endl;
        SDL_Quit();
    }

    window = SDL_CreateWindow("CHIP-8", 100, 50, 320, 480, SDL_WINDOW_SHOWN);

    if (!window)
    {
        std::cout << "Erro: " << SDL_GetError() << std::endl;
        SDL_Quit();
    }

    SDL_Delay(5000);
    SDL_Quit();


    // chip8.initialize();

    for (;;) {

    //     // [ ] emulate cycle
    //     // [ ] update graphics
    //     // [ ] get keypress
    }

    return 0;
}