#include <iostream>
#include <SDL2/SDL.h>

class chip8
{
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

    // delay timer
    unsigned short delay_timer;

    // sound timer 
    unsigned short sound_timer;

    // stack and stack_pointer
    unsigned short stck[16];
    unsigned short stack_pointer;

    unsigned char keypad[16];

    void initialize()
    {
        PC = 0x200;
        opcode = 0;
        I = 0;

        clear_gfx();
        clear_memory();
        clear_registers();
        clear_stack();
        // [ ] validate clear functions
        // [ ] load fonts

        // [ ] load game
    }

    void emulate_cycle()
    {
        // [ ] fetch
        // [ ] decode
        // [ ] execute

        // [ ] update (set timers, pc, draw_flag, etc)
    }

    void render_SDL_from_gfx(SDL_Renderer **renderer, SDL_Rect rect)
    {
        int i, j;

        for (i = 0; i <= 32; i++)
        {
            for (j = 0; j <= 64; j++)
            {
                if (this->gfx[j][i] != 0)
                {
                    SDL_RenderFillRect(*renderer, &rect);
                }

                rect.x = rect.x + 12;
            }

            rect.x = 0;
            rect.y = rect.y + 12;
        }

        SDL_RenderPresent(*renderer);
    }

private:
    void clear_gfx()
    {
        int i, j;

        for (i = 0; i <= 32; i++)
        {
            for (j = 0; j <= 64; j++)
            {
                this->gfx[j][i] = 0;
            }
        }
    }

    void clear_memory()
    {
        for (unsigned char &x : memory)
        {
            x = 0;
        }
    }

    void clear_registers()
    {
        for (unsigned char &x : V)
        {
            x = 0;
        }
    };

    void clear_stack()
    {
        for (unsigned short &x : stck)
        {
            x = 0;
        }

        stack_pointer = 0;
    }
};

int main()
{
    chip8 chip8;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *screenSurface;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);
    SDL_CreateWindowAndRenderer(900, 600, 0, &window, &renderer);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = 12;
    rect.h = 12;

    chip8.initialize();
    chip8.gfx[10][20] = 1;
    chip8.render_SDL_from_gfx(&renderer, rect);

    SDL_Delay(3000);

    // SDL_DestroyRenderer(renderer);
    // SDL_DestroyWindow(window);
    // SDL_Quit();

    // chip8.initialize();

    // for (;;)
    // {

    //     // [ ] emulate cycle
    //     // [ ] update graphics
    //     // [ ] get keypress
    // }

    return 0;
}