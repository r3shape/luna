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

// default runtime callbacks
byte exitCallback(LunaEventCode code, LunaEvent data) {
    if (code == LUNA_EVENT_EXIT) {
        saneLog->log(SANE_LOG_WARN, "[LunaRuntime] Exit Event");
        LunaRuntimeInternal.running = SSDK_FALSE;
        return SSDK_TRUE;
    }; return SSDK_FALSE;
}

byte resizeCallback(LunaEventCode code, LunaEvent data) {
    if (code == LUNA_EVENT_RESIZE) {
        saneLog->log(SANE_LOG_WARN, "[LunaRuntime] Resize Event");
        return SSDK_TRUE;
    }; return SSDK_FALSE;
}


none lunaConfigureRuntime(LunaRuntimeConfig config) {
    LunaRuntimeInternal.config.title = (!config.title) ? LUNA_ENGINE_STRING : config.title;

    LunaRuntimeInternal.config.backend = (
        config.backend == LUNA_BACKEND_INVALID
    ) ? LUNA_BACKEND_OPENGL : config.backend;

    LunaRuntimeInternal.config.windowPos = (
        config.windowPos.x < 0.0 || config.windowPos.y < 0.0
    ) ? (Vec2){0.0, 0.0} : config.windowPos;

        LunaRuntimeInternal.config.windowSize = (
        config.windowSize.x <= 0.0 || config.windowSize.y <= 0.0
    ) ? (Vec2){1280.0, 720.0} : config.windowSize;

    LunaRuntimeInternal.running = SSDK_TRUE;
}

byte lunaDeinitRuntimeImpl(none) {
    byte result = SSDK_TRUE;

    if (!LunaRuntimeInternal.platform.unloadLibrary(&LunaRuntimeInternal.userLibrary)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntime] failed to unload user library");
    }

    if (!lunaDeinitRenderer(&LunaRuntimeInternal.renderer)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntime] failed to deinitialize renderer");
        result = SSDK_FALSE;
    }

    if (!lunaDeinitPlatform(&LunaRuntimeInternal.platform)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntime] failed to deinitialize platform");
        result = SSDK_FALSE;
    }
    
    if (!lunaDeinitInputs(&LunaRuntimeInternal.inputs)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntime] failed to deinitialize inputs");
        result = SSDK_FALSE;
    }
    
    if (!lunaDeinitEvents(&LunaRuntimeInternal.events)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntime] failed to deinitialize events");
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
    
    saneLog->log(SANE_LOG_SUCCESS, "[LunaRuntime] deinitialized runtime");
    
    return result;
}

