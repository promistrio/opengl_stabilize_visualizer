#pragma once
#include <opencv2/opencv.hpp>


extern "C" void captureLibInit();
extern "C" void startThread();

extern "C" bool setFrame(cv::Mat frame);

extern "C" bool setFrameBGR(cv::Mat gray, cv::Mat bgr);