#include <iostream>
#include <fstream>
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

    bool draw_flag;

    // preloaded fontset
    unsigned char fontset[80] =
        {
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
    void initialize()
    {
        PC = 0x200;
        opcode = 0;
        I = 0x00;
        draw_flag = 0;

        clear_gfx();
        clear_memory();
        clear_registers();
        clear_stack();
        reset_timers();
        load_fontset();

        load_game("IBM_Logo.ch8");
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

    // load fontset values to memory
    void load_fontset()
    {
        for (int x = 0; x <= 80; x++)
        {
            memory[x] = fontset[x];
        }
    }

    void clear_memory()
    {
        for (unsigned char &x : memory)
        {
            x = 0;
        }
    }

    void reset_timers()
    {
        delay_timer, sound_timer = 0;
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

    // open file, extract its bits and load into memory
    void load_game(std::string path)
    {
        std::ifstream f(path, std::ios::binary | std::ios::in);

        if (!f.is_open())
        {
            throw std::invalid_argument("invalid ROM path");
        }

        // holder char
        char c;
        // start on memory address
        int adrs = 0x200;

        for (int i = 0x200; f.get(c); i++)
        {
            if (adrs >= 4096)
            {
                throw std::invalid_argument("file size too big");
            }
            memory[i] = (uint8_t)c;
            adrs++;
        }
    }
};

int main()
{
    chip8 chip8;
    // SDL_Window *window;
    // SDL_Renderer *renderer;
    // SDL_Surface *screenSurface;

    // SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);
    // SDL_CreateWindowAndRenderer(900, 600, 0, &window, &renderer);
    // SDL_RenderClear(renderer);

    // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // SDL_Rect rect;
    // rect.x = 0;
    // rect.y = 0;
    // rect.w = 12;
    // rect.h = 12;

    chip8.initialize();
    // chip8.gfx[10][20] = 1;
    // chip8.render_SDL_from_gfx(&renderer, rect);

    // SDL_Delay(3000);

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