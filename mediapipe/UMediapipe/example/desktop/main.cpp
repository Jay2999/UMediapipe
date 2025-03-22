#include "mediapipe/UMediapipe/include/UMediapipe.h"
#include "mediapipe/framework/port/opencv_video_inc.h"
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include <iostream>

#include <windows.h>

using namespace cv;
using namespace std;

ump::UMediapipe* _ump = nullptr;

BOOL WINAPI consoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        _ump->stopHandDetection();
    }
    return TRUE;
}

void onLandmarkReceived(ump::HandDetectionResult* hand) {
    std::cout << hand->Landmarks(0).x() << std::endl;
}

int main() {
    SetConsoleCtrlHandler(consoleHandler, TRUE);
    _ump = new ump::UMediapipe(onLandmarkReceived, 640, 480);
    _ump->beginHandDetection();

    Mat frame;
    VideoCapture cap;
    int deviceID = 0;
    int apiID = cv::CAP_ANY;
    cap.open(deviceID, apiID);
    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open camera\n";
        return -1;
    }

    cout << "Start grabbing" << endl << "Press any key to terminate" << endl;
    for (;;)
    {
        // wait for a new frame from camera and store it into 'frame'
        cap.read(frame);
        // check if we succeeded
        if (frame.empty()) {
            cerr << "ERROR! blank frame grabbed\n";
            break;
        }
        //_ump->sendFrame(frame.ptr<unsigned char>());

        if (frame.isContinuous()) {
            _ump->sendFrame(frame.ptr<unsigned char>());
        }
        imshow("img", frame);
        if (waitKey(5) >= 0)
            break;
    }
    _ump->stopHandDetection();
    return 0;
}