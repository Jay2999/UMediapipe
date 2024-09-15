#include "mediapipe/UMediapipe/include/UMediapipe.h"
#include <iostream>

void onLandmarkReceived(HandLandmarks* landmarks) {
    std::cout << landmarks->values[0].x() << std::endl;
}

int main() {
    beginLandmarkDetection(onLandmarkReceived);
    waitForEnd();
}