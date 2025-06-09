#ifndef __LUNA_GPUAPI_H__
#define __LUNA_GPUAPI_H__

#include <include/SSDK/SSDK.h>
#include <include/luna/core/defines.h>

#define LUNA_GPU_CALL_MAX       1024
#define LUNA_GPU_PHASE_MAX      4
#define LUNA_GPU_RESOURCE_MAX   1024

typedef u32 LunaGpuHandle;

typedef enum LunaGpuBackend {
    LUNA_BACKEND_INVALID,
    LUNA_BACKEND_OPENGL,
    LUNA_BACKEND_VULKAN,
    LUNA_BACKEND_DIRECTX
} LunaGpuBackend;

typedef enum LunaGpuBufferType {
    LUNA_BUFFER_INVALID,
    LUNA_BUFFER_TEXTURE,
    LUNA_BUFFER_ELEMENT,
    LUNA_BUFFER_VERTEX,
    LUNA_BUFFER_FRAME
} LunaGpuBufferType;

typedef enum LunaGpuTextureType {
    LUNA_TEXTURE_INVALID,
    LUNA_TEXTURE_2D,
    LUNA_TEXTURE_3D
} LunaGpuTextureType;

typedef enum LunaGpuTextureFormat {
    LUNA_TEXTURE_FORMAT_INVALID,
    LUNA_TEXTURE_FORMAT_RGBA        = 0x1908,
    LUNA_TEXTURE_FORMAT_RGB         = 0x1907
} LunaGpuTextureFormat;

typedef enum LunaGpuMode {
    LUNA_MODE_INVLAID,
    LUNA_MODE_TRIANGLE      = 0x0004,
    LUNA_MODE_POINT         = 0x0000,
    LUNA_MODE_LINE          = 0x0001,
    LUNA_MODE_QUAD          = 0x0007,
} LunaGpuMode;

typedef enum LunaGpuVertexAttribute {
    LUNA_VERTEX_LOCATION_ATTRIBUTE  = 1 << 0,
    LUNA_VERTEX_TEXTURE_ATTRIBUTE   = 1 << 1,
    LUNA_VERTEX_NORMAL_ATTRIBUTE    = 1 << 2,
    LUNA_VERTEX_COLOR_ATTRIBUTE     = 1 << 3,
    LUNA_VERTEX_ATTRIBUTES          = 4
} LunaGpuVertexAttribute;

typedef enum LunaGpuPhaseType {
    LUNA_PHASE_INVALID,
    LUNA_PHASE_SHADOW,
    LUNA_PHASE_OPAQUE,
    LUNA_PHASE_LIGHT,
    LUNA_PHASE_DEPTH
} LunaGpuPhaseType;

typedef enum LunaGpuUniformType {
    LUNA_UNIFORM_INVALID,
    LUNA_UNIFORM_INT,
    LUNA_UNIFORM_FLOAT,
    LUNA_UNIFORM_VEC2,
    LUNA_UNIFORM_VEC3,
    LUNA_UNIFORM_VEC4,
    LUNA_UNIFORM_MAT4,
    LUNA_UNIFORM_TYPES
} LunaGpuUniformType;

typedef struct LunaTextureBuffer {
    str path;
    ptr data;
    u32 slot;
    i32 width;
    i32 height;
    i32 channels;
    LunaGpuHandle tbo;
    LunaGpuTextureType type;
    LunaGpuTextureFormat format;
} LunaTextureBuffer;

typedef struct LunaElementBuffer {
    u32* elementv;
    u32 elements;
    LunaGpuHandle ebo;
} LunaElementBuffer;

typedef struct LunaVertexBuffer {
    f32* vertexv;
    u32 vertices;
    LunaGpuHandle vao;
    u8 format;
} LunaVertexBuffer;

typedef struct LunaFrameBuffer {
    LunaGpuHandle tbo;
    LunaGpuHandle rbo;
    LunaGpuHandle fbo;
} LunaFrameBuffer;

typedef struct LunaGpuBuffer {
    union {
        LunaFrameBuffer frame;
        LunaVertexBuffer vertex;
        LunaElementBuffer element;
        LunaTextureBuffer texture;
    };
    LunaGpuBufferType type;
} LunaGpuBuffer;

