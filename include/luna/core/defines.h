#define LUNA_ENGINE_STRING "Luna 0.0.1"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
    #define LUNA_PLATFORM_WINDOWS
#elif defined(__linux__) || defined(__gnu_linux__)
    #define LUNA_PLATFORM_LINUX
#endif

/*
 * LUNA_STATIC is defined for static library.
 * LUNA_BUILD  is defined for building the DLL library.
 */
#ifdef LUNA_STATIC
    #define LUNA_API extern
#else
    #ifdef LUNA_BUILD
        #define LUNA_API extern __declspec(dllexport)
    #else
        #define LUNA_API extern __declspec(dllimport)
#endif
#endif

#define LUNA_FNPTR(ret, sym, ...) ret (*sym)(__VA_ARGS__)

