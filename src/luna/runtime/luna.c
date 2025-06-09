#include <include/luna/runtime/luna.h>
#include <include/luna/core/platform.h>

static struct LunaRuntimeInternal {
    LunaWindow window;
    LunaEvents events;
    LunaInputs inputs;
    LunaPlatform platform;
    LunaRenderer renderer;
    LunaRuntimeApi userApi;
    LunaRuntimeConfig config;
    LunaLibrary userLibrary;
    byte running;
} LunaRuntimeInternal = {0};

LunaEvents* lunaEvents;
LunaInputs* lunaInputs;
LunaPlatform* lunaPlatform;
LunaRenderer* lunaRenderer;

none lunaConfigureRuntime(LunaRuntimeConfig config) {
    LunaRuntimeInternal.config.title = (!config.title) ? LUNA_ENGINE_STRING : config.title;

    LunaRuntimeInternal.config.backend = (
        config.backend == LUNA_BACKEND_INVALID
    ) ? LUNA_BACKEND_OPENGL : config.backend;

    LunaRuntimeInternal.config.windowPos = (
        config.windowPos.x < 0.0 || config.windowPos.y < 0.0
    ) ? (Vec2){0.0, 0.0} : config.windowPos;

        LunaRuntimeInternal.config.windowSize = (
        config.windowSize.x <= 0.0 || config.windowPos.y <= 0.0
    ) ? (Vec2){1280.0, 720.0} : config.windowSize;

    LunaRuntimeInternal.running = SSDK_TRUE;
}

byte lunaInitRuntimeImpl(str library) {
    byte result = SSDK_TRUE;

    ssdkInitMemory();
    ssdkInitMath();
    ssdkInitFile();
    ssdkInitLog();
    ssdkInitDS();
    
    lunaEvents = &LunaRuntimeInternal.events;
    if (!lunaInitEvents(&LunaRuntimeInternal.events)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntimeInternal] failed to initialize events");
        result = SSDK_FALSE;
    } else saneLog->log(SANE_LOG_SUCCESS, "[LunaRuntimeInternal] initialized events");
    
    // TODO: pass ptr to LunaEvents to LunaInputsInternal
    lunaInputs = &LunaRuntimeInternal.inputs;
    if (!lunaInitInputs(&LunaRuntimeInternal.inputs)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntimeInternal] failed to initialize inputs");
        result = SSDK_FALSE;
    } else saneLog->log(SANE_LOG_SUCCESS, "[LunaRuntimeInternal] initialized inputs");

    // TODO: pass ptr to LunaEvents and LunaInputs to LunaPlatformInternal
    lunaPlatform = &LunaRuntimeInternal.platform;
    if (!lunaInitPlatform(&LunaRuntimeInternal.platform)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntimeInternal] failed to initialize platform");
        result = SSDK_FALSE;
    } else saneLog->log(SANE_LOG_SUCCESS, "[LunaRuntimeInternal] initialized platform");

    if (!LunaRuntimeInternal.platform.loadLibrary("external/user", library, &LunaRuntimeInternal.userLibrary)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntimeInternal] failed to load user library");
        result = SSDK_FALSE;
    } else saneLog->log(SANE_LOG_SUCCESS, "[LunaRuntimeInternal] loaded user library");
    
    LunaRuntimeExportFunc exportFunc;
    if (!LunaRuntimeInternal.platform.loadLibrarySymbol("lunaExport", &LunaRuntimeInternal.userLibrary, &exportFunc)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntimeInternal] failed to load lunaExport symbol");
        if (!LunaRuntimeInternal.platform.unloadLibrary(&LunaRuntimeInternal.userLibrary)) {
            saneLog->log(SANE_LOG_ERROR, "[LunaRuntimeInternal] failed to unload user library");
        } result = SSDK_FALSE;
    } else saneLog->log(SANE_LOG_SUCCESS, "[LunaRuntimeInternal] loaded lunaExport symbol");
    
    LunaRuntimeInternal.userApi.configure = lunaConfigureRuntime;

    exportFunc(&LunaRuntimeInternal.userApi);
    if (!LunaRuntimeInternal.userApi.init || !LunaRuntimeInternal.userApi.update ||
        !LunaRuntimeInternal.userApi.render || !LunaRuntimeInternal.userApi.deinit) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntimeInternal] incomplete LunaRuntimeApi");
        if (!LunaRuntimeInternal.platform.unloadLibrary(&LunaRuntimeInternal.userLibrary)) {
            saneLog->log(SANE_LOG_ERROR, "[LunaRuntimeInternal] failed to unload user library");
        } result = SSDK_FALSE;
    } else saneLog->log(SANE_LOG_SUCCESS, "[LunaRuntimeInternal] LunaRuntimeApi exported");

    LunaRuntimeInternal.platform.createWindow(
        LunaRuntimeInternal.config.title,
        LunaRuntimeInternal.config.windowPos,
        LunaRuntimeInternal.config.windowSize,
        &LunaRuntimeInternal.window
    );

    lunaRenderer = &LunaRuntimeInternal.renderer;
    if (!lunaInitRenderer(LunaRuntimeInternal.config.backend, &LunaRuntimeInternal.renderer)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntimeInternal] failed to initialize renderer");
        result = SSDK_FALSE;
    } else saneLog->log(SANE_LOG_SUCCESS, "[LunaRuntimeInternal] initialized renderer");

    return result;
}

