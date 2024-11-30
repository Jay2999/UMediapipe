#include "mediapipe/UMediapipe/include/UMediapipe.h"
#include <iostream>

#include <windows.h>

BOOL WINAPI consoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        stopLandmarkDetection();
    }
    return TRUE;
}

void onLandmarkReceived(HandLandmarks* landmarks) {
    std::cout << landmarks->values[0].x() << std::endl;
}

int main() {
    SetConsoleCtrlHandler(consoleHandler, TRUE);
    beginLandmarkDetection(onLandmarkReceived);
    waitForEnd();
}