#ifndef __LUNA_RENDERER_H__
#define __LUNA_RENDERER_H__

#include <include/luna/core/api/gpuapi.h>
#include <include/SSDK/SSDK.h>

typedef struct LunaRenderer {
    LUNA_FNPTR(none, render, none);
    LUNA_FNPTR(LunaGpuHandle, createFrame, none);
    LUNA_FNPTR(LunaGpuHandle, createCall, LunaGpuCall call);
    LUNA_FNPTR(LunaGpuHandle, createPhase, LunaGpuPhaseType type, ptr phase);       // ptr cast to specific phase structure internally
    LUNA_FNPTR(LunaGpuHandle, createBuffer, LunaGpuBufferType type, ptr buffer);    // ptr cast to specific buffer structure internally
    LUNA_FNPTR(LunaGpuHandle, createShader, str vertexShader, str fragmentShader);
    LUNA_FNPTR(LunaGpuHandle, createPipeline, LunaGpuHandle program, u8 uniforms, LunaGpuUniform* uniformv);
} LunaRenderer;
extern LunaRenderer* lunaRenderer;

LUNA_API byte lunaInitRenderer(LunaGpuBackend backend, LunaRenderer* table);
LUNA_API byte lunaDeinitRenderer(LunaRenderer* table);

#endif // __LUNA_RENDERER_H__