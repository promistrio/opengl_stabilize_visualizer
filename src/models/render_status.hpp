#pragma once
#include "../CaptureLib.h"
#include <opencv2/opencv.hpp>
#include <mutex>

class RenderStatus
{
private:
    std::mutex _mutex;
    bool _canUseTexture = false;
    bool _canPrepareTexture = true;
    cv::Mat _bgrtest = cv::Mat(1080, 1920, CV_8UC3, cv::Scalar(94,206,165));
public:
    bool canUseTexture();
    bool canPrepareTexture();
    void textureWasUsed();
    void textureWasPrepared();
    unsigned char * getTexturePointer();
    cv::Mat getTextureMat();
    /*void setTextureMat(cv::Mat mat)
    {
        _bgrtest = mat;
    }*/
};



