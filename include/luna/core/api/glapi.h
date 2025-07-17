#ifndef __LUNA_GLAPI_H__
#define __LUNA_GLAPI_H__

#define _LUNA_INTERNAL_
#include <include/luna/core/defines.h>

#include <gl/GL.h>
#include <gl/GLU.h>

#define LUNA_GLFNPTR(sym) __##sym sym = NULL

// BUFFER FUNCTIONS
typedef LUNA_FNPTR(none, __GLGenBuffers, s32 n, u32* buffers);
typedef LUNA_FNPTR(none, __GLBindBuffer, u32 target, u32 buffer);
typedef LUNA_FNPTR(none, __GLBufferData, u32 target, s32 size, const ptr data, u32 usage);
typedef LUNA_FNPTR(ptr, __GLMapBuffer, u32 target, u32 access);
typedef LUNA_FNPTR(none, __GLUnmapBuffer, u32 target);
typedef LUNA_FNPTR(none, __GLBufferSubData, u32 target, s32 offset, s32 size, const ptr data);
typedef LUNA_FNPTR(none, __GLGetBufferParameteriv, u32 target, u32 pname, s32* params);
typedef LUNA_FNPTR(none, __GLDeleteBuffers, s32 n, const u32* buffers);

// VAO FUNCTIONS
typedef LUNA_FNPTR(none, __GLGenVertexArrays, s32 n, u32* arrays);
typedef LUNA_FNPTR(none, __GLBindVertexArray, u32 array);
typedef LUNA_FNPTR(none, __GLDeleteVertexArrays, s32 n, const u32* arrays);
typedef LUNA_FNPTR(none, __GLEnableVertexAttribArray, u32 index);
typedef LUNA_FNPTR(none, __GLDisableVertexAttribArray, u32 index);
typedef LUNA_FNPTR(none, __GLVertexAttribPointer, u32 index, s32 size, u32 type, u32 normalized, s32 stride, const ptr pointer);

// SHADER FUNCTIONS
typedef LUNA_FNPTR(u32, __GLCreateShader, u32 type);
typedef LUNA_FNPTR(none, __GLShaderSource, u32 shader, s32 count, const cstr* string, const s32* length);
typedef LUNA_FNPTR(none, __GLCompileShader, u32 shader);
typedef LUNA_FNPTR(none, __GLDeleteShader, u32 shader);
typedef LUNA_FNPTR(none, __GLGetShaderiv, u32 shader, u32 pname, s32* params);
typedef LUNA_FNPTR(none, __GLGetShaderInfoLog, u32 shader, s32 maxLength, s32* length, str infoLog);

typedef LUNA_FNPTR(u32, __GLCreateProgram, void);
typedef LUNA_FNPTR(none, __GLAttachShader, u32 program, u32 shader);
typedef LUNA_FNPTR(none, __GLDetachShader, u32 program, u32 shader);
typedef LUNA_FNPTR(none, __GLLinkProgram, u32 program);
typedef LUNA_FNPTR(none, __GLUseProgram, u32 program);
typedef LUNA_FNPTR(none, __GLDeleteProgram, u32 program);
typedef LUNA_FNPTR(none, __GLGetProgramiv, u32 program, u32 pname, s32* params);
typedef LUNA_FNPTR(none, __GLGetProgramInfoLog, u32 program, s32 maxLength, s32* length, str infoLog);
typedef LUNA_FNPTR(s32, __GLGetUniformLocation, u32 program, cstr name);

// UNIFORMS
typedef LUNA_FNPTR(none, __GLUniform1i, s32 location, s32 v0);
typedef LUNA_FNPTR(none, __GLUniform1f, s32 location, f32 v0);
typedef LUNA_FNPTR(none, __GLUniform2fv, s32 location, s32 count, const f32* value);
typedef LUNA_FNPTR(none, __GLUniform3fv, s32 location, s32 count, const f32* value);
typedef LUNA_FNPTR(none, __GLUniform4fv, s32 location, s32 count, const f32* value);
typedef LUNA_FNPTR(none, __GLUniformMatrix4fv, s32 location, s32 count, u32 transpose, const f32* value);

// TEXTURES
typedef LUNA_FNPTR(none, __GLGenTextures, s32 n, u32* textures);
typedef LUNA_FNPTR(none, __GLBindTexture, u32 target, u32 texture);
typedef LUNA_FNPTR(none, __GLTexParameteri, u32 target, u32 pname, s32 param);
typedef LUNA_FNPTR(none, __GLTexImage2D, u32 target, s32 level, s32 internalFormat, s32 width, s32 height, s32 border, u32 format, u32 type, const ptr data);
typedef LUNA_FNPTR(none, __GLActiveTexture, u32 texture);
typedef LUNA_FNPTR(none, __GLDeleteTextures, s32 n, const u32* textures);
typedef LUNA_FNPTR(none, __GLGenerateMipmap, u32 target);

// FRAMEBUFFERS + RENDERBUFFERS
typedef LUNA_FNPTR(none, __GLGenFramebuffers, s32 n, u32* ids);
typedef LUNA_FNPTR(none, __GLBindFramebuffer, u32 target, u32 framebuffer);
typedef LUNA_FNPTR(none, __GLFramebufferTexture2D, u32 target, u32 attachment, u32 textarget, u32 texture, s32 level);
typedef LUNA_FNPTR(none, __GLFramebufferRenderbuffer, u32 target, u32 attachment, u32 renderbuffertarget, u32 renderbuffer);
typedef LUNA_FNPTR(u32, __GLCheckFramebufferStatus, u32 target);
typedef LUNA_FNPTR(none, __GLDeleteFramebuffers, s32 n, const u32* ids);

typedef LUNA_FNPTR(none, __GLGenRenderbuffers, s32 n, u32* renderbuffers);
typedef LUNA_FNPTR(none, __GLBindRenderbuffer, u32 target, u32 renderbuffer);
typedef LUNA_FNPTR(none, __GLRenderbufferStorage, u32 target, u32 internalFormat, s32 width, s32 height);
typedef LUNA_FNPTR(none, __GLDeleteRenderbuffers, s32 n, const u32* renderbuffers);

// DRAWING
typedef LUNA_FNPTR(none, __GLDrawArrays, u32 mode, s32 first, s32 count);
typedef LUNA_FNPTR(none, __GLDrawElements, u32 mode, s32 count, u32 type, const ptr indices);

// STATE
typedef LUNA_FNPTR(none, __GLEnable, u32 cap);
typedef LUNA_FNPTR(none, __GLDisable, u32 cap);
typedef LUNA_FNPTR(none, __GLBlendFunc, u32 sfactor, u32 dfactor);
typedef LUNA_FNPTR(none, __GLCullFace, u32 mode);
typedef LUNA_FNPTR(none, __GLDepthFunc, u32 func);
typedef LUNA_FNPTR(none, __GLViewport, s32 x, s32 y, s32 width, s32 height);
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