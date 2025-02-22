#define APP_NAME "UMediapipe"
#ifdef DEBUG_BUILD

#if ANDROID
#include <android/log.h>
#define DEBUG(MSG) __android_log_print(ANDROID_LOG_VERBOSE, APP_NAME, MSG);

#elif WINDOWS
#include "absl/log/absl_log.h"
#define DEBUG(MSG) ABSL_LOG(INFO) << MSG;
#endif // ANDROID

#else
#define DEBUG(MSG)
#endif // DEBUG_BUILD