#include <r3ngin/include/r3n.runtime/r3ngin.h>
#include <r3ngin/include/r3n.core/platform.h>
#include <r3kit/include/io/log.h>

static struct R3N_Runtime_Internal {
    u8 running;
    R3N_Window window;
    R3N_Runtime user_api;
    R3N_Library user_library;
    R3N_Runtime_Config config;
} R3N_Runtime_Internal = {0};


// default runtime callbacks
u8 exit_callback(R3N_Event_Code code, R3N_Event data) {
    if (code == R3N_EVENT_EXIT) {
        r3_log_stdout(WARN_LOG, "[R3N_Runtime] Exit Event\n");
        R3N_Runtime_Internal.running = 0;
        return 1;
    }; return 0;
}

u8 resize_callback(R3N_Event_Code code, R3N_Event data) {
    if (code == R3N_EVENT_RESIZE) {
        r3_log_stdout(WARN_LOG, "[R3N_Runtime] Resize Event\n");
        return 1;
    }; return 0;
}


none r3n_configure_runtime_impl(R3N_Runtime_Config config) {
    R3N_Runtime_Internal.config.title = (!config.title) ? R3N_ENGINE_STRING : config.title;

    R3N_Runtime_Internal.config.backend = (
        config.backend == R3N_BACKEND_INVALID
    ) ? R3N_BACKEND_OPENGL : config.backend;

    R3N_Runtime_Internal.config.windowPos = (
        VEC_X(config.windowPos) < 0.0 || VEC_Y(config.windowPos) < 0.0
    ) ? (Vec2){ .data = {0.0, 0.0} } : config.windowPos;

        R3N_Runtime_Internal.config.windowSize = (
        VEC_X(config.windowSize) <= 0.0 || VEC_Y(config.windowSize) <= 0.0
    ) ? (Vec2){ .data = {1280, 720} } : config.windowSize;

    R3N_Runtime_Internal.running = 1;
}

u8 r3n_deinit_runtime_impl(none) {
    u8 result = 1;

    if (!R3N_Platform_Ptr->unload_library(&R3N_Runtime_Internal.user_library)) {
        r3_log_stdout(ERROR_LOG, "[R3N_Runtime] failed to unload user library\n");
    }

    if (!r3n_deinit_renderer()) {
        r3_log_stdout(ERROR_LOG, "[R3N_Runtime] failed to deinitialize renderer\n");
        result = 0;
    }

    if (!r3n_deinit_platform()) {
        r3_log_stdout(ERROR_LOG, "[R3N_Runtime] failed to deinitialize platform\n");
        result = 0;
    }

    if (!r3n_deinit_inputs()) {
        r3_log_stdout(ERROR_LOG, "[R3N_Runtime] failed to deinitialize inputs\n");
        result = 0;
    }

    if (!r3n_deinit_events() ){
        r3_log_stdout(ERROR_LOG, "[R3N_Runtime] failed to deinitialize events\n");
        result = 0;
    }

    R3N_Runtime_Internal.user_api.init = NULL;
    R3N_Runtime_Internal.user_api.update = NULL;
    R3N_Runtime_Internal.user_api.render = NULL;
    R3N_Runtime_Internal.user_api.deinit = NULL;
    R3N_Runtime_Internal.user_api.configure = NULL;

    R3N_Runtime_Internal.config.title = NULL;
    R3N_Runtime_Internal.config.windowPos = (Vec2){ .data = {0.0, 0.0} };
    R3N_Runtime_Internal.config.windowSize = (Vec2){ .data = {0, 0} };
    R3N_Runtime_Internal.config.backend = R3N_BACKEND_INVALID;

    r3_log_stdout(SUCCESS_LOG, "[R3N_Runtime] deinitialized runtime\n");

    return result;
}

