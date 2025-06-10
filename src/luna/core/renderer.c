#include <include/luna/core/renderer.h>
#include <include/luna/core/api/glapi.h>

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
    saneLog->log(SANE_LOG_DUMP, "[LunaRenderer] render dump!");
    return;
}


byte lunaInitRenderer(LunaGpuBackend backend, LunaRenderer* table, ptr platform_table) {
    if (!table || !platform_table) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRenderer] invalid table ptr :: lunaInitRenderer()");
        return SSDK_FALSE;
    }
    
    switch(backend) {
        case LUNA_BACKEND_OPENGL: {
            if (!lunaInitGlApi(&LunaRendererInternal.gpuApi, platform_table)) {
                saneLog->log(SANE_LOG_ERROR, "[LunaRenderer] failed to initialize gpu backend: OpenGL");
            } else saneLog->log(SANE_LOG_SUCCESS, "[LunaRenderer] initialized gpu backend: OpenGL");
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

    saneLog->log(SANE_LOG_SUCCESS, "[LunaRenderer] table initialized");
    return SSDK_TRUE;
}

byte lunaDeinitRenderer(LunaRenderer* table) {
    if (!table) {
        saneLog->log(SANE_LOG_ERROR, "[LunaRenderer] invalid table ptr :: lunaInitRenderer()");
        return SSDK_FALSE;
    }
    
    switch(LunaRendererInternal.backend) {
        case LUNA_BACKEND_OPENGL: {
            if (!lunaDeinitGlApi(&LunaRendererInternal.gpuApi)) {
                saneLog->log(SANE_LOG_ERROR, "[LunaRenderer] failed to deinitialize gpu backend: OpenGL");
            } else saneLog->log(SANE_LOG_SUCCESS, "[LunaRenderer] deinitialized gpu backend: OpenGL");
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

    saneLog->log(SANE_LOG_SUCCESS, "[LunaRenderer] table deinitialized");
    return SSDK_TRUE;
}
