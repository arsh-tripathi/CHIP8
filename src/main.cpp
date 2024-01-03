// Includes here
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>

#include "include/chip8.h"
#include "include/SDL2/SDL.h"
#include "include/SDL2/SDL_render.h"
#include "include/SDL2/SDL_stdinc.h"
#include "include/SDL2/SDL_video.h"

using namespace std;

int main(int argc, char *argv[]) {
	if (argc < 2) {
		cerr <<  "Usage: ./chip8 rom" << endl;
		exit(1);
	}
	char *rom = argv[1];

	// Initializing SDL
	bool quit = false;

	Chip8 c{15};
	// c.executeInstruction(0x00E0); // clear display
	// c.executeInstruction(0x6207); // set the register 2 to the number 07
	// c.executeInstruction(0xF229); // set I to sprite for the number at the register at second place
	// c.executeInstruction(0x6000); // set register 0 to 0x10
	// c.executeInstruction(0x6100); // set register 1 to 0x10
	// c.executeInstruction(0xD015); // draw 

	SDL_Event e;
	while (!quit) {
		SDL_PollEvent(&e);
		switch (e.type) {
			case SDL_QUIT:
				quit = true;
				break;
			default:
				break;
		}

	}

	return 0;
}