byte lunaDeinitRuntimeImpl(none) {
    byte result = SSDK_TRUE;

    if (!LunaRuntimeInternal.platform.unloadLibrary(&LunaRuntimeInternal.userLibrary)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntimeInternal] failed to unload user library");
    }

    if (!lunaDeinitRenderer(&LunaRuntimeInternal.renderer)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntimeInternal] failed to deinitialize renderer");
        result = SSDK_FALSE;
    }

    if (!lunaDeinitPlatform(&LunaRuntimeInternal.platform)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntimeInternal] failed to deinitialize platform");
        result = SSDK_FALSE;
    }
    
    if (!lunaDeinitInputs(&LunaRuntimeInternal.inputs)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntimeInternal] failed to deinitialize inputs");
        result = SSDK_FALSE;
    }
    
    if (!lunaDeinitEvents(&LunaRuntimeInternal.events)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntimeInternal] failed to deinitialize events");
        result = SSDK_FALSE;
    }

    ssdkExitDS();
    ssdkExitLog();
    ssdkExitFile();
    ssdkExitMath();
    ssdkExitMemory();
    
    LunaRuntimeInternal.userApi.init = NULL;
    LunaRuntimeInternal.userApi.update = NULL;
    LunaRuntimeInternal.userApi.render = NULL;
    LunaRuntimeInternal.userApi.deinit = NULL;
    LunaRuntimeInternal.userApi.configure = NULL;
    
    LunaRuntimeInternal.config.title = NULL;
    LunaRuntimeInternal.config.windowPos = (Vec2){0.0, 0.0};
    LunaRuntimeInternal.config.windowSize = (Vec2){0.0, 0.0};
    LunaRuntimeInternal.config.backend = LUNA_BACKEND_INVALID;
    
    saneLog->log(SANE_LOG_SUCCESS, "[LunaRuntimeInternal] deinitialized runtime");
    
    return result;
}


i32 main(int agrc, char** argv) {
    if (!lunaInitRuntimeImpl(argv[1])) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntimeInternal] failed to initialize runtime");
        return 1;
    } else saneLog->log(SANE_LOG_SUCCESS, "[LunaRuntimeInternal] initialized runtime");

    LunaRuntimeInternal.userApi.init();
    do {
        LunaRuntimeInternal.platform.pollEvents();
        LunaRuntimeInternal.platform.pollInputs();

        LunaRuntimeInternal.userApi.update(1.0);
        LunaRuntimeInternal.userApi.render();

        LunaRuntimeInternal.renderer.render();
        LunaRuntimeInternal.platform.swapBuffers();
    } while (LunaRuntimeInternal.running);
    LunaRuntimeInternal.userApi.deinit();
    
    if (!lunaDeinitRuntimeImpl()) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntimeInternal] failed to deinitialize runtime");
        return 1;
    } else saneLog->log(SANE_LOG_SUCCESS, "[LunaRuntimeInternal] deinitialized runtime");

    return 0;
}
