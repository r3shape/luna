#include <r3ngin/include/r3n.runtime/r3ngin.h>
#include <r3kit/include/io/log.h>

R3N_GPU_Handle opaque_phase_handle;
R3N_GPU_Handle pipeline_handle;
R3N_GPU_Handle program_handle;

Vec3 u_color = (Vec3){ .data = {0.0, 1.0, 1.0} };

R3N_GPU_Handle tri_handle;
f32 tri_verts[] = {
    -0.5, -0.5, 0.0,
     0.0,  0.5, 0.0,
     0.5, -0.5, 0.0
};

void init(void) {
    tri_handle = R3N_Renderer_Ptr->create_buffer((R3N_GPU_Buffer) {
        .type = R3N_BUFFER_VERTEX,
        .vertex.vertices = 3,
        .vertex.vertexv = tri_verts,
        .vertex.size = sizeof(tri_verts),
        .vertex.attribs = R3N_VERTEX_LOCATION_ATTRIBUTE,
    });

    program_handle = R3N_Renderer_Ptr->create_program(
        "r3n.data/assets/shaders/default/shader.vert",
        "r3n.data/assets/shaders/default/shader.frag"
    );

    pipeline_handle = R3N_Renderer_Ptr->create_pipeline((R3N_GPU_Pipeline){
        .binds = 0,
        .uniforms = 1,
        .uniformv = {
            { .type=R3N_UNIFORM_MAT4, .mat4=IDENTITY(), .name="u_model" }
        },
        .program = program_handle
    });

    return;
}

void update(f32 dt) {
    if (R3N_Inputs_Ptr->key_is_down(R3N_KEY_R)) u_color.data[0] += 0.1;
    if (R3N_Inputs_Ptr->key_is_down(R3N_KEY_G)) u_color.data[1] += 0.1;
    if (R3N_Inputs_Ptr->key_is_down(R3N_KEY_B)) u_color.data[2] += 0.1;

    u_color.data[0] -= 0.01;
    u_color.data[1] -= 0.01;
    u_color.data[2] -= 0.01;

    u_color.data[0] = CLAMP(u_color.data[0], 0.0, 1.0);
    u_color.data[1] = CLAMP(u_color.data[1], 0.0, 1.0);
    u_color.data[2] = CLAMP(u_color.data[2], 0.0, 1.0);
    return;
}

void render(void) {
    R3N_Renderer_Ptr->create_frame();

    opaque_phase_handle = R3N_Renderer_Ptr->create_phase((R3N_GPU_Phase){
        .type = R3N_PHASE_OPAQUE,
        .opaque.clear_color = (Vec4){ .data = {22, 21, 27, 255} }
    });

    R3N_Renderer_Ptr->create_call((R3N_GPU_Call){
        .phase = opaque_phase_handle,
        .pipeline = pipeline_handle,
        .element_buffer = 0,
        .vertex_buffer = tri_handle,
        .uniforms = 1, .uniformv = {
            {.type = R3N_UNIFORM_VEC3, .name = "u_color", .vec3 = u_color}
        }
        });
    
    R3N_Renderer_Ptr->render();
    return;
}

void deinit(void) {
    r3_log_stdout(INFO_LOG, "[User] R3N_Runtime deinit call!\n");
    return;
}

void r3n_export(R3N_Runtime* api) {
    api->configure((R3N_Runtime_Config){
        .title = "R3NGIN Testbed",
        .windowPos = (Vec2){ .data = {100, 100} },
        .windowSize = (Vec2){ .data = {1280, 720} },
        .backend = R3N_BACKEND_OPENGL
    });
    api->init = init;
    api->update = update;
    api->render = render;
    api->deinit = deinit;
}
