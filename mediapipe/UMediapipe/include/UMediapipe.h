#pragma once

#include "Types.h"

namespace ump {
    class UMP_API UMediapipe {
    private:
        unsigned frameWidth;
        unsigned frameHeight;
    public:
        UMediapipe(HandDetectionCallback handCallback, unsigned frameWidth, unsigned frameHeight);
        void sendFrame(unsigned char *data);
        void beginHandDetection();
        void stopHandDetection();
        ~UMediapipe();
    };
}