#ifndef __LUNA_PLATFORM_H__
#define __LUNA_PLATFORM_H__

#define _LUNA_INTERNAL_
#include <include/luna/core/defines.h>

typedef enum LunaWindowFlag {
    WINDOW_FOCUSED              = 1 << 0,
    WINDOW_SHOW_CURSOR          = 1 << 1,
    WINDOW_BIND_CURSOR          = 1 << 2,
    WINDOW_CENTER_CURSOR        = 1 << 3
} LunaWindowFlag;

typedef struct LunaWindow {
    Vec2 location;
    Vec2 size;
    f32 aspect;
    u32 flags;
    str title;
} LunaWindow;

typedef struct LunaLibrary {
    str name;
    ptr handle;
} LunaLibrary;

typedef struct LunaPlatformApi {
    LUNA_FNPTR(u8, createWindow, str title, u32 width, u32 height, u32 x, u32 y, LunaWindow* window);
    LUNA_FNPTR(none, destroyWindow, none);

    LUNA_FNPTR(none, pollEvents, none);
    LUNA_FNPTR(none, pollInputs, none);

    LUNA_FNPTR(none, swapBuffers, none);
    LUNA_FNPTR(u8, createGLContext, none);
    LUNA_FNPTR(none, destroyGLContext, none);

    LUNA_FNPTR(u8, getWindowFlag, u32 flag);
    LUNA_FNPTR(u8, setWindowFlag, u32 flag);
    LUNA_FNPTR(u8, remWindowFlag, u32 flag);
    LUNA_FNPTR(LunaWindow*, getWindow, none);

    LUNA_FNPTR(u8, unloadLibrary, LunaLibrary* library);
    LUNA_FNPTR(u8, loadLibrary, str path, str name, LunaLibrary* library);
    LUNA_FNPTR(u8, loadLibrarySymbol, str name, ptr* symbol, LunaLibrary* library);
} LunaPlatformApi;
LUNA_API LunaPlatformApi* lunaPlatformApi;

LUNA_API u8 lunaInitPlatform(none);
LUNA_API u8 lunaDeinitPlatform(none);

#endif // __LUNA_PLATFORM_H__
