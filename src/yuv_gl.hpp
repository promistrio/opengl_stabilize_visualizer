#include <SDL2/SDL.h>


#include <SDL2/SDL_opengl.h>
#include <chrono>
#include <iostream>

class yuvGL{
    public:
        void init();
        void refresh();
        void destruct();

        //GLuint loadTexture();

    private:
        int done;
        SDL_Window *window;
        SDL_Surface *surface;
        GLuint texture;
        GLfloat texcoords[4];
};