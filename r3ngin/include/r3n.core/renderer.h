#ifndef __R3N_RENDERER_H__
#define __R3N_RENDERER_H__

#define _R3KIT_INCLUDE_
#include <r3ngin/include/r3n.core/api/gpu.h>
#include <r3ngin/include/r3n.core/defines.h>

typedef struct R3N_Renderer {
    R3N_FNPTR(none, render, none);
    R3N_FNPTR(R3N_GPU_Handle, create_frame, none);
    R3N_FNPTR(R3N_GPU_Handle, create_call, R3N_GPU_Call call);
    
    R3N_FNPTR(R3N_GPU_Handle, create_phase, R3N_GPU_Phase phase);
    R3N_FNPTR(none, destroy_phase, R3N_GPU_Handle phase);

    
    R3N_FNPTR(R3N_GPU_Handle, create_buffer, R3N_GPU_Buffer buffer);
    R3N_FNPTR(none, destroy_buffer, R3N_GPU_Handle buffer);

    
    R3N_FNPTR(R3N_GPU_Handle, create_program, str vertexShader, str fragmentShader);
    R3N_FNPTR(none, destroy_program, R3N_GPU_Handle program);

    
    R3N_FNPTR(R3N_GPU_Handle, create_pipeline, R3N_GPU_Pipeline pipeline);
    R3N_FNPTR(none, destroy_pipeline, R3N_GPU_Handle pipeline);
} R3N_Renderer;
R3N_API R3N_Renderer* R3N_Renderer_Ptr;

R3N_API u8 r3n_init_renderer(R3N_GPU_Backend backend);
R3N_API u8 r3n_deinit_renderer(none);

#endif // __R3N_RENDERER_H__