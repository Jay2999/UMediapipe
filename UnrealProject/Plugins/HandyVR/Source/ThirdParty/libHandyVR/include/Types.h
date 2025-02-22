#include "Platform.h"

namespace UMediapipe {
    class UMP_API Landmark {
    public:
        inline float &x() { return values[0]; }
        inline float &y() { return values[1]; }
        inline float &z() { return values[2]; }
    private:
        float values[3];
    };

    class UMP_API HandLandmarks {
    private:
        Landmark values[21];
    public:
        inline Landmark &operator[](unsigned index) { return values[index]; }
        inline const Landmark &operator[](unsigned index) const { return values[index]; }
    };

    typedef void (*HandLandmarksCallback)(HandLandmarks *);
}