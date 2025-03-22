#pragma once

#include "Platform.h"

namespace ump {
    class UMP_API Landmark {
    public:
        inline float& x() { return values[0]; }
        inline const float& x() const { return values[0]; }
        inline float& y() { return values[1]; }
        inline const float& y() const { return values[1]; }
        inline float& z() { return values[2]; }
        inline const float& z() const { return values[2]; }
    private:
        float values[3];
    };

    class UMP_API HandLandmarks {
    private:
        Landmark values[21];
    public:
        inline Landmark& operator[](unsigned index) { return values[index]; }
        inline const Landmark& operator[](unsigned index) const { return values[index]; }
    };

    enum UMP_API Handedness {
        LEFT,
        RIGHT
    };

    enum UMP_API HandGestures {
        NONE,
        OPEN_PALM,
        CLOSED_FIST
    };

    class UMP_API HandDetectionResult {
    private:
        HandLandmarks landmarks;
        Handedness handedness;
        HandGestures gesture;
    public:
        inline HandLandmarks& Landmarks() { return landmarks; }
        inline const HandLandmarks& Landmarks() const { return landmarks; }
        inline Landmark& Landmarks(unsigned index) { return landmarks[index]; }
        inline const Landmark& Landmarks(unsigned  index) const { return landmarks[index]; }
        inline Handedness& getHandedness() { return handedness; }
        inline const Handedness& getHandedness() const { return handedness; }
        inline HandGestures& Gesture() { return gesture; }
        inline const HandGestures& Gesture() const { return gesture; }
    };

    typedef void (*HandDetectionCallback)(HandDetectionResult*);
}