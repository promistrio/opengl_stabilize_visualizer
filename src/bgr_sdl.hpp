#include <SDL2/SDL.h>


#include <SDL2/SDL_opengl.h>
#include <chrono>
#include <iostream>

#define WIN_WIDTH  1920
#define WIN_HEIGHT 1080

class bgrSDL{
    public:
        void init();
        void refresh();
        void destruct();
        void loadTexture(unsigned char * textureData);

        //GLuint loadTexture();

    private:
        bool cold_start = true;
        int done;
        SDL_Window *window;
        SDL_Surface *surface;
        GLuint texture;
        GLfloat texcoords[4];
        
};