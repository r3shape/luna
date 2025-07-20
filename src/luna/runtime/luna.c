#include <include/luna/runtime/luna.h>
#include <include/luna/core/platform.h>
#include <include/r3kt/io/log.h>

static struct LunaRuntimeInternal {
    u8 running;
    LunaWindow window;
    LunaRuntimeApi userApi;
    LunaLibrary userLibrary;
    LunaRuntimeConfig config;
} LunaRuntimeInternal = {0};


// default runtime callbacks
u8 exitCallback(LunaEventCode code, LunaEvent data) {
    if (code == LUNA_EVENT_EXIT) {
        r3_log_stdout(WARN_LOG, "[LunaRuntime] Exit Event\n");
        LunaRuntimeInternal.running = 0;
        return 1;
    }; return 0;
}

u8 resizeCallback(LunaEventCode code, LunaEvent data) {
    if (code == LUNA_EVENT_RESIZE) {
        r3_log_stdout(WARN_LOG, "[LunaRuntime] Resize Event\n");
        return 1;
    }; return 0;
}


none lunaConfigureRuntime(LunaRuntimeConfig config) {
    LunaRuntimeInternal.config.title = (!config.title) ? LUNA_ENGINE_STRING : config.title;

    LunaRuntimeInternal.config.backend = (
        config.backend == LUNA_BACKEND_INVALID
    ) ? LUNA_BACKEND_OPENGL : config.backend;

    LunaRuntimeInternal.config.windowPos = (
        VEC_X(config.windowPos) < 0.0 || VEC_Y(config.windowPos) < 0.0
    ) ? (Vec2){ .data = {0.0, 0.0} } : config.windowPos;

        LunaRuntimeInternal.config.windowSize = (
        VEC_X(config.windowSize) <= 0.0 || VEC_Y(config.windowSize) <= 0.0
    ) ? (Vec2){ .data = {1280, 720} } : config.windowSize;

    LunaRuntimeInternal.running = 1;
}

u8 lunaDeinitRuntimeImpl(none) {
    u8 result = 1;

    if (!lunaPlatformApi->unloadLibrary(&LunaRuntimeInternal.userLibrary)) {
        r3_log_stdout(ERROR_LOG, "[LunaRuntime] failed to unload user library\n");
    }

    if (!lunaDeinitRenderer()) {
        r3_log_stdout(ERROR_LOG, "[LunaRuntime] failed to deinitialize renderer\n");
        result = 0;
    }

    if (!lunaDeinitPlatform()) {
        r3_log_stdout(ERROR_LOG, "[LunaRuntime] failed to deinitialize platform\n");
        result = 0;
    }

    if (!lunaDeinitInputs()) {
        r3_log_stdout(ERROR_LOG, "[LunaRuntime] failed to deinitialize inputs\n");
        result = 0;
    }

    if (!lunaDeinitEvents() ){
        r3_log_stdout(ERROR_LOG, "[LunaRuntime] failed to deinitialize events\n");
        result = 0;
    }

    LunaRuntimeInternal.userApi.init = NULL;
    LunaRuntimeInternal.userApi.update = NULL;
    LunaRuntimeInternal.userApi.render = NULL;
    LunaRuntimeInternal.userApi.deinit = NULL;
    LunaRuntimeInternal.userApi.configure = NULL;

    LunaRuntimeInternal.config.title = NULL;
    LunaRuntimeInternal.config.windowPos = (Vec2){ .data = {0.0, 0.0} };
    LunaRuntimeInternal.config.windowSize = (Vec2){ .data = {0, 0} };
    LunaRuntimeInternal.config.backend = LUNA_BACKEND_INVALID;

    r3_log_stdout(SUCCESS_LOG, "[LunaRuntime] deinitialized runtime\n");

    return result;
}

