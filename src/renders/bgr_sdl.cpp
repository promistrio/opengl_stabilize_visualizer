#include "bgr_sdl.hpp"
#include <iostream>
#include <chrono>
#include <thread>


using namespace BGRwindow;

void bgrSDL::init()
{
    this->window = SDL_CreateWindow("OpenGL Test", 0, 0, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext Context = SDL_GL_CreateContext(this->window);
}

void bgrSDL::updateTexture()
{
    
      glEnable(GL_TEXTURE_2D);
      if (this->_renderStatus.canUseTexture() == true)
      {
        std::cout << "canUseTexture" << std::endl;

        // if first loading than init texture 
        if (this->cold_start)
        {
          std::cout << "load texture" << std::endl;
          glGenTextures( 1, &texture );
          glBindTexture( GL_TEXTURE_2D, texture );

          glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
          glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);

          glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, WIN_WIDTH, WIN_HEIGHT, 0, GL_BGR,
          GL_UNSIGNED_BYTE, _renderStatus.getTexturePointer());
          this->cold_start = false;
        }
        else
        {
          std::cout << "update texture" << std::endl;
          glBindTexture( GL_TEXTURE_2D, texture );

          glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
          glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);

          glTexSubImage2D(GL_TEXTURE_2D,0,0,0, WIN_WIDTH,WIN_HEIGHT,GL_BGR,GL_UNSIGNED_BYTE,_renderStatus.getTexturePointer());
        }

        //tell ffmpeg process, that it can change texture
        std::cout << "canLoad = false" << std::endl;
      }
      _renderStatus.textureWasUsed();
    
}

void bgrSDL::refresh()
{
    this->updateTexture();
    SDL_Event Event;
    while (SDL_PollEvent(&Event))
    {
      if (Event.type == SDL_KEYDOWN)
      {
        /*switch (Event.key.keysym.sym)
        {
          case SDLK_ESCAPE:
            Running = 0;
            break;
          case 'f':
            FullScreen = !FullScreen;
            if (FullScreen)
            {
              SDL_SetWindowFullscreen(Window, WindowFlags | SDL_WINDOW_FULLSCREEN_DESKTOP);
            }
            else
            {
              SDL_SetWindowFullscreen(Window, WindowFlags);
            }
            break;
          default:
            break;
        }*/
      }
      else if (Event.type == SDL_QUIT)
      {
        //Running = 0;
      }
    }

    

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,WIN_WIDTH,0,WIN_HEIGHT,-1,1);
    glViewport(0,0,WIN_WIDTH,WIN_WIDTH);
    glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
    glColor3f(1.0f,1.0f,1.0f);

    glEnable(GL_TEXTURE_2D);

    int width = 1920;
    int height = 1080;

    glBegin(GL_QUADS);
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexCoord2f(0,0); glVertex2i(0,0);
    glTexCoord2f(1,0); glVertex2i(width,0);
    glTexCoord2f(1,1); glVertex2i(width,height);
    glTexCoord2f(0,1); glVertex2i(0,height);
    glEnd();

    SDL_GL_SwapWindow(this->window);
}
void bgrSDL::destruct()
{
    
}