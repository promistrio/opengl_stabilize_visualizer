/*
 * Read video frame with FFmpeg and convert to OpenCV image
 *
 * Copyright (c) 2016 yohhoy
 */
#include <iostream>
#include <vector>
#include <thread>
#include <capture_lib.h>

//#include "yuv_gl.hpp"
#include "renders/bgr_sdl.hpp"

using namespace std;

void glWinLoop()
{
    // init GL Window
    BGRwindow::bgrSDL sdl_win;
    sdl_win.init();
    
    while(sdl_win.running())
    {
        sdl_win.refresh();  
    }
    sdl_win.destruct();
}

int main()
{   
    std::thread glWindow(glWinLoop);

    char url[] = "udp://127.0.0.1:2399 -framedrop";
    startThread(url);
    

    glWindow.join();
		
	cout << "Decoding complete!" << endl;
}