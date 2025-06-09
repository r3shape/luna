#ifndef __LUNA_PLATFORM_H__
#define __LUNA_PLATFORM_H__

#include <include/SSDK/SSDK.h>
#include <include/luna/core/defines.h>

typedef enum LunaWindowFlag {
    WINDOW_FOCUSED              = 1 << 0,
    WINDOW_SHOW_CURSOR          = 1 << 1,
    WINDOW_BIND_CURSOR          = 1 << 2,
    WINDOW_CENTER_CURSOR        = 1 << 3
} LunaWindowFlag;

typedef struct LunaWindow {
    u32 flags;
    str title;
    Vec2 size;
    f32 aspect;
    Vec2 location;
} LunaWindow;

typedef struct LunaLibrary {
    str name;
    ptr handle;
} LunaLibrary;

typedef struct LunaPlatform {
    LUNA_FNPTR(byte, createWindow, str title, Vec2 size, Vec2 location, LunaWindow* window);
    LUNA_FNPTR(none, destroyWindow, none);
    
    LUNA_FNPTR(none, pollEvents, none);
    LUNA_FNPTR(none, pollInputs, none);
    
    LUNA_FNPTR(none, swapBuffers, none);
    LUNA_FNPTR(byte, createGLContext, none);
    LUNA_FNPTR(none, destroyGLContext, none);

    LUNA_FNPTR(byte, getWindowFlag, u32 flag);
    LUNA_FNPTR(byte, setWindowFlag, u32 flag);
    LUNA_FNPTR(byte, remWindowFlag, u32 flag);

    LUNA_FNPTR(byte, unloadLibrary, LunaLibrary* library);
    LUNA_FNPTR(byte, loadLibrary, str path, str name, LunaLibrary* library);
    LUNA_FNPTR(byte, loadLibrarySymbol, str name, ptr* symbol, LunaLibrary* library);
} LunaPlatform;
extern LunaPlatform* lunaPlatform;

LUNA_API byte lunaInitPlatform(LunaPlatform* table);
LUNA_API byte lunaDeinitPlatform(LunaPlatform* table);

#endif // __LUNA_PLATFORM_H__