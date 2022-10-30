#include <iostream>

/*
    CHIP-8 Interpreter

    16-bit addresses stack
*/

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

    // [ ] setup graphics engine

    chip8.initialize();

    for (;;) {

        // [ ] emulate cycle
        // [ ] update graphics
        // [ ] get keypress
    }

    return 0;
}