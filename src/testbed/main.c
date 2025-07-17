#include <include/luna/runtime/luna.h>
#include <include/r3kt/io/log.h>

void init(void) {
    r3_log_stdout(INFO_LOG, "[User] LunaRuntimeApi init call!\n");
    return;
}

void update(f32 dt) {
    r3_log_stdout(INFO_LOG, "[User] LunaRuntimeApi update call!\n");
    return;
}

void render(void) {
    r3_log_stdout(INFO_LOG, "[User] LunaRuntimeApi render call!\n");
    return;
}

void deinit(void) {
    r3_log_stdout(INFO_LOG, "[User] LunaRuntimeApi deinit call!\n");
    return;
}

void lunaExport(LunaRuntimeApi* api) {
    api->configure((LunaRuntimeConfig){
        .title = "Luna Testbed",
        .windowPos = (Vec2){ .data = {100, 100} },
        .windowSize = (Vec2){ .data = {1280, 720} },
        .backend = LUNA_BACKEND_OPENGL
    });
    api->init = init;
    api->update = update;
    api->render = render;
    api->deinit = deinit;
}