u8 r3n_init_runtime_impl(str library) {
    u8 result = 1;

    if (!r3n_init_events()) {
        r3_log_stdout(ERROR_LOG, "[R3N_Runtime] failed to initialize events\n");
        return r3n_deinit_runtime_impl();
    } else r3_log_stdout(SUCCESS_LOG, "[R3N_Runtime] initialized events\n");

    if (!r3n_init_inputs()) {
        r3_log_stdout(ERROR_LOG, "[R3N_Runtime] failed to initialize inputs\n");
        return r3n_deinit_runtime_impl();
    } else r3_log_stdout(SUCCESS_LOG, "[R3N_Runtime] initialized inputs\n");

    if (!r3n_init_platform()) {
        r3_log_stdout(ERROR_LOG, "[R3N_Runtime] failed to initialize platform\n");
        return r3n_deinit_runtime_impl();
    } else r3_log_stdout(SUCCESS_LOG, "[R3N_Runtime] initialized platform\n");

    if (!R3N_Platform_Ptr->load_library(R3N_USER_PATH_DEFAULT, library, &R3N_Runtime_Internal.user_library)) {
        r3_log_stdout(ERROR_LOG, "[R3N_Runtime] failed to load user library\n");
        return r3n_deinit_runtime_impl();
    } else r3_log_stdout(SUCCESS_LOG, "[R3N_Runtime] loaded user library\n");

    ptr exportFunc;
    if (!R3N_Platform_Ptr->load_library_symbol("r3n_export", &exportFunc, &R3N_Runtime_Internal.user_library) || exportFunc == NULL) {
        r3_log_stdout(ERROR_LOG, "[R3N_Runtime] failed to load r3n_export symbol\n");
        if (!R3N_Platform_Ptr->unload_library(&R3N_Runtime_Internal.user_library)) {
            r3_log_stdout(ERROR_LOG, "[R3N_Runtime] failed to unload user library\n");
        } return r3n_deinit_runtime_impl();
    } else r3_log_stdout(SUCCESS_LOG, "[R3N_Runtime] loaded r3n_export symbol\n");

    R3N_Runtime_Internal.user_api.configure = r3n_configure_runtime_impl;

    ((R3N_Runtime_Export_Func)exportFunc)(&R3N_Runtime_Internal.user_api);
    if (!R3N_Runtime_Internal.user_api.init || !R3N_Runtime_Internal.user_api.update ||
        !R3N_Runtime_Internal.user_api.render || !R3N_Runtime_Internal.user_api.deinit) {
        r3_log_stdout(ERROR_LOG, "[R3N_Runtime] incomplete R3N_Runtime\n");
        if (!R3N_Platform_Ptr->unload_library(&R3N_Runtime_Internal.user_library)) {
            r3_log_stdout(ERROR_LOG, "[R3N_Runtime] failed to unload user library\n");
        } return r3n_deinit_runtime_impl();
    } else r3_log_stdout(SUCCESS_LOG, "[R3N_Runtime] R3N_Runtime exported\n");

    R3N_Platform_Ptr->create_window(
        R3N_Runtime_Internal.config.title,
        (u32)VEC_X(R3N_Runtime_Internal.config.windowSize),
        (u32)VEC_Y(R3N_Runtime_Internal.config.windowSize),
        (u32)VEC_X(R3N_Runtime_Internal.config.windowPos),
        (u32)VEC_Y(R3N_Runtime_Internal.config.windowPos),
        &R3N_Runtime_Internal.window
    );

    if (!r3n_init_renderer(R3N_Runtime_Internal.config.backend)) {
        r3_log_stdout(ERROR_LOG, "[R3N_Runtime] failed to initialize renderer\n");
        return r3n_deinit_runtime_impl();
    } else r3_log_stdout(SUCCESS_LOG, "[R3N_Runtime] initialized renderer\n");

    return result;
}


s32 main(int agrc, char** argv) {
    if (!r3n_init_runtime_impl(argv[1])) {
        r3_log_stdout(ERROR_LOG, "[R3N_Runtime] failed to initialize runtime\n");
        return 1;
    } else r3_log_stdout(SUCCESS_LOG, "[R3N_Runtime] initialized runtime\n");

    if (!R3N_Events_Ptr->register_callback(R3N_EVENT_EXIT, exit_callback)) {
        r3_log_stdout(ERROR_LOG, "[R3N_Runtime] failed to register default exit callback\n");
        if (!r3n_deinit_runtime_impl()) {
            r3_log_stdout(ERROR_LOG, "[R3N_Runtime] failed to deinitialize runtime\n");
            return 1;
        } else r3_log_stdout(SUCCESS_LOG, "[R3N_Runtime] deinitialized runtime\n");
    }

    if (!R3N_Events_Ptr->register_callback(R3N_EVENT_RESIZE, resize_callback)) {
        r3_log_stdout(ERROR_LOG, "[R3N_Runtime] failed to register default resize callback\n");
        if (!r3n_deinit_runtime_impl()) {
            r3_log_stdout(ERROR_LOG, "[R3N_Runtime] failed to deinitialize runtime\n");
            return 1;
        } else r3_log_stdout(SUCCESS_LOG, "[R3N_Runtime] deinitialized runtime\n");
    }

    R3N_Runtime_Internal.user_api.init();
    do {
        R3N_Platform_Ptr->poll_events();
        R3N_Platform_Ptr->poll_inputs();

        if (R3N_Inputs_Ptr->key_is_down(R3N_KEY_F12)) {
            R3N_Events_Ptr->push_event(R3N_EVENT_EXIT, (R3N_Event){0});
        }

        R3N_Runtime_Internal.user_api.update(1.0);
        R3N_Runtime_Internal.user_api.render();

        R3N_Platform_Ptr->swap_buffers();
    } while (R3N_Runtime_Internal.running);
    R3N_Runtime_Internal.user_api.deinit();

    if (!r3n_deinit_runtime_impl()) {
        r3_log_stdout(ERROR_LOG, "[R3N_Runtime] failed to deinitialize runtime\n");
        return 1;
    } else r3_log_stdout(SUCCESS_LOG, "[R3N_Runtime] deinitialized runtime\n");

    return 0;
}