byte lunaInitRuntimeImpl(str library) {
    byte result = SSDK_TRUE;

    ssdkInitMemory();
    ssdkInitMath();
    ssdkInitFile();
    ssdkInitLog();
    ssdkInitDS();
    
    if (!lunaInitEvents(&LunaRuntimeInternal.events)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntime] failed to initialize events");
        return lunaDeinitRuntimeImpl();
    } else saneLog->log(SANE_LOG_SUCCESS, "[LunaRuntime] initialized events");
    
    if (!lunaInitInputs(&LunaRuntimeInternal.inputs, &LunaRuntimeInternal.events)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntime] failed to initialize inputs");
        return lunaDeinitRuntimeImpl();
    } else saneLog->log(SANE_LOG_SUCCESS, "[LunaRuntime] initialized inputs");

    if (!lunaInitPlatform(&LunaRuntimeInternal.platform, &LunaRuntimeInternal.events, &LunaRuntimeInternal.inputs)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntime] failed to initialize platform");
        return lunaDeinitRuntimeImpl();
    } else saneLog->log(SANE_LOG_SUCCESS, "[LunaRuntime] initialized platform");

    if (!LunaRuntimeInternal.platform.loadLibrary(LUNA_USER_PATH_DEFAULT, library, &LunaRuntimeInternal.userLibrary)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntime] failed to load user library");
        return lunaDeinitRuntimeImpl();
    } else saneLog->log(SANE_LOG_SUCCESS, "[LunaRuntime] loaded user library");
    
    ptr exportFunc;
    if (!LunaRuntimeInternal.platform.loadLibrarySymbol("lunaExport", &exportFunc, &LunaRuntimeInternal.userLibrary)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntime] failed to load lunaExport symbol");
        if (!LunaRuntimeInternal.platform.unloadLibrary(&LunaRuntimeInternal.userLibrary)) {
            saneLog->log(SANE_LOG_ERROR, "[LunaRuntime] failed to unload user library");
        } return lunaDeinitRuntimeImpl();
    } else saneLog->log(SANE_LOG_SUCCESS, "[LunaRuntime] loaded lunaExport symbol");
    
    LunaRuntimeInternal.userApi.configure = lunaConfigureRuntime;
    
    ((LunaRuntimeExportFunc)exportFunc)(&LunaRuntimeInternal.userApi);
    if (!LunaRuntimeInternal.userApi.init || !LunaRuntimeInternal.userApi.update ||
        !LunaRuntimeInternal.userApi.render || !LunaRuntimeInternal.userApi.deinit) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntime] incomplete LunaRuntimeApi");
        if (!LunaRuntimeInternal.platform.unloadLibrary(&LunaRuntimeInternal.userLibrary)) {
            saneLog->log(SANE_LOG_ERROR, "[LunaRuntime] failed to unload user library");
        } return lunaDeinitRuntimeImpl();
    } else saneLog->log(SANE_LOG_SUCCESS, "[LunaRuntime] LunaRuntimeApi exported");

    LunaRuntimeInternal.platform.createWindow(
        LunaRuntimeInternal.config.title,
        (u32)LunaRuntimeInternal.config.windowSize.x,
        (u32)LunaRuntimeInternal.config.windowSize.y,
        (u32)LunaRuntimeInternal.config.windowPos.x,
        (u32)LunaRuntimeInternal.config.windowPos.y,
        &LunaRuntimeInternal.window
    );

    if (!lunaInitRenderer(LunaRuntimeInternal.config.backend, &LunaRuntimeInternal.renderer, &LunaRuntimeInternal.platform)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntime] failed to initialize renderer");
        return lunaDeinitRuntimeImpl();
    } else saneLog->log(SANE_LOG_SUCCESS, "[LunaRuntime] initialized renderer");

    return result;
}


i32 main(int agrc, char** argv) {
    if (!lunaInitRuntimeImpl(argv[1])) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntime] failed to initialize runtime");
        return 1;
    } else saneLog->log(SANE_LOG_SUCCESS, "[LunaRuntime] initialized runtime");

    if (!LunaRuntimeInternal.events.registerCallback(LUNA_EVENT_EXIT, exitCallback)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntime] failed to register default exit callback");
        if (!lunaDeinitRuntimeImpl()) {
            saneLog->log(SANE_LOG_ERROR, "[LunaRuntime] failed to deinitialize runtime");
            return 1;
        } else saneLog->log(SANE_LOG_SUCCESS, "[LunaRuntime] deinitialized runtime");
    }

    if (!LunaRuntimeInternal.events.registerCallback(LUNA_EVENT_RESIZE, resizeCallback)) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntime] failed to register default resize callback");
        if (!lunaDeinitRuntimeImpl()) {
            saneLog->log(SANE_LOG_ERROR, "[LunaRuntime] failed to deinitialize runtime");
            return 1;
        } else saneLog->log(SANE_LOG_SUCCESS, "[LunaRuntime] deinitialized runtime");
    }

    LunaRuntimeInternal.userApi.init();
    do {
        LunaRuntimeInternal.platform.pollEvents();
        LunaRuntimeInternal.platform.pollInputs();

        if (LunaRuntimeInternal.inputs.keyIsDown(LUNA_KEY_F12)) {
            LunaRuntimeInternal.events.pushEvent(LUNA_EVENT_EXIT, (LunaEvent){0});
        }

        LunaRuntimeInternal.userApi.update(1.0);
        LunaRuntimeInternal.userApi.render();

        LunaRuntimeInternal.renderer.render();
        LunaRuntimeInternal.platform.swapBuffers();
    } while (LunaRuntimeInternal.running);
    LunaRuntimeInternal.userApi.deinit();
    
    if (!lunaDeinitRuntimeImpl()) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRuntime] failed to deinitialize runtime");
        return 1;
    } else saneLog->log(SANE_LOG_SUCCESS, "[LunaRuntime] deinitialized runtime");

    return 0;
}
