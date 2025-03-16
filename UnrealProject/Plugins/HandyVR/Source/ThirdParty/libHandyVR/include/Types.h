#pragma once

#include "Platform.h"

namespace ump {
    class UMP_API Landmark {
    public:
        inline float &x() { return values[0]; }
        inline float &y() { return values[1]; }
        inline float &z() { return values[2]; }
    private:
        float values[3];
    };

    enum UMP_API Handedness {
        LEFT,
        RIGHT
    };

    class UMP_API HandLandmarks {
    private:
        Landmark values[21];
        Handedness _handedness;
    public:
        inline Landmark& operator[](unsigned index) { return values[index]; }
        inline const Landmark& operator[](unsigned index) const { return values[index]; }
        inline Handedness& getHandedness() { return _handedness; }
        inline const Handedness& getHandedness() const { return _handedness; }
    };

    typedef void (*HandLandmarksCallback)(HandLandmarks *);
}