u8 lunaInitRuntimeImpl(str library) {
    u8 result = 1;

    if (!lunaInitEvents()) {
        r3_log_stdout(ERROR_LOG, "[LunaRuntime] failed to initialize events\n");
        return lunaDeinitRuntimeImpl();
    } else r3_log_stdout(SUCCESS_LOG, "[LunaRuntime] initialized events\n");

    if (!lunaInitInputs()) {
        r3_log_stdout(ERROR_LOG, "[LunaRuntime] failed to initialize inputs\n");
        return lunaDeinitRuntimeImpl();
    } else r3_log_stdout(SUCCESS_LOG, "[LunaRuntime] initialized inputs\n");

    if (!lunaInitPlatform()) {
        r3_log_stdout(ERROR_LOG, "[LunaRuntime] failed to initialize platform\n");
        return lunaDeinitRuntimeImpl();
    } else r3_log_stdout(SUCCESS_LOG, "[LunaRuntime] initialized platform\n");

    if (!lunaPlatformApi->loadLibrary(LUNA_USER_PATH_DEFAULT, library, &LunaRuntimeInternal.userLibrary)) {
        r3_log_stdout(ERROR_LOG, "[LunaRuntime] failed to load user library\n");
        return lunaDeinitRuntimeImpl();
    } else r3_log_stdout(SUCCESS_LOG, "[LunaRuntime] loaded user library\n");

    ptr exportFunc;
    if (!lunaPlatformApi->loadLibrarySymbol("lunaExport", &exportFunc, &LunaRuntimeInternal.userLibrary) || exportFunc == NULL) {
        r3_log_stdout(ERROR_LOG, "[LunaRuntime] failed to load lunaExport symbol\n");
        if (!lunaPlatformApi->unloadLibrary(&LunaRuntimeInternal.userLibrary)) {
            r3_log_stdout(ERROR_LOG, "[LunaRuntime] failed to unload user library\n");
        } return lunaDeinitRuntimeImpl();
    } else r3_log_stdout(SUCCESS_LOG, "[LunaRuntime] loaded lunaExport symbol\n");

    LunaRuntimeInternal.userApi.configure = lunaConfigureRuntime;

    ((LunaRuntimeExportFunc)exportFunc)(&LunaRuntimeInternal.userApi);
    if (!LunaRuntimeInternal.userApi.init || !LunaRuntimeInternal.userApi.update ||
        !LunaRuntimeInternal.userApi.render || !LunaRuntimeInternal.userApi.deinit) {
        r3_log_stdout(ERROR_LOG, "[LunaRuntime] incomplete LunaRuntimeApi\n");
        if (!lunaPlatformApi->unloadLibrary(&LunaRuntimeInternal.userLibrary)) {
            r3_log_stdout(ERROR_LOG, "[LunaRuntime] failed to unload user library\n");
        } return lunaDeinitRuntimeImpl();
    } else r3_log_stdout(SUCCESS_LOG, "[LunaRuntime] LunaRuntimeApi exported\n");

    lunaPlatformApi->createWindow(
        LunaRuntimeInternal.config.title,
        (u32)VEC_X(LunaRuntimeInternal.config.windowSize),
        (u32)VEC_Y(LunaRuntimeInternal.config.windowSize),
        (u32)VEC_X(LunaRuntimeInternal.config.windowPos),
        (u32)VEC_Y(LunaRuntimeInternal.config.windowPos),
        &LunaRuntimeInternal.window
    );

    if (!lunaInitRenderer(LunaRuntimeInternal.config.backend)) {
        r3_log_stdout(ERROR_LOG, "[LunaRuntime] failed to initialize renderer\n");
        return lunaDeinitRuntimeImpl();
    } else r3_log_stdout(SUCCESS_LOG, "[LunaRuntime] initialized renderer\n");

    return result;
}


s32 main(int agrc, char** argv) {
    if (!lunaInitRuntimeImpl(argv[1])) {
        r3_log_stdout(ERROR_LOG, "[LunaRuntime] failed to initialize runtime\n");
        return 1;
    } else r3_log_stdout(SUCCESS_LOG, "[LunaRuntime] initialized runtime\n");

    if (!lunaEventApi->registerCallback(LUNA_EVENT_EXIT, exitCallback)) {
        r3_log_stdout(ERROR_LOG, "[LunaRuntime] failed to register default exit callback\n");
        if (!lunaDeinitRuntimeImpl()) {
            r3_log_stdout(ERROR_LOG, "[LunaRuntime] failed to deinitialize runtime\n");
            return 1;
        } else r3_log_stdout(SUCCESS_LOG, "[LunaRuntime] deinitialized runtime\n");
    }

    if (!lunaEventApi->registerCallback(LUNA_EVENT_RESIZE, resizeCallback)) {
        r3_log_stdout(ERROR_LOG, "[LunaRuntime] failed to register default resize callback\n");
        if (!lunaDeinitRuntimeImpl()) {
            r3_log_stdout(ERROR_LOG, "[LunaRuntime] failed to deinitialize runtime\n");
            return 1;
        } else r3_log_stdout(SUCCESS_LOG, "[LunaRuntime] deinitialized runtime\n");
    }

    LunaRuntimeInternal.userApi.init();
    do {
        lunaPlatformApi->pollEvents();
        lunaPlatformApi->pollInputs();

        if (lunaInputApi->keyIsDown(LUNA_KEY_F12)) {
            lunaEventApi->pushEvent(LUNA_EVENT_EXIT, (LunaEvent){0});
        }

        LunaRuntimeInternal.userApi.update(1.0);
        LunaRuntimeInternal.userApi.render();

        lunaPlatformApi->swapBuffers();
    } while (LunaRuntimeInternal.running);
    LunaRuntimeInternal.userApi.deinit();

    if (!lunaDeinitRuntimeImpl()) {
        r3_log_stdout(ERROR_LOG, "[LunaRuntime] failed to deinitialize runtime\n");
        return 1;
    } else r3_log_stdout(SUCCESS_LOG, "[LunaRuntime] deinitialized runtime\n");

    return 0;
}
