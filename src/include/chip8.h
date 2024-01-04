#ifndef __CHIP8_H__
#define __CHIP8_H__
#include "display.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <random>

class Chip8 { 
    // Program Counter - Stores the current address
    Uint16 PC;

    // Stack
    Uint16 STACK[16];

    // Stack Pointer
    int SP;

    // General Purpose 8bit registers
    Uint8 V[16];

    // 16 bit register
    Uint16 I;

    // Special Purpose 8bit registers
    Uint8 DT; // Delay Timer
    Uint8 ST; // Sound Timer

    static std::vector<std::vector<Uint8>> SPRITES;

    Uint8 memory[4096];

    std::vector<std::vector<bool>> reald;
    public:
    Display d;
    SDL_TimerID timer;

    Uint8 scale;

    public:
        Chip8(Uint8 scale, char *rom);
        ~Chip8();
        void tick();
        void executeInstruction(Uint16 cmd);
        void run();
    private:
        void printStaus();

};

#endif