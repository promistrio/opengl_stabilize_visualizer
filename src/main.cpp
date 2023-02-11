/*
 * Read video frame with FFmpeg and convert to OpenCV image
 *
 * Copyright (c) 2016 yohhoy
 */
#include <iostream>
#include <vector>
#include <thread>


//#include "yuv_gl.hpp"
#include "renders/bgr_sdl.hpp"
#include "opencv_file_sink.hpp"
#include "models/render_status.hpp"

using namespace std;
using namespace ffmpegcpp;

void glWinLoop(RenderStatus & renderstatus)
{
    // init GL Window
    BGRwindow::bgrSDL sdl_win (renderstatus);
    sdl_win.init();
    
    while(true)
    {
        sdl_win.refresh();  
    }
    sdl_win.destruct();
}

void glFFmpegLoop(RenderStatus & renderstatus)
{
    try
	{
        Demuxer* demuxer = new Demuxer("/media/user/Common/data/БП/Вертушки/MP4/2022-06-15_19-45-00_Cam2.mp4");//../../samples/big_buck_bunny.mp4
        StreamCapturer::OpenCVFileSink* fileSink = new StreamCapturer::OpenCVFileSink(renderstatus);
        demuxer->DecodeBestVideoStream(fileSink);
        demuxer->PreparePipeline();    

        while(!demuxer->IsDone())
        {
            demuxer->Step();
        }

        delete demuxer;
		delete fileSink;
    }
    catch (FFmpegException e)
	{
		cerr << "Exception caught!" << endl;
		throw e;
	}
}



int main()
{    
    RenderStatus renderStatus;
    std::thread glWindow(glWinLoop, std::ref(renderStatus));
    std::thread glFFmpeg(glFFmpegLoop, std::ref(renderStatus));

    glWindow.join();
    glFFmpeg.join();

    // This example will decode a video stream from a container and output it as raw image data, one image per frame.

    //std::thread glWindow(glWinLoop);

    //sdl_win.loadTexture(bgr.data); callback

    // done
    
    //glWindow.join();
		
	cout << "Decoding complete!" << endl;
}