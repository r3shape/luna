#ifndef __LUNA_RENDERER_H__
#define __LUNA_RENDERER_H__

#define _LUNA_INTERNAL_
#include <include/luna/core/api/gpu.h>
#include <include/luna/core/defines.h>

typedef struct LunaRenderApi {
    LUNA_FNPTR(none, render, none);
    LUNA_FNPTR(LunaGpuHandle, createFrame, none);
    LUNA_FNPTR(LunaGpuHandle, createCall, LunaGpuCall call);
    
    LUNA_FNPTR(LunaGpuHandle, createPhase, LunaGpuPhase phase);
    LUNA_FNPTR(none, destroyPhase, LunaGpuHandle phase);

    
    LUNA_FNPTR(LunaGpuHandle, createBuffer, LunaGpuBuffer buffer);
    LUNA_FNPTR(none, destroyBuffer, LunaGpuHandle buffer);

    
    LUNA_FNPTR(LunaGpuHandle, createProgram, str vertexShader, str fragmentShader);
    LUNA_FNPTR(none, destroyProgram, LunaGpuHandle program);

    
    LUNA_FNPTR(LunaGpuHandle, createPipeline, LunaGpuPipeline pipeline);
    LUNA_FNPTR(none, destroyPipeline, LunaGpuHandle pipeline);
} LunaRenderApi;
LUNA_API LunaRenderApi* lunaRenderApi;

LUNA_API u8 lunaInitRenderer(LunaGpuBackend backend);
LUNA_API u8 lunaDeinitRenderer(none);

#endif // __LUNA_RENDERER_H__