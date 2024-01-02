// Includes here
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>

#include "include/display.h"
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
	vector<vector<bool>> placeholder;

	Display d{15};

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

		d.updateDisplay(placeholder);
	}

	return 0;
}