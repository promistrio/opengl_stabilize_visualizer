#pragma once
#include <SDL2/SDL.h>

#include <SDL2/SDL_opengl.h>
#include <chrono>
#include <iostream>
#include <capture_lib.h>

#define WIN_WIDTH 1920
#define WIN_HEIGHT 1080

namespace BGRwindow
{
    class bgrSDL
    {
    public:
        bgrSDL()
        {

        }
        void init();
        void refresh();
        void destruct();
        void updateTexture();
        bool running();

    private:
        int counter = 0;
        GLuint texture;
        bool cold_start = true;
        int done;
        SDL_Window *window;
        SDL_Surface *surface;
        bool _running = true;
    };
};