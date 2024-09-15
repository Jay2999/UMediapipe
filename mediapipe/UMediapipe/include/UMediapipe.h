#ifdef DLL_EXPORT
    #define UMP_API __declspec(dllexport)
#else
    #define UMP_API __declspec(dllimport)
#endif

typedef struct landmark_t {
public:
    inline float& x() { return values[0]; }
    inline float& y() { return values[1]; }
    inline float& z() { return values[2]; }
private:
    float values[3];
} Landmark;

typedef struct handLandmarks_t {
    Landmark values[21];
} HandLandmarks;

typedef void (*HandLandmarksCallback)(HandLandmarks*);

UMP_API void beginLandmarkDetection(HandLandmarksCallback callback);
UMP_API void stopLandmarkDetection();
UMP_API void waitForEnd();