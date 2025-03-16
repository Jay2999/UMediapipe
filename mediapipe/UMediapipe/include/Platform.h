#pragma once

#ifdef _MSC_VER
#define WINDOWS 1
#define ANDROID 0
#else
#define WINDOWS 0
#define ANDROID 1
#endif // _MSC_VER

#if WINDOWS
#define UMP_CALL
#ifdef DLL_EXPORT
    #define UMP_API __declspec(dllexport)
#else
    #define UMP_API __declspec(dllimport)
#endif // DLL_EXPORT
#endif // WINDOWS

#if ANDROID
#define UMP_API
#ifdef DLL_EXPORT
    #define UMP_CALL
#else
    #define UMP_CALL __attribute__((weak))
#endif // DLL_EXPORT
#endif // ANDROID