#ifdef _MSC_VER
    #define UMP_CALL __cdecl
    #ifdef DLL_EXPORT
        #define UMP_API __declspec(dllexport)
    #else
        #define UMP_API __declspec(dllimport)
#endif // DLL_EXPORT
#else
    #define UMP_API extern "C"
    #ifdef DLL_EXPORT
        #define UMP_CALL
    #else
        #define UMP_CALL __attribute__((weak))
    #endif // DLL_EXPORT
#endif // _MSC_VER

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

typedef void (__cdecl *HandLandmarksCallback)(HandLandmarks*);

UMP_API void UMP_CALL beginLandmarkDetection(HandLandmarksCallback callback);
UMP_API void UMP_CALL stopLandmarkDetection();
UMP_API void UMP_CALL waitForEnd();
UMP_API void UMP_CALL sendFrame(unsigned char* data, int width, int height);