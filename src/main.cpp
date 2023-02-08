/*
 * Read video frame with FFmpeg and convert to OpenCV image
 *
 * Copyright (c) 2016 yohhoy
 */
#include <iostream>
#include <vector>
#include <thread>


//#include "yuv_gl.hpp"
#include "renders/yuv_sdl.h"
#include "renders/bgr_sdl.hpp"
#include <iostream>
#include "opencv_file_sink.h"

using namespace std;
using namespace ffmpegcpp;

void glWinLoop()
{
    

    //main loop
    while(true)
    {
        
    }

    
}



int main()
{    
	// This example will decode a video stream from a container and output it as raw image data, one image per frame.
	try
	{
        //std::thread glWindow(glWinLoop);
        // init GL Window
        BGRwindow::bgrSDL sdl_win (StreamCapturer::textureLoaded);
        sdl_win.init();

        //init ffmpeg lib
        Demuxer* demuxer = new Demuxer("I Am Legend - Trailer.mp4");//../../samples/big_buck_bunny.mp4
        StreamCapturer::OpenCVFileSink* fileSink = new StreamCapturer::OpenCVFileSink(BGRwindow::loadTexture);
        demuxer->DecodeBestVideoStream(fileSink);
        demuxer->PreparePipeline();

		while (!demuxer->IsDone())
		{
			demuxer->Step();
            sdl_win.refresh();
		}

        //sdl_win.loadTexture(bgr.data); callback
  
		// done
        sdl_win.destruct();
        //glWindow.join();
		delete demuxer;
		delete fileSink;
	}
	catch (FFmpegException e)
	{
		cerr << "Exception caught!" << endl;
		throw e;
	}

	cout << "Decoding complete!" << endl;
}