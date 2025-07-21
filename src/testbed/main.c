#include <include/luna/runtime/luna.h>
#include <include/r3kt/io/log.h>

LunaGpuHandle opaque_phase_handle;
LunaGpuHandle pipeline_handle;
LunaGpuHandle program_handle;

Vec3 u_color = (Vec3){ .data = {0.0, 1.0, 1.0} };

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
        .uniforms = 1,
        .uniformv = {
            { .type=LUNA_UNIFORM_MAT4, .mat4=IDENTITY(), .name="u_model" }
        },
        .program = program_handle
    });

    return;
}

void update(f32 dt) {
    if (lunaInputApi->keyIsDown(LUNA_KEY_R)) u_color.data[0] += 0.1;
    if (lunaInputApi->keyIsDown(LUNA_KEY_G)) u_color.data[1] += 0.1;
    if (lunaInputApi->keyIsDown(LUNA_KEY_B)) u_color.data[2] += 0.1;

    u_color.data[0] -= 0.01;
    u_color.data[1] -= 0.01;
    u_color.data[2] -= 0.01;

    u_color.data[0] = CLAMP(u_color.data[0], 0.0, 1.0);
    u_color.data[1] = CLAMP(u_color.data[1], 0.0, 1.0);
    u_color.data[2] = CLAMP(u_color.data[2], 0.0, 1.0);
    return;
}

void render(void) {
    lunaRenderApi->createFrame();

    opaque_phase_handle = lunaRenderApi->createPhase((LunaGpuPhase){
        .type = LUNA_PHASE_OPAQUE,
        .opaque.clear_color = (Vec4){ .data = {22, 21, 27, 255} }
    });

    lunaRenderApi->createCall((LunaGpuCall){
        .phase = opaque_phase_handle,
        .pipeline = pipeline_handle,
        .element_buffer = 0,
        .vertex_buffer = tri_handle,
        .uniforms = 1, .uniformv = {
            {.type = LUNA_UNIFORM_VEC3, .name = "u_color", .vec3 = u_color}
        }
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
