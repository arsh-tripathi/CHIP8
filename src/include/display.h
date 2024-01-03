#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "SDL2/SDL.h"
#include <iostream>
#include <vector>
#include <map>

class Display {
    SDL_Window *window;
    SDL_Texture *texture;
    SDL_Renderer *renderer;
    Uint32 *pixels;
    Uint32 bgcolor;
    Uint32 drawcolor;
    public:
        Display(Uint8 scale);
        ~Display();
        void updateDisplay(std::vector<std::vector<bool>> newPixels);
        void clearDisplay();
        void buzz();
        bool isKeyDown(Uint16 key);
    private:
        void updatePixel(int x, int y, Uint32 color);
};

#endif