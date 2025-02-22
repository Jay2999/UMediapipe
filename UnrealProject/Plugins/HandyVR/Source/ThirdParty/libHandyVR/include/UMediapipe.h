#include "Types.h"

/*UMP_API void UMP_CALL beginLandmarkDetection(HandLandmarksCallback callback);
UMP_API void UMP_CALL stopLandmarkDetection();
UMP_API void UMP_CALL waitForEnd();
UMP_API void UMP_CALL sendFrame(unsigned char* data, int width, int height);
*/

namespace UMediapipe {
    class UMP_API UMediapipe {
    private:
        HandLandmarksCallback leftHandCallback;
        HandLandmarksCallback rightHandCallback;
        unsigned frameWidth;
        unsigned frameHeight;
    public:
        UMediapipe(HandLandmarksCallback leftHandCallback, HandLandmarksCallback rightHandCallback, unsigned framwWidth, unsigned frameHeight);
        void sendFrame(unsigned char *data);
        void beginLandmarkDetection();
        void stopLandmarkDetection();
        ~UMediapipe();
    };
}