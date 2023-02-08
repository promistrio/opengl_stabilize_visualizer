#pragma once
#include <opencv2/opencv.hpp>
#include <SDL2/SDL.h>

#include <SDL2/SDL_opengl.h>
#include <chrono>
#include <iostream>

#define WIN_WIDTH 1920
#define WIN_HEIGHT 1080

namespace BGRwindow
{

    static unsigned char *textureData;
    static bool canLoad = false;

    static void loadTexture(unsigned char *texture_data)
    {
        textureData = texture_data;
        canLoad = true;
        if (canLoad == true)
        {
            std::cout << "canLoad = true" << std::endl;
        }
    }

    class bgrSDL
    {
    public:
        bgrSDL(std::function<void(void)> callback)
        {
            uploaded_callback = callback;
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
        std::function<void(void)> uploaded_callback;
    };
};