#ifndef __LUNA_H__
#define __LUNA_H__

#include <include/SSDK/SSDK.h>
#include <include/luna/core/events.h>
#include <include/luna/core/inputs.h>
#include <include/luna/core/renderer.h>

#define LUNA_USER_PATH_DEFAULT "external/user"

typedef struct LunaRuntimeConfig {
    str title;
    Vec2 windowPos;
    Vec2 windowSize;
    u32 backend;
} LunaRuntimeConfig;

typedef LUNA_FNPTR(none, LunaRuntimeInitFunc, none);
typedef LUNA_FNPTR(none, LunaRuntimeUpdateFunc, f32);
typedef LUNA_FNPTR(none, LunaRuntimeRenderFunc, none);
typedef LUNA_FNPTR(none, LunaRuntimeDeinitFunc, none);
typedef LUNA_FNPTR(none, LunaRuntimeConfigFunc, LunaRuntimeConfig);

typedef struct LunaRuntimeApi {
    LunaRuntimeInitFunc init;
    LunaRuntimeUpdateFunc update;
    LunaRuntimeRenderFunc render;
    LunaRuntimeDeinitFunc deinit;
    LunaRuntimeConfigFunc configure;
} LunaRuntimeApi;

typedef LUNA_FNPTR(none, LunaRuntimeExportFunc, LunaRuntimeApi*);

#endif // __LUNA_H__