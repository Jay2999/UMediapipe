#pragma once

#include "Types.h"

namespace ump {
    class UMP_API UMediapipe {
    private:
        unsigned frameWidth;
        unsigned frameHeight;
    public:
        UMediapipe(HandLandmarksCallback handCallback, unsigned frameWidth, unsigned frameHeight);
        void sendFrame(unsigned char *data);
        void beginLandmarkDetection();
        void stopLandmarkDetection();
        ~UMediapipe();
    };
}