#include <utility>
#include <limits.h>
#include "CaptureLib.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/features2d/features2d.hpp"
//#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/flann/flann.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <fstream>
#include <time.h>
#include "videostab.h"

using namespace std;
using namespace cv;

// This class redirects cv::Exception to our process so that we can catch it and handle it accordingly.
class cvErrorRedirector {
public:
    int cvCustomErrorCallback()
    {
        std::cout << "A cv::Exception has been caught. Skipping this frame..." << std::endl;
        return 0;
    }

    cvErrorRedirector() {
        //cvRedirectError((cv::ErrorCallback)cvCustomErrorCallback(), this);
    }
};

#define  HORIZONTAL_BORDER_CROP 30;

static cvErrorRedirector redir;

//Create a object of stabilization class
static VideoStab stab;

//Initialize the VideoCapture object
static VideoCapture * cap;

Mat frame_2;
Mat frame_1;
bool is_init = false;



bool InitFrame(cv::Mat _frame1){
    _frame1.copyTo(frame_1);
}

bool setFrame(Mat new_frame)
{
    if (!is_init){
        InitFrame(new_frame);
        is_init = true;
        return true;
    }
    try {
            //cvtColor(frame_2, frame2, COLOR_BGR2GRAY);

            cv::Mat smoothedFrame;

            smoothedFrame = stab.stabilize(frame_1, new_frame);

            imshow("Stabilized Video", smoothedFrame);

            if (cv::waitKey(1) == 0x1b){
            exit(0);
        }

            frame_1 = new_frame.clone();
    }
    catch (cv::Exception& e) {
        *cap >> frame_1;
        //cvtColor(frame_1, frame1, COLOR_BGR2GRAY);
    }
    std::cout << std::endl;
    return true;
}