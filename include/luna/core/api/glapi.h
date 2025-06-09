#ifndef __LUNA_GLAPI_H__
#define __LUNA_GLAPI_H__

#include <include/SSDK/SSDK.h>
#include <include/luna/core/defines.h>

#include <gl/GL.h>
#include <gl/GLU.h>

// TODO: devise a lightweight -- preferrably "no struct" -- GL function loader

#define LUNA_GLFNPTR(sym) __##sym sym

// BUFFER FUNCTIONS
typedef LUNA_FNPTR(none, __GLGenBuffers, i32 n, u32* buffers);
LUNA_API LUNA_GLFNPTR(GLGenBuffers);

typedef LUNA_FNPTR(none, __GLBindBuffer, u32 target, u32 buffer);
LUNA_API LUNA_GLFNPTR(GLBindBuffer);

typedef LUNA_FNPTR(none, __GLBufferData, u32 target, i32 size, const ptr data, u32 usage);
LUNA_API LUNA_GLFNPTR(GLBufferData);

typedef LUNA_FNPTR(ptr, __GLMapBuffer, u32 target, u32 access);
LUNA_API LUNA_GLFNPTR(GLMapBuffer);

typedef LUNA_FNPTR(none, __GLUnmapBuffer, u32 target);
LUNA_API LUNA_GLFNPTR(GLUnmapBuffer);

typedef LUNA_FNPTR(none, __GLBufferSubData, u32 target, i32 offset, i32 size, const ptr data);
LUNA_API LUNA_GLFNPTR(GLBufferSubData);

typedef LUNA_FNPTR(none, __GLGetBufferParameteriv, u32 target, u32 pname, i32* params);
LUNA_API LUNA_GLFNPTR(GLGetBufferParameteriv);

typedef LUNA_FNPTR(none, __GLDeleteBuffers, i32 n, const u32* buffers);
LUNA_API LUNA_GLFNPTR(GLDeleteBuffers);


// VAO FUNCTIONS
typedef LUNA_FNPTR(none, __GLGenVertexArrays, i32 n, u32* arrays);
LUNA_API LUNA_GLFNPTR(GLGenVertexArrays);

typedef LUNA_FNPTR(none, __GLBindVertexArray, u32 array);
LUNA_API LUNA_GLFNPTR(GLBindVertexArray);

typedef LUNA_FNPTR(none, __GLDeleteVertexArrays, i32 n, const u32* arrays);
LUNA_API LUNA_GLFNPTR(GLDeleteVertexArrays);

typedef LUNA_FNPTR(none, __GLEnableVertexAttribArray, u32 index);
LUNA_API LUNA_GLFNPTR(GLEnableVertexAttribArray);

typedef LUNA_FNPTR(none, __GLDisableVertexAttribArray, u32 index);
LUNA_API LUNA_GLFNPTR(GLDisableVertexAttribArray);

typedef LUNA_FNPTR(none, __GLVertexAttribPointer, u32 index, i32 size, u32 type, u32 normalized, i32 stride, const ptr pointer);
LUNA_API LUNA_GLFNPTR(GLVertexAttribPointer);


// SHADER FUNCTIONS
typedef LUNA_FNPTR(u32, __GLCreateShader, u32 type);
LUNA_API LUNA_GLFNPTR(GLCreateShader);

typedef LUNA_FNPTR(none, __GLShaderSource, u32 shader, i32 count, const cstr* string, const i32* length);
LUNA_API LUNA_GLFNPTR(GLShaderSource);

typedef LUNA_FNPTR(none, __GLCompileShader, u32 shader);
LUNA_API LUNA_GLFNPTR(GLCompileShader);

typedef LUNA_FNPTR(none, __GLDeleteShader, u32 shader);
LUNA_API LUNA_GLFNPTR(GLDeleteShader);

typedef LUNA_FNPTR(none, __GLGetShaderiv, u32 shader, u32 pname, i32* params);
LUNA_API LUNA_GLFNPTR(GLGetShaderiv);

typedef LUNA_FNPTR(none, __GLGetShaderInfoLog, u32 shader, i32 maxLength, i32* length, str infoLog);
LUNA_API LUNA_GLFNPTR(GLGetShaderInfoLog);


