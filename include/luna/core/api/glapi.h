#ifndef __LUNA_GLAPI_H__
#define __LUNA_GLAPI_H__

#include <include/SSDK/SSDK.h>
#include <include/luna/core/defines.h>

#include <gl/GL.h>
#include <gl/GLU.h>

// TODO: devise a lightweight -- preferrably "no struct" -- GL function loader

#define LUNA_GLFNPTR(sym) __##sym sym = NULL

// BUFFER FUNCTIONS
typedef LUNA_FNPTR(none, __GLGenBuffers, i32 n, u32* buffers);
typedef LUNA_FNPTR(none, __GLBindBuffer, u32 target, u32 buffer);
typedef LUNA_FNPTR(none, __GLBufferData, u32 target, i32 size, const ptr data, u32 usage);
typedef LUNA_FNPTR(ptr, __GLMapBuffer, u32 target, u32 access);
typedef LUNA_FNPTR(none, __GLUnmapBuffer, u32 target);
typedef LUNA_FNPTR(none, __GLBufferSubData, u32 target, i32 offset, i32 size, const ptr data);
typedef LUNA_FNPTR(none, __GLGetBufferParameteriv, u32 target, u32 pname, i32* params);
typedef LUNA_FNPTR(none, __GLDeleteBuffers, i32 n, const u32* buffers);

// VAO FUNCTIONS
typedef LUNA_FNPTR(none, __GLGenVertexArrays, i32 n, u32* arrays);
typedef LUNA_FNPTR(none, __GLBindVertexArray, u32 array);
typedef LUNA_FNPTR(none, __GLDeleteVertexArrays, i32 n, const u32* arrays);
typedef LUNA_FNPTR(none, __GLEnableVertexAttribArray, u32 index);
typedef LUNA_FNPTR(none, __GLDisableVertexAttribArray, u32 index);
typedef LUNA_FNPTR(none, __GLVertexAttribPointer, u32 index, i32 size, u32 type, u32 normalized, i32 stride, const ptr pointer);

// SHADER FUNCTIONS
typedef LUNA_FNPTR(u32, __GLCreateShader, u32 type);
typedef LUNA_FNPTR(none, __GLShaderSource, u32 shader, i32 count, const cstr* string, const i32* length);
typedef LUNA_FNPTR(none, __GLCompileShader, u32 shader);
typedef LUNA_FNPTR(none, __GLDeleteShader, u32 shader);
typedef LUNA_FNPTR(none, __GLGetShaderiv, u32 shader, u32 pname, i32* params);
typedef LUNA_FNPTR(none, __GLGetShaderInfoLog, u32 shader, i32 maxLength, i32* length, str infoLog);

typedef LUNA_FNPTR(u32, __GLCreateProgram, void);
typedef LUNA_FNPTR(none, __GLAttachShader, u32 program, u32 shader);
typedef LUNA_FNPTR(none, __GLDetachShader, u32 program, u32 shader);
typedef LUNA_FNPTR(none, __GLLinkProgram, u32 program);
typedef LUNA_FNPTR(none, __GLUseProgram, u32 program);
typedef LUNA_FNPTR(none, __GLDeleteProgram, u32 program);
typedef LUNA_FNPTR(none, __GLGetProgramiv, u32 program, u32 pname, i32* params);
typedef LUNA_FNPTR(none, __GLGetProgramInfoLog, u32 program, i32 maxLength, i32* length, str infoLog);
typedef LUNA_FNPTR(i32, __GLGetUniformLocation, u32 program, cstr name);

// UNIFORMS
typedef LUNA_FNPTR(none, __GLUniform1i, i32 location, i32 v0);
typedef LUNA_FNPTR(none, __GLUniform1f, i32 location, f32 v0);
typedef LUNA_FNPTR(none, __GLUniform2fv, i32 location, i32 count, const f32* value);
typedef LUNA_FNPTR(none, __GLUniform3fv, i32 location, i32 count, const f32* value);
typedef LUNA_FNPTR(none, __GLUniform4fv, i32 location, i32 count, const f32* value);
typedef LUNA_FNPTR(none, __GLUniformMatrix4fv, i32 location, i32 count, u32 transpose, const f32* value);

