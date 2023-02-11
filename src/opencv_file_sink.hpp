#pragma once

// OpenCV
#include "CaptureLib.h"
#include <opencv2/opencv.hpp>

// FFmpeg
extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
#include <libavformat/avio.h>
#include <libavutil/file.h>
}

#include "models/render_status.hpp"

#include "ffmpegcpp.h"
using namespace ffmpegcpp;

namespace StreamCapturer
{

    class OpenCVFileSink : public VideoFrameSink, public FrameWriter
    {
    public:
        OpenCVFileSink(RenderStatus & renderstatus):_renderStatus(renderstatus)
        {

        }

        FrameSinkStream *CreateStream()
        {
            stream = new FrameSinkStream(this, 0);
            return stream;
        }

        virtual void WriteFrame(int /* streamIndex */, AVFrame *frame, StreamData * /* streamData */)
        {
            /*sdl_win.refresh( frame->data[0],
                                frame->data[1],
                                frame->data[2]);*/

            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

            cv::Mat imageY(cv::Size(frame->width, frame->height), CV_8UC1, frame->data[0]);
            cv::Mat imageU(cv::Size(frame->width / 2, frame->height / 2), CV_8UC1, frame->data[1]);
            cv::Mat imageV(cv::Size(frame->width / 2, frame->height / 2), CV_8UC1, frame->data[2]);

            cv::Mat u_resized, v_resized;
            cv::resize(imageU, u_resized, cv::Size(frame->width, frame->height), 0, 0, cv::INTER_NEAREST);
            cv::resize(imageV, v_resized, cv::Size(frame->width, frame->height), 0, 0, cv::INTER_NEAREST);

            cv::Mat yuv;

            std::vector<cv::Mat> yuv_channels = {imageY, u_resized, v_resized};
            cv::merge(yuv_channels, yuv);

            /*cv::imshow("Y", imageY);
            cv::imshow("U", imageU);
            cv::imshow("V", imageV);
            //cv::imshow("BGR", bgr);

            if (cv::waitKey(1) == 0x1b){
                exit(0);
            }*/

            
            // if render ready to show our texture
            if (_renderStatus.canPrepareTexture())
            {
                //get pointer on cv::Mat from _renderStatus and change it
                cv::Mat bgr =  this->_renderStatus.getTextureMat();
                cv::cvtColor(yuv, bgr, cv::COLOR_YUV2BGR);
                setFrameBGR(imageY, bgr);
                this->_renderStatus.textureWasPrepared();
            }
            

            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            std::cout << "Frame duration = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
        }

        virtual void Close(int /* streamIndex */)
        {
            delete stream;
        }

        virtual bool IsPrimed()
        {
            // Return whether we have all information we need to start writing out data.
            // Since we don't really need any data in this use case, we are always ready.
            // A container might only be primed once it received at least one frame from each source
            // it will be muxing together (see Muxer.cpp for how this would work then).
            return true;
        }

    private:
        char fileNameBuffer[1024];
        int frameNumber = 0;
        FrameSinkStream *stream;

        RenderStatus & _renderStatus;
    };

};