#ifndef __LUNA_DEFINES_H__
#define __LUNA_DEFINES_H__

#include <include/r3kt/defines.h>

#ifdef _LUNA_INTERNAL_
    #include <include/r3kt/mem/alloc.h>
    #include <include/r3kt/mem/arena.h>
    #include <include/r3kt/io/log.h>
    #include <include/r3kt/io/buf.h>
    #include <include/r3kt/ds/arr.h>
    #include <include/r3kt/math.h>
#endif

#define LUNA_ENGINE_STRING "Luna 0.0.1"

typedef char* str;
typedef const char* cstr;

#define LUNA_SCREEN_QUAD_VERTEX_PROGRAM ".external/.data/assets/shaders/default/screen.vert"
#define LUNA_SCREEN_QUAD_FRAGMENT_PROGRAM  ".external/.data/assets/shaders/default/screen.frag"

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

#endif // __LUNA_DEFINES_H__