typedef struct LunaGpuUniform {
    union {
        i32 i32;
        f32 f32;
        Vec2 vec2;
        Vec3 vec3;
        Vec4 vec4;
        Mat4 mat4;
    };
    str name;
    u32 location;
    LunaGpuUniformType type;
} LunaGpuUniform;

typedef struct LunaGpuProgram {
    str vertex;
    str fragment;
    HashArray uniforms;
    LunaGpuHandle handle;
} LunaGpuProgram;

typedef struct LunaGpuPipeline {
    LunaGpuUniform uniformv[4];
    LunaGpuHandle bindv[8];
    LunaGpuHandle program;
    LunaGpuHandle handle;
    u32 binds;
    u8 uniforms;
} LunaGpuPipeline;

typedef struct LunaGpuDepthPhase {
    f32 clear_depth;
} LunaGpuDepthPhase;

typedef struct LunaGpuOpaquePhase {
    Vec3 clear_color;
} LunaGpuOpaquePhase;

typedef struct LunaGpuLightPhase { u8 mask; } LunaGpuLightPhase;
typedef struct LunaGpuShadowPhase { u8 mask; } LunaGpuShadowPhase;

typedef struct LunaGpuPhase {
    LunaGpuUniform uniformv[4];
    // LunaGpuHandle writev[8];
    // LunaGpuHandle readv[8];
    union {
        LunaGpuDepthPhase depth;
        LunaGpuOpaquePhase opaque;

        LunaGpuLightPhase light;
        LunaGpuShadowPhase shadow;
    };
    LunaGpuPhaseType type;
    LunaGpuHandle handle;
    u8 uniforms;
    // u32 writes;
    // u32 reads;
} LunaGpuPhase;

typedef struct LunaGpuCall {
    LunaGpuUniform uniformv[6];
    LunaGpuHandle elementBuffer;
    LunaGpuHandle vertexBuffer;
    LunaGpuHandle pipeline;
    LunaGpuHandle phase;
    u8 uniforms;
} LunaGpuCall;

typedef struct LunaGpuFrame {
    u8 phases;                  // phase mask, used to track and prevent redundant GpuPhase creation
    u32 calls;
    Allocator arena;
    LunaGpuHandle handle;
    LunaGpuBuffer buffer;
    LunaGpuCall callv[LUNA_GPU_CALL_MAX];
    LunaGpuPhase phasev[LUNA_GPU_PHASE_MAX];
} LunaGpuFrame;

typedef struct LunaGpuApi {
    LUNA_FNPTR(none, clearDepthBuffer, f32 depth);
    LUNA_FNPTR(none, clearColorBuffer, Vec3 color);

    LUNA_FNPTR(none, createProgram, LunaGpuProgram* program);
    LUNA_FNPTR(none, destroyProgram, LunaGpuProgram* program);
    
    LUNA_FNPTR(none, sendUniform, str name, LunaGpuProgram* program);
    LUNA_FNPTR(none, setUniform, LunaGpuUniform* uniform, LunaGpuProgram* program);
    
    LUNA_FNPTR(none, createVertexBuffer, LunaGpuBuffer* buffer);
    LUNA_FNPTR(none, destroyVertexBuffer, LunaGpuBuffer* buffer);
    
    LUNA_FNPTR(none, createElementBuffer, LunaGpuBuffer* buffer);
    LUNA_FNPTR(none, destroyElementBuffer, LunaGpuBuffer* buffer);
    
    LUNA_FNPTR(none, createTextureBuffer, LunaGpuBuffer* buffer);
    LUNA_FNPTR(none, destroyTextureBuffer, LunaGpuBuffer* buffer);
   
    LUNA_FNPTR(none, createFrameBuffer, LunaGpuBuffer* buffer);
    LUNA_FNPTR(none, destroyFrameBuffer, LunaGpuBuffer* buffer);

    LUNA_FNPTR(none, bindBuffer, LunaGpuBuffer* buffer);
    LUNA_FNPTR(none, readBuffer, LunaGpuBuffer* buffer);
    LUNA_FNPTR(none, writeBuffer, LunaGpuBuffer* buffer);
    LUNA_FNPTR(none, bindProgram, LunaGpuProgram* program);
} LunaGpuApi;

#endif  // __LUNA_GPUAPI_H__