// TEXTURES
typedef LUNA_FNPTR(none, __GLGenTextures, i32 n, u32* textures);
typedef LUNA_FNPTR(none, __GLBindTexture, u32 target, u32 texture);
typedef LUNA_FNPTR(none, __GLTexParameteri, u32 target, u32 pname, i32 param);
typedef LUNA_FNPTR(none, __GLTexImage2D, u32 target, i32 level, i32 internalFormat, i32 width, i32 height, i32 border, u32 format, u32 type, const ptr data);
typedef LUNA_FNPTR(none, __GLActiveTexture, u32 texture);
typedef LUNA_FNPTR(none, __GLDeleteTextures, i32 n, const u32* textures);
typedef LUNA_FNPTR(none, __GLGenerateMipmap, u32 target);

// FRAMEBUFFERS + RENDERBUFFERS
typedef LUNA_FNPTR(none, __GLGenFramebuffers, i32 n, u32* ids);
typedef LUNA_FNPTR(none, __GLBindFramebuffer, u32 target, u32 framebuffer);
typedef LUNA_FNPTR(none, __GLFramebufferTexture2D, u32 target, u32 attachment, u32 textarget, u32 texture, i32 level);
typedef LUNA_FNPTR(none, __GLFramebufferRenderbuffer, u32 target, u32 attachment, u32 renderbuffertarget, u32 renderbuffer);
typedef LUNA_FNPTR(u32, __GLCheckFramebufferStatus, u32 target);
typedef LUNA_FNPTR(none, __GLDeleteFramebuffers, i32 n, const u32* ids);

typedef LUNA_FNPTR(none, __GLGenRenderbuffers, i32 n, u32* renderbuffers);
typedef LUNA_FNPTR(none, __GLBindRenderbuffer, u32 target, u32 renderbuffer);
typedef LUNA_FNPTR(none, __GLRenderbufferStorage, u32 target, u32 internalFormat, i32 width, i32 height);
typedef LUNA_FNPTR(none, __GLDeleteRenderbuffers, i32 n, const u32* renderbuffers);

// DRAWING
typedef LUNA_FNPTR(none, __GLDrawArrays, u32 mode, i32 first, i32 count);
typedef LUNA_FNPTR(none, __GLDrawElements, u32 mode, i32 count, u32 type, const ptr indices);

// STATE
typedef LUNA_FNPTR(none, __GLEnable, u32 cap);
typedef LUNA_FNPTR(none, __GLDisable, u32 cap);
typedef LUNA_FNPTR(none, __GLBlendFunc, u32 sfactor, u32 dfactor);
typedef LUNA_FNPTR(none, __GLCullFace, u32 mode);
typedef LUNA_FNPTR(none, __GLDepthFunc, u32 func);
typedef LUNA_FNPTR(none, __GLViewport, i32 x, i32 y, i32 width, i32 height);
typedef LUNA_FNPTR(none, __GLPolygonMode, u32 face, u32 mode);
typedef LUNA_FNPTR(none, __GLClear, u32 mask);
typedef LUNA_FNPTR(none, __GLClearColor, f32 r, f32 g, f32 b, f32 a);
typedef LUNA_FNPTR(none, __GLClearDepth, f64 depth);

// INFO / DEBUG
typedef LUNA_FNPTR(u32, __GLGetError, void);
typedef LUNA_FNPTR(cstr, __GLGetString, u32 name);

LUNA_API byte lunaInitGlApi(LunaGpuApi* table, ptr platform_table);
LUNA_API byte lunaDeinitGlApi(LunaGpuApi* table);

#endif  // __LUNA_GLAPI_H__