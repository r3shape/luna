#include <include/luna/runtime/luna.h>

void init(void) {
    saneLog->log(SANE_LOG_DUMP, "[User] LunaRuntimeApi init call!");
    return;
}

void update(f32 dt) {
    saneLog->log(SANE_LOG_DUMP, "[User] LunaRuntimeApi update call!");
    return;
}

void render(void) {
    saneLog->log(SANE_LOG_DUMP, "[User] LunaRuntimeApi render call!");
    return;
}

void deinit(void) {
    saneLog->log(SANE_LOG_DUMP, "[User] LunaRuntimeApi deinit call!");
    return;
}

void lunaExport(LunaRuntimeApi* api) {
    api->configure((LunaRuntimeConfig){
        .title = "Luna Testbed",
        .windowPos = {100, 100},
        .windowSize = {1280, 720},
        .backend = LUNA_BACKEND_OPENGL
    });
    api->init = init;
    api->update = update;
    api->render = render;
    api->deinit = deinit;
}
