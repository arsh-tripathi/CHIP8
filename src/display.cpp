#include "include/display.h"
static int HEIGHT = 32;
static int WIDTH = 64;
static int SCALE = 1;

using namespace std;

Display::Display(Uint8 scale) {
	SCALE = scale;
	HEIGHT *= SCALE;
	WIDTH *= SCALE;
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		SDL_Log("Failed to intialize SDL: %s", SDL_GetError());
		exit(1);
	}
	window = SDL_CreateWindow("CHIP8", 100, 100, WIDTH, HEIGHT, 0);
	if (!window) {
		SDL_Log("Couldn't create window: %s\n", SDL_GetError());
		exit(1);
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		SDL_Log("Couldn't create renderer: %s\n", SDL_GetError());
		exit(1);
	}

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
	if (!texture) {
		SDL_Log("Couldn't create texture: %s\n", SDL_GetError());
		exit(1);
	}
	pixels = new Uint32[WIDTH*HEIGHT];

	bgcolor = SDL_MapRGBA(SDL_GetWindowSurface(window)->format, 0, 0, 0, 255);
	drawcolor = SDL_MapRGBA(SDL_GetWindowSurface(window)->format, 0, 255, 166, 255);
	for (int i = 0; i < WIDTH*HEIGHT; ++i) {
		pixels[i] = bgcolor;
	}
	SDL_UpdateTexture(texture, nullptr, pixels, WIDTH * sizeof(Uint32));
}

Display::~Display() {
	delete[] pixels;
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Display::updatePixel(int x, int y, Uint32 color) {
	for (int i = x; i < x + SCALE; ++i) {
		for (int j = y; j < y  + SCALE; ++j) {
			pixels[j*WIDTH + i] = color;
		}
	}
}

void Display::clearDisplay() {
	for (int i = 0; i < WIDTH*HEIGHT; ++i) pixels[i] = bgcolor;
	SDL_UpdateTexture(texture, nullptr, pixels, WIDTH * sizeof(Uint32));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}

void Display::updateDisplay(vector<vector<bool>> newPixels) {
	for (size_t i = 0; i < newPixels.size(); ++i) {
		for (size_t j = 0; j < newPixels[i].size(); ++j) {
			updatePixel(j*SCALE, i*SCALE, newPixels[i][j] ? drawcolor : bgcolor);
			// cerr << "i , j, val : " << i << ", " << j << ", " << newPixels[i][j] << endl;
		}
	}
	SDL_UpdateTexture(texture, nullptr, pixels, WIDTH * sizeof(Uint32));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}

void Display::buzz() {}

bool Display::isKeyDown(Uint16 key) {
	std::map<Uint16, SDL_KeyCode> m {
		{0x1, SDLK_1}, {0x2, SDLK_2}, {0x3, SDLK_3}, {0xC, SDLK_4}, 
		{0x4, SDLK_q}, {0x5, SDLK_w}, {0x6, SDLK_e}, {0xD, SDLK_r},
		{0x5, SDLK_a}, {0x6, SDLK_s}, {0x6, SDLK_d}, {0xE, SDLK_f},
		{0xA, SDLK_z}, {0x0, SDLK_x}, {0xB, SDLK_c}, {0xF, SDLK_v}
	};
	const Uint8* state = SDL_GetKeyboardState(nullptr);
	return state[m[key]];
}