typedef LUNA_FNPTR(u32, __GLCreateProgram, void);
LUNA_API LUNA_GLFNPTR(GLCreateProgram);

typedef LUNA_FNPTR(none, __GLAttachShader, u32 program, u32 shader);
LUNA_API LUNA_GLFNPTR(GLAttachShader);

typedef LUNA_FNPTR(none, __GLDetachShader, u32 program, u32 shader);
LUNA_API LUNA_GLFNPTR(GLDetachShader);

typedef LUNA_FNPTR(none, __GLLinkProgram, u32 program);
LUNA_API LUNA_GLFNPTR(GLLinkProgram);

typedef LUNA_FNPTR(none, __GLUseProgram, u32 program);
LUNA_API LUNA_GLFNPTR(GLUseProgram);

typedef LUNA_FNPTR(none, __GLDeleteProgram, u32 program);
LUNA_API LUNA_GLFNPTR(GLDeleteProgram);

typedef LUNA_FNPTR(none, __GLGetProgramiv, u32 program, u32 pname, i32* params);
LUNA_API LUNA_GLFNPTR(GLGetProgramiv);

typedef LUNA_FNPTR(none, __GLGetProgramInfoLog, u32 program, i32 maxLength, i32* length, str infoLog);
LUNA_API LUNA_GLFNPTR(GLGetProgramInfoLog);

typedef LUNA_FNPTR(i32, __GLGetUniformLocation, u32 program, cstr name);
LUNA_API LUNA_GLFNPTR(GLGetUniformLocation);


// UNIFORMS
typedef LUNA_FNPTR(none, __GLUniform1i, i32 location, i32 v0);
LUNA_API LUNA_GLFNPTR(GLUniform1i);

typedef LUNA_FNPTR(none, __GLUniform1f, i32 location, f32 v0);
LUNA_API LUNA_GLFNPTR(GLUniform1f);

typedef LUNA_FNPTR(none, __GLUniform2fv, i32 location, i32 count, const f32* value);
LUNA_API LUNA_GLFNPTR(GLUniform2fv);

typedef LUNA_FNPTR(none, __GLUniform3fv, i32 location, i32 count, const f32* value);
LUNA_API LUNA_GLFNPTR(GLUniform3fv);

typedef LUNA_FNPTR(none, __GLUniform4fv, i32 location, i32 count, const f32* value);
LUNA_API LUNA_GLFNPTR(GLUniform4fv);

typedef LUNA_FNPTR(none, __GLUniformMatrix4fv, i32 location, i32 count, u32 transpose, const f32* value);
LUNA_API LUNA_GLFNPTR(GLUniformMatrix4fv);


// TEXTURES
typedef LUNA_FNPTR(none, __GLGenTextures, i32 n, u32* textures);
LUNA_API LUNA_GLFNPTR(GLGenTextures);

typedef LUNA_FNPTR(none, __GLBindTexture, u32 target, u32 texture);
LUNA_API LUNA_GLFNPTR(GLBindTexture);

typedef LUNA_FNPTR(none, __GLTexParameteri, u32 target, u32 pname, i32 param);
LUNA_API LUNA_GLFNPTR(GLTexParameteri);

typedef LUNA_FNPTR(none, __GLTexImage2D, u32 target, i32 level, i32 internalFormat, i32 width, i32 height, i32 border, u32 format, u32 type, const ptr data);
LUNA_API LUNA_GLFNPTR(GLTexImage2D);

typedef LUNA_FNPTR(none, __GLActiveTexture, u32 texture);
LUNA_API LUNA_GLFNPTR(GLActiveTexture);

typedef LUNA_FNPTR(none, __GLDeleteTextures, i32 n, const u32* textures);
LUNA_API LUNA_GLFNPTR(GLDeleteTextures);

typedef LUNA_FNPTR(none, __GLGenerateMipmap, u32 target);
LUNA_API LUNA_GLFNPTR(GLGenerateMipmap);


// FRAMEBUFFERS + RENDERBUFFERS
typedef LUNA_FNPTR(none, __GLGenFramebuffers, i32 n, u32* ids);
LUNA_API LUNA_GLFNPTR(GLGenFramebuffers);

typedef LUNA_FNPTR(none, __GLBindFramebuffer, u32 target, u32 framebuffer);
LUNA_API LUNA_GLFNPTR(GLBindFramebuffer);

