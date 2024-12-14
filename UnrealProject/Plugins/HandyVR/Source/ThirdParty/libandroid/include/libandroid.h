#ifdef _MSC_VER
    #define WEAK 
#else
    #define WEAK __attribute__((weak))
#endif

extern "C" {
    int getNumber() WEAK;
    const char* getText() WEAK;
}