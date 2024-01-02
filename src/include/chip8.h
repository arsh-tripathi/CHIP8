#ifndef __CHIP8_H__
#define __CHIP8_H__
#include "display.h"
#include <vector>
#include <iostream>
#include <random>

class Chip8 { 
    // Program Counter - Stores the current address
    Uint16 PC;

    // Stack
    Uint16 STACK[16];

    // Stack Pointer
    Uint16 *SP;

    // General Purpose 8bit registers
    Uint8 V[16];

    // 16 bit register
    Uint16 I;

    // Special Purpose 8bit registers
    Uint8 DT; // Delay Timer
    Uint8 ST; // Sound Timer

    static std::vector<std::vector<Uint8>> SPRITES;

    Uint8 memory[4096];

    Display d;     

    Uint8 scale;

    public:
        Chip8(Uint8 scale);
        void tick();
    private:
        void convertCommad(Uint16 cmd);

};

#endif