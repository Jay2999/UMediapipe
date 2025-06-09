#pragma once

#ifdef _MSC_VER
#define WINDOWS 1
#define ANDROID 0
#else
#define WINDOWS 0
#define ANDROID 1
#endif // _MSC_VER

#if WINDOWS
#ifdef DLL_EXPORT
    #define UMP_API __declspec(dllexport)
#else
    #define UMP_API __declspec(dllimport)
#endif // DLL_EXPORT
#endif // WINDOWS

#if ANDROID
    #define UMP_API
#endif // ANDROID