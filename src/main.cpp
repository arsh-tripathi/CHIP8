// Includes here
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <fstream>

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

	Chip8 c{15, argv[1]};
	c.run();

	return 0;
}
