#ifndef __R3N_DEFINES_H__
#define __R3N_DEFINES_H__

#include <r3kit/include/defines.h>

#ifdef _R3KIT_INCLUDE_
    #include <r3kit/include/mem/alloc.h>
    #include <r3kit/include/mem/arena.h>
    #include <r3kit/include/io/log.h>
    #include <r3kit/include/io/buf.h>
    #include <r3kit/include/ds/arr.h>
    #include <r3kit/include/math.h>
#endif

#define R3N_ENGINE_STRING "Luna 0.0.1"

typedef char* str;
typedef const char* cstr;

#define R3N_SCREEN_QUAD_VERTEX_PROGRAM "r3n.data/assets/shaders/default/screen.vert"
#define R3N_SCREEN_QUAD_FRAGMENT_PROGRAM  "r3n.data/assets/shaders/default/screen.frag"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
    #define R3N_PLATFORM_WINDOWS
#elif defined(__linux__) || defined(__gnu_linux__)
    #define R3N_PLATFORM_LINUX
#endif

/*
 * R3N_STATIC is defined for static library.
 * R3N_BUILD  is defined for building the DLL library.
 */
#ifdef R3N_STATIC
    #define R3N_API extern
#else
    #ifdef R3N_BUILD
        #define R3N_API extern __declspec(dllexport)
    #else
        #define R3N_API extern __declspec(dllimport)
#endif
#endif

#define R3N_FNPTR(ret, sym, ...) ret (*sym)(__VA_ARGS__)

#endif // __R3N_DEFINES_H__