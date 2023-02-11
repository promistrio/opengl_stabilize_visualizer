#pragma once
#include <opencv2/opencv.hpp>
#include <SDL2/SDL.h>

#include <SDL2/SDL_opengl.h>
#include <chrono>
#include <iostream>
#include "../models/render_status.hpp"

#define WIN_WIDTH 1920
#define WIN_HEIGHT 1080

namespace BGRwindow
{
    class bgrSDL
    {
    public:
        bgrSDL(RenderStatus & renderstatus):_renderStatus(renderstatus)
        {
            std::cout << "canLoad = false" << std::endl;
        }
        void init();
        void refresh();
        void destruct();
        void updateTexture();

    private:
        int counter = 0;
        GLuint texture;
        bool cold_start = true;
        int done;
        SDL_Window *window;
        SDL_Surface *surface;
        RenderStatus &_renderStatus;
    };
};