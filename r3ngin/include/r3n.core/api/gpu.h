#ifndef __R3N_GPUAPI_H__
#define __R3N_GPUAPI_H__

#define _R3KIT_INCLUDE_
#include <r3ngin/include/r3n.core/defines.h>

#define R3N_GPU_CALL_MAX               KiB
#define R3N_GPU_PHASE_MAX              4
#define R3N_GPU_RESOURCE_MAX           KiB
#define R3N_GPU_PROGRAM_BUFFER_SIZE   (2 * KiB)

#define R3N_GPU_CALL_UNIFORM_MAX           8

#define R3N_GPU_PHASE_BUFFER_MAX           8
#define R3N_GPU_PHASE_UNIFORM_MAX          4

#define R3N_GPU_PIPELINE_BUFFER_MAX        8
#define R3N_GPU_PIPELINE_UNIFORM_MAX       4

typedef u32 R3N_GPU_Handle;

typedef enum R3N_GPU_Backend {
    R3N_BACKEND_INVALID,
    R3N_BACKEND_OPENGL,
    R3N_BACKEND_VULKAN,
    R3N_BACKEND_DIRECTX
} R3N_GPU_Backend;

typedef enum R3N_GPU_Buffer_Type {
    R3N_BUFFER_INVALID,
    R3N_BUFFER_TEXTURE,
    R3N_BUFFER_ELEMENT,
    R3N_BUFFER_VERTEX,
    R3N_BUFFER_FRAME,
    R3N_BUFFER_TYPES
} R3N_GPU_Buffer_Type;

typedef enum R3N_GPU_Texture_Type {
    R3N_TEXTURE_INVALID,
    R3N_TEXTURE_2D,
    R3N_TEXTURE_3D
} R3N_GPU_Texture_Type;

typedef enum R3N_GPU_Texture_Format {
    R3N_TEXTURE_FORMAT_INVALID,
    R3N_TEXTURE_FORMAT_RGBA        = 0x1908,
    R3N_TEXTURE_FORMAT_RGB         = 0x1907
} R3N_GPU_Texture_Format;

typedef enum R3N_GPU_Mode {
    R3N_MODE_INVLAID,
    R3N_MODE_TRIANGLE      = 0x0004,
    R3N_MODE_POINT         = 0x0000,
    R3N_MODE_LINE          = 0x0001,
    R3N_MODE_QUAD          = 0x0007,
} R3N_GPU_Mode;

typedef enum R3N_GPU_Vertex_Attribute {
    R3N_VERTEX_LOCATION_ATTRIBUTE  = 1 << 0,
    R3N_VERTEX_TEXTURE_ATTRIBUTE   = 1 << 1,
    R3N_VERTEX_NORMAL_ATTRIBUTE    = 1 << 2,
    R3N_VERTEX_COLOR_ATTRIBUTE     = 1 << 3,
    R3N_VERTEX_ATTRIBUTES          = 4
} R3N_GPU_Vertex_Attribute;

typedef enum R3N_GPU_Phase_Type {
    R3N_PHASE_INVALID,
    R3N_PHASE_SHADOW,
    R3N_PHASE_OPAQUE,
    R3N_PHASE_LIGHT,
    R3N_PHASE_DEPTH,
    R3N_PHASE_TYPES
} R3N_GPU_Phase_Type;

typedef enum R3N_GPU_Uniform_Type {
    R3N_UNIFORM_INVALID,
    R3N_UNIFORM_INT,
    R3N_UNIFORM_FLOAT,
    R3N_UNIFORM_VEC2,
    R3N_UNIFORM_VEC3,
    R3N_UNIFORM_VEC4,
    R3N_UNIFORM_MAT4,
    R3N_UNIFORM_TYPES
} R3N_GPU_Uniform_Type;

typedef struct R3N_GPU_Texture_Buffer {
    str path;
    ptr data;
    u32 slot;
    s32 width;
    s32 height;
    s32 channels;
    R3N_GPU_Handle tbo;
    R3N_GPU_Texture_Type type;
    R3N_GPU_Texture_Format format;
} R3N_GPU_Texture_Buffer;

typedef struct R3N_GPU_Element_Buffer {
    u32* elementv;
    u32 elements;
    u32 size;
    R3N_GPU_Handle ebo;
} R3N_GPU_Element_Buffer;

typedef struct R3N_GPU_Vertex_Buffer {
    f32* vertexv;
    u32 size;
    u32 vertices;
    R3N_GPU_Handle vbo;
    R3N_GPU_Handle vao;
    u8 attribs;
} R3N_GPU_Vertex_Buffer;

typedef struct R3N_GPU_Frame_Buffer {
    R3N_GPU_Handle tbo;
    R3N_GPU_Handle rbo;
    R3N_GPU_Handle fbo;
} R3N_GPU_Frame_Buffer;

typedef struct R3N_GPU_Buffer {
    union {
        R3N_GPU_Frame_Buffer frame;
        R3N_GPU_Vertex_Buffer vertex;
        R3N_GPU_Element_Buffer element;
        R3N_GPU_Texture_Buffer texture;
    };
    R3N_GPU_Handle handle;
    R3N_GPU_Buffer_Type type;
} R3N_GPU_Buffer;

typedef struct R3N_GPU_Uniform {
    union {
        s32 s32;
        f32 f32;
        Vec2 vec2;
        Vec3 vec3;
        Vec4 vec4;
        Mat4 mat4;
    };
    cstr name;
    s32 location;
    R3N_GPU_Uniform_Type type;
} R3N_GPU_Uniform;

typedef struct R3N_GPU_Program {
    str fragment_path;
    str vertex_path;
    Buffer fragment_buffer;
    Buffer vertex_buffer;
    u32 program;
    Array uniformv;
    R3N_GPU_Handle handle;
} R3N_GPU_Program;

typedef struct R3N_GPU_Pipeline {
    R3N_GPU_Uniform uniformv[R3N_GPU_PIPELINE_UNIFORM_MAX];
    R3N_GPU_Handle bindv[R3N_GPU_PIPELINE_BUFFER_MAX];
    R3N_GPU_Handle program;
    R3N_GPU_Handle handle;
    u32 binds;
    u8 uniforms;
} R3N_GPU_Pipeline;

typedef struct R3N_GPU_Depth_Phase {
    f32 clear_depth;
} R3N_GPU_Depth_Phase;

typedef struct R3N_GPU_Opaque_Phase {
    Vec4 clear_color;
} R3N_GPU_Opaque_Phase;

typedef struct R3N_GPU_Light_Phase { u8 mask; } R3N_GPU_Light_Phase;
typedef struct R3N_GPU_Shadow_Phase { u8 mask; } R3N_GPU_Shadow_Phase;

typedef struct R3N_GPU_Phase {
    R3N_GPU_Uniform uniformv[R3N_GPU_PHASE_UNIFORM_MAX];
    R3N_GPU_Handle writev[R3N_GPU_PHASE_BUFFER_MAX];
    R3N_GPU_Handle readv[R3N_GPU_PHASE_BUFFER_MAX];
    union {
        R3N_GPU_Depth_Phase depth;
        R3N_GPU_Opaque_Phase opaque;

        R3N_GPU_Light_Phase light;
        R3N_GPU_Shadow_Phase shadow;
    };
    R3N_GPU_Phase_Type type;
    R3N_GPU_Handle handle;
    u8 uniforms;
    u32 writes;
    u32 reads;
} R3N_GPU_Phase;

typedef struct R3N_GPU_Call {
    R3N_GPU_Uniform uniformv[R3N_GPU_CALL_UNIFORM_MAX];
    R3N_GPU_Handle element_buffer;
    R3N_GPU_Handle vertex_buffer;
    R3N_GPU_Handle pipeline;
    R3N_GPU_Handle phase;
    R3N_GPU_Handle handle;
    u8 uniforms;
} R3N_GPU_Call;

typedef struct R3N_GPU_Frame {
    u8 phases;                  // phase mask, used to track and prevent redundant GpuPhase creation
    u32 calls;
    R3N_GPU_Handle handle;
    R3N_GPU_Handle buffer;
    Arena_Allocator arena;
    R3N_GPU_Call callv[R3N_GPU_CALL_MAX];
    R3N_GPU_Phase phasev[R3N_GPU_PHASE_MAX];
} R3N_GPU_Frame;

typedef struct R3N_GPU {
    R3N_FNPTR(u8, clear_depth_buffer, f32 depth);
    R3N_FNPTR(u8, clear_color_buffer, Vec4 color);

    R3N_FNPTR(u8, create_program, R3N_GPU_Program* program);
    R3N_FNPTR(u8, destroy_program, R3N_GPU_Program* program);
    
    R3N_FNPTR(u8, send_uniform, cstr name, R3N_GPU_Program* program);
    R3N_FNPTR(u8, set_uniform, R3N_GPU_Uniform* uniform, R3N_GPU_Program* program);
    
    R3N_FNPTR(u8, create_vertex_buffer, R3N_GPU_Buffer* buffer);
    R3N_FNPTR(u8, destroy_vertex_buffer, R3N_GPU_Buffer* buffer);
    
    R3N_FNPTR(u8, create_element_buffer, R3N_GPU_Buffer* buffer);
    R3N_FNPTR(u8, destroy_element_buffer, R3N_GPU_Buffer* buffer);
    
    R3N_FNPTR(u8, create_texture_buffer, R3N_GPU_Buffer* buffer);
    R3N_FNPTR(u8, destroy_texture_buffer, R3N_GPU_Buffer* buffer);
   
    R3N_FNPTR(u8, create_frame_buffer, R3N_GPU_Buffer* buffer);
    R3N_FNPTR(u8, destroy_frame_buffer, R3N_GPU_Buffer* buffer);

    R3N_FNPTR(u8, bind_buffer, R3N_GPU_Buffer* buffer);
    R3N_FNPTR(u8, read_buffer, R3N_GPU_Buffer* buffer);
    R3N_FNPTR(u8, write_buffer, R3N_GPU_Buffer* buffer);
    R3N_FNPTR(u8, bind_program, R3N_GPU_Program* program);
} R3N_GPU;
extern R3N_GPU* R3N_GPU_Ptr;

#endif  // __R3N_GPUAPI_H__