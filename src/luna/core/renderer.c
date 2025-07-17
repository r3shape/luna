#include <include/luna/core/renderer.h>
#include <include/luna/core/api/glapi.h>
#include <include/r3kt/io/log.h>
#include <include/r3kt/math.h>

static struct LunaRendererInternal {
    LunaGpuPipeline pipelinev[LUNA_GPU_RESOURCE_MAX];
    LunaGpuProgram programv[LUNA_GPU_RESOURCE_MAX];
    LunaGpuBuffer bufferv[LUNA_GPU_RESOURCE_MAX];
    LunaGpuApi gpuApi;
    LunaGpuPhase* phase;
    LunaGpuFrame* frame;
    LunaGpuBackend backend;
    Mat4 projection;
    u32 pipelines;
    u32 programs;
    u32 buffers;
    u32 frames;
} LunaRendererInternal = {0};

// global dispatch table ptr
LunaRenderer* lunaRenderer = NULL;


LunaGpuHandle createFrameImpl(none) {
    return 0;
}

LunaGpuHandle createCallImpl(LunaGpuCall call) {
    return 0;
}

LunaGpuHandle createPhaseImpl(LunaGpuPhaseType type, ptr phase) {
    return 0;
}

LunaGpuHandle createBufferImpl(LunaGpuBufferType type, ptr buffer) {
    return 0;
}

LunaGpuHandle createShaderImpl(str vertexShader, str fragmentShader) {
    return 0;
}

LunaGpuHandle createPipelineImpl(LunaGpuHandle program, u8 uniforms, LunaGpuUniform* uniformv) {
    return 0;
}


none renderImpl(none) {
    r3_log_stdout(INFO_LOG, "[LunaRenderer] render dump!\n");
    return;
}


u8 lunaInitRenderer(LunaGpuBackend backend, LunaRenderer* table, ptr platform_table) {
    if (!table || !platform_table) {
        r3_log_stdout(ERROR_LOG, "[LunaRenderer] invalid table ptr :: lunaInitRenderer()\n");
        return 0;
    }
    
    switch(backend) {
        case LUNA_BACKEND_OPENGL: {
            if (!lunaInitGlApi(&LunaRendererInternal.gpuApi, platform_table)) {
                r3_log_stdout(ERROR_LOG, "[LunaRenderer] failed to initialize gpu backend: OpenGL\n");
            } else r3_log_stdout(SUCCESS_LOG, "[LunaRenderer] initialized gpu backend: OpenGL\n");
        } break;
        case LUNA_BACKEND_VULKAN:  // fall-through
        case LUNA_BACKEND_DIRECTX:  // fall-through
        case LUNA_BACKEND_INVALID:  // fall-through
        default: break;
    }
    
    LunaRendererInternal.backend = backend;
    table->render = renderImpl;
    table->createCall = createCallImpl;
    table->createFrame = createFrameImpl;
    table->createPhase = createPhaseImpl;
    table->createBuffer = createBufferImpl;
    table->createShader = createShaderImpl;
    table->createPipeline = createPipelineImpl;

    // assign global dispatch table ptr
    lunaRenderer = table;

    r3_log_stdout(SUCCESS_LOG, "[LunaRenderer] table initialized\n");
    return 1;
}

u8 lunaDeinitRenderer(LunaRenderer* table) {
    if (!table) {
        r3_log_stdout(ERROR_LOG, "[LunaRenderer] invalid table ptr :: lunaInitRenderer()\n");
        return 0;
    }
    
    switch(LunaRendererInternal.backend) {
        case LUNA_BACKEND_OPENGL: {
            if (!lunaDeinitGlApi(&LunaRendererInternal.gpuApi)) {
                r3_log_stdout(ERROR_LOG, "[LunaRenderer] failed to deinitialize gpu backend: OpenGL\n");
            } else r3_log_stdout(SUCCESS_LOG, "[LunaRenderer] deinitialized gpu backend: OpenGL\n");
        } break;
        case LUNA_BACKEND_VULKAN:  // fall-through
        case LUNA_BACKEND_DIRECTX:  // fall-through
        case LUNA_BACKEND_INVALID:  // fall-through
        default: break;
    }

    // null global dispatch table ptr
    lunaRenderer = NULL;

    table->render = NULL;
    table->createCall = NULL;
    table->createFrame = NULL;
    table->createPhase = NULL;
    table->createBuffer = NULL;
    table->createShader = NULL;
    table->createPipeline = NULL;
    LunaRendererInternal.backend = LUNA_BACKEND_INVALID;

    r3_log_stdout(SUCCESS_LOG, "[LunaRenderer] table deinitialized\n");
    return 1;
}