typedef LUNA_FNPTR(none, __GLFramebufferTexture2D, u32 target, u32 attachment, u32 textarget, u32 texture, i32 level);
LUNA_API LUNA_GLFNPTR(GLFramebufferTexture2D);

typedef LUNA_FNPTR(none, __GLFramebufferRenderbuffer, u32 target, u32 attachment, u32 renderbuffertarget, u32 renderbuffer);
LUNA_API LUNA_GLFNPTR(GLFramebufferRenderbuffer);

typedef LUNA_FNPTR(u32, __GLCheckFramebufferStatus, u32 target);
LUNA_API LUNA_GLFNPTR(GLCheckFramebufferStatus);

typedef LUNA_FNPTR(none, __GLDeleteFramebuffers, i32 n, const u32* ids);
LUNA_API LUNA_GLFNPTR(GLDeleteFramebuffers);


typedef LUNA_FNPTR(none, __GLGenRenderbuffers, i32 n, u32* renderbuffers);
LUNA_API LUNA_GLFNPTR(GLGenRenderbuffers);

typedef LUNA_FNPTR(none, __GLBindRenderbuffer, u32 target, u32 renderbuffer);
LUNA_API LUNA_GLFNPTR(GLBindRenderbuffer);

typedef LUNA_FNPTR(none, __GLRenderbufferStorage, u32 target, u32 internalFormat, i32 width, i32 height);
LUNA_API LUNA_GLFNPTR(GLRenderbufferStorage);

typedef LUNA_FNPTR(none, __GLDeleteRenderbuffers, i32 n, const u32* renderbuffers);
LUNA_API LUNA_GLFNPTR(GLDeleteRenderbuffers);


// DRAWING
typedef LUNA_FNPTR(none, __GLDrawArrays, u32 mode, i32 first, i32 count);
LUNA_API LUNA_GLFNPTR(GLDrawArrays);

typedef LUNA_FNPTR(none, __GLDrawElements, u32 mode, i32 count, u32 type, const ptr indices);
LUNA_API LUNA_GLFNPTR(GLDrawElements);


// STATE
typedef LUNA_FNPTR(none, __GLEnable, u32 cap);
LUNA_API LUNA_GLFNPTR(GLEnable);

typedef LUNA_FNPTR(none, __GLDisable, u32 cap);
LUNA_API LUNA_GLFNPTR(GLDisable);

typedef LUNA_FNPTR(none, __GLBlendFunc, u32 sfactor, u32 dfactor);
LUNA_API LUNA_GLFNPTR(GLBlendFunc);

typedef LUNA_FNPTR(none, __GLCullFace, u32 mode);
LUNA_API LUNA_GLFNPTR(GLCullFace);

typedef LUNA_FNPTR(none, __GLDepthFunc, u32 func);
LUNA_API LUNA_GLFNPTR(GLDepthFunc);

typedef LUNA_FNPTR(none, __GLViewport, i32 x, i32 y, i32 width, i32 height);
LUNA_API LUNA_GLFNPTR(GLViewport);

typedef LUNA_FNPTR(none, __GLPolygonMode, u32 face, u32 mode);
LUNA_API LUNA_GLFNPTR(GLPolygonMode);

typedef LUNA_FNPTR(none, __GLClear, u32 mask);
LUNA_API LUNA_GLFNPTR(GLClear);

typedef LUNA_FNPTR(none, __GLClearColor, f32 r, f32 g, f32 b, f32 a);
LUNA_API LUNA_GLFNPTR(GLClearColor);

typedef LUNA_FNPTR(none, __GLClearDepth, f64 depth);
LUNA_API LUNA_GLFNPTR(GLClearDepth);


// INFO / DEBUG
typedef LUNA_FNPTR(u32, __GLGetError, void);
LUNA_API LUNA_GLFNPTR(GLGetError);

typedef LUNA_FNPTR(cstr, __GLGetString, u32 name);
LUNA_API LUNA_GLFNPTR(GLGetString);


LUNA_API byte lunaInitGlApi(LunaGpuApi* table);
LUNA_API byte lunaDeinitGlApi(LunaGpuApi* table);

#endif  // __LUNA_GLAPI_H__