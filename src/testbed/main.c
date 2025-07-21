#include <include/luna/runtime/luna.h>
#include <include/r3kt/io/log.h>

LunaGpuHandle opaque_phase_handle;
LunaGpuHandle pipeline_handle;
LunaGpuHandle program_handle;

LunaGpuHandle tri_handle;
f32 tri_verts[] = {
    -0.5, -0.5, 0.0,
     0.0,  0.5, 0.0,
     0.5, -0.5, 0.0
};

void init(void) {
    tri_handle = lunaRenderApi->createBuffer((LunaGpuBuffer) {
        .type = LUNA_BUFFER_VERTEX,
        .vertex.vertices = 3,
        .vertex.vertexv = tri_verts,
        .vertex.size = sizeof(tri_verts),
        .vertex.attribs = LUNA_VERTEX_LOCATION_ATTRIBUTE,
    });

    program_handle = lunaRenderApi->createProgram(".external/.data/assets/shaders/default/shader.vert", ".external/.data/assets/shaders/default/shader.frag");

    pipeline_handle = lunaRenderApi->createPipeline((LunaGpuPipeline){
        .binds = 0,
        .uniforms = 0,
        .program = program_handle
    });

    return;
}

void update(f32 dt) {
    // r3_log_stdout(INFO_LOG, "[User] LunaRuntimeApi update call!\n");
    return;
}

void render(void) {
    lunaRenderApi->createFrame();

    opaque_phase_handle = lunaRenderApi->createPhase((LunaGpuPhase){
        .type = LUNA_PHASE_OPAQUE,
        .uniforms = 0,
        .opaque.clear_color = (Vec4){ .data = {22, 21, 27, 255} }
    });

    lunaRenderApi->createCall((LunaGpuCall){
        .uniforms = 0,
        .phase = opaque_phase_handle,
        .pipeline = pipeline_handle,
        .element_buffer = 0,
        .vertex_buffer = tri_handle
    });
    
    lunaRenderApi->render();
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
