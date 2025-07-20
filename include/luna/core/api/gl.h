#ifndef __LUNA_GLAPI_H__
#define __LUNA_GLAPI_H__

#include <include/luna/core/defines.h>

#include <gl/GL.h>
#include <gl/GLU.h>

// GL equivalent defines
#define GLAPI_STATIC_DRAW                0x88E4
#define GLAPI_ARRAY_BUFFER               0x8892
#define GLAPI_ELEMENT_BUFFER             0x8893

#define GLAPI_VERTEX_SHADER              0x8B31
#define GLAPI_FRAGMENT_SHADER            0x8B30

#define GLAPI_LINK_STATUS                0x8B82
#define GLAPI_COMPILE_STATUS             0x8B81
#define GLAPI_ACTIVE_ATTRIBUTES          0x8B89
#define GLAPI_SHADING_LANGUAGE_VERSION   0x8B8C

#define GLAPI_TEXT_UNIT0                 0x84C0
#define GLAPI_TEXT_UNIT1                 0x84C1
#define GLAPI_TEXT_UNIT2                 0x84C2
#define GLAPI_TEXT_UNIT3                 0x84C3
#define GLAPI_ACTIVE_TEXTURE             0x84E0
#define GLAPI_MAX_TEXTURE_UNITS          0x84E2

#define GLAPI_LINE                       0x1B01
#define GLAPI_FRONT_AND_BACK             0x0408

#define GLAPI_FRAMEBUFFER 0x8D40
#define GLAPI_COLOR_ATTACHMENT0 0x8CE0
#define GLAPI_COLOR_ATTACHMENT1 0x8CE1
#define GLAPI_COLOR_ATTACHMENT2 0x8CE2
#define GLAPI_COLOR_ATTACHMENT3 0x8CE3
#define GLAPI_COLOR_ATTACHMENT4 0x8CE4
#define GLAPI_COLOR_ATTACHMENT5 0x8CE5
#define GLAPI_COLOR_ATTACHMENT6 0x8CE6
#define GLAPI_COLOR_ATTACHMENT7 0x8CE7
#define GLAPI_COLOR_ATTACHMENT8 0x8CE8
#define GLAPI_COLOR_ATTACHMENT9 0x8CE9
#define GLAPI_COLOR_ATTACHMENT10 0x8CEA
#define GLAPI_COLOR_ATTACHMENT11 0x8CEB
#define GLAPI_COLOR_ATTACHMENT12 0x8CEC
#define GLAPI_COLOR_ATTACHMENT13 0x8CED
#define GLAPI_COLOR_ATTACHMENT14 0x8CEE
#define GLAPI_COLOR_ATTACHMENT15 0x8CEF
#define GLAPI_DEPTH_ATTACHMENT 0x8D00
#define GLAPI_STENCIL_ATTACHMENT 0x8D20
#define GLAPI_DEPTH_STENCIL_ATTACHMENT 0x821A

#define GLAPI_RENDERBUFFER 0x8D41
#define GLAPI_DEPTH24_STENCIL8 0x88F0

typedef struct LunaOpenGL {
    // BUFFER FUNCTIONS
    LUNA_FNPTR(none, glGenBuffers, s32 n, u32* buffers);
    LUNA_FNPTR(none, glBindBuffer, u32 target, u32 buffer);
    LUNA_FNPTR(none, glBufferData, u32 target, s32 size, const ptr data, u32 usage);
    LUNA_FNPTR(ptr, glMapBuffer, u32 target, u32 access);
    LUNA_FNPTR(none, glUnmapBuffer, u32 target);
    LUNA_FNPTR(none, glBufferSubData, u32 target, s32 offset, s32 size, const ptr data);
    LUNA_FNPTR(none, glGetBufferParameteriv, u32 target, u32 pname, s32* params);
    LUNA_FNPTR(none, glDeleteBuffers, s32 n, const u32* buffers);

    // VAO FUNCTIONS
    LUNA_FNPTR(none, glGenVertexArrays, s32 n, u32* arrays);
    LUNA_FNPTR(none, glBindVertexArray, u32 array);
    LUNA_FNPTR(none, glDeleteVertexArrays, s32 n, const u32* arrays);
    LUNA_FNPTR(none, glEnableVertexAttribArray, u32 index);
    LUNA_FNPTR(none, glDisableVertexAttribArray, u32 index);
    LUNA_FNPTR(none, glVertexAttribPointer, u32 index, s32 size, u32 type, u32 normalized, s32 stride, const ptr pointer);

    // SHADER FUNCTIONS
    LUNA_FNPTR(u32, glCreateShader, u32 type);
    LUNA_FNPTR(none, glShaderSource, u32 shader, s32 count, const cstr* string, const s32* length);
    LUNA_FNPTR(none, glCompileShader, u32 shader);
    LUNA_FNPTR(none, glDeleteShader, u32 shader);
    LUNA_FNPTR(none, glGetShaderiv, u32 shader, u32 pname, s32* params);
    LUNA_FNPTR(none, glGetShaderInfoLog, u32 shader, s32 maxLength, s32* length, str infoLog);

    LUNA_FNPTR(u32, glCreateProgram, void);
    LUNA_FNPTR(none, glAttachShader, u32 program, u32 shader);
    LUNA_FNPTR(none, glDetachShader, u32 program, u32 shader);
    LUNA_FNPTR(none, glLinkProgram, u32 program);
    LUNA_FNPTR(none, glUseProgram, u32 program);
    LUNA_FNPTR(none, glDeleteProgram, u32 program);
    LUNA_FNPTR(none, glGetProgramiv, u32 program, u32 pname, s32* params);
    LUNA_FNPTR(none, glGetProgramInfoLog, u32 program, s32 maxLength, s32* length, str infoLog);
    LUNA_FNPTR(s32, glGetUniformLocation, u32 program, cstr name);

    // UNIFORMS
    LUNA_FNPTR(none, glUniform1i, s32 location, s32 v0);
    LUNA_FNPTR(none, glUniform1f, s32 location, f32 v0);
    LUNA_FNPTR(none, glUniform2fv, s32 location, s32 count, const f32* value);
    LUNA_FNPTR(none, glUniform3fv, s32 location, s32 count, const f32* value);
    LUNA_FNPTR(none, glUniform4fv, s32 location, s32 count, const f32* value);
    LUNA_FNPTR(none, glUniformMatrix4fv, s32 location, s32 count, u32 transpose, const f32* value);

    // TEXTURES
    LUNA_FNPTR(none, glGenTextures, s32 n, u32* textures);
    LUNA_FNPTR(none, glBindTexture, u32 target, u32 texture);
    LUNA_FNPTR(none, glTexParameteri, u32 target, u32 pname, s32 param);
    LUNA_FNPTR(none, glTexImage2D, u32 target, s32 level, s32 internalFormat, s32 width, s32 height, s32 border, u32 format, u32 type, const ptr data);
    LUNA_FNPTR(none, glActiveTexture, u32 texture);
    LUNA_FNPTR(none, glDeleteTextures, s32 n, const u32* textures);
    LUNA_FNPTR(none, glGenerateMipmap, u32 target);

    // FRAMEBUFFERS + RENDERBUFFERS
    LUNA_FNPTR(none, glGenFramebuffers, s32 n, u32* ids);
    LUNA_FNPTR(none, glBindFramebuffer, u32 target, u32 framebuffer);
    LUNA_FNPTR(none, glFramebufferTexture2D, u32 target, u32 attachment, u32 textarget, u32 texture, s32 level);
    LUNA_FNPTR(none, glFramebufferRenderbuffer, u32 target, u32 attachment, u32 renderbuffertarget, u32 renderbuffer);
    LUNA_FNPTR(u32, glCheckFramebufferStatus, u32 target);
    LUNA_FNPTR(none, glDeleteFramebuffers, s32 n, const u32* ids);

    LUNA_FNPTR(none, glGenRenderbuffers, s32 n, u32* renderbuffers);
    LUNA_FNPTR(none, glBindRenderbuffer, u32 target, u32 renderbuffer);
    LUNA_FNPTR(none, glRenderbufferStorage, u32 target, u32 internalFormat, s32 width, s32 height);
    LUNA_FNPTR(none, glDeleteRenderbuffers, s32 n, const u32* renderbuffers);

    // DRAWING
    LUNA_FNPTR(none, glDrawArrays, u32 mode, s32 first, s32 count);
    LUNA_FNPTR(none, glDrawElements, u32 mode, s32 count, u32 type, const ptr indices);

    // STATE
    LUNA_FNPTR(none, glEnable, u32 cap);
    LUNA_FNPTR(none, glDisable, u32 cap);
    LUNA_FNPTR(none, glBlendFunc, u32 sfactor, u32 dfactor);
    LUNA_FNPTR(none, glCullFace, u32 mode);
    LUNA_FNPTR(none, glDepthFunc, u32 func);
    LUNA_FNPTR(none, glViewport, s32 x, s32 y, s32 width, s32 height);
    LUNA_FNPTR(none, glPolygonMode, u32 face, u32 mode);
    LUNA_FNPTR(none, glClear, u32 mask);
    LUNA_FNPTR(none, glClearColor, f32 r, f32 g, f32 b, f32 a);
    LUNA_FNPTR(none, glClearDepth, f64 depth);

    // INFO / DEBUG
    LUNA_FNPTR(u32, glGetError, void);
    LUNA_FNPTR(cstr, glGetString, u32 name);
} LunaOpenGL;
extern LunaOpenGL* lunaOpenGL;

LUNA_API u8 lunaInitOpenGL(none);
LUNA_API u8 lunaDeinitOpenGL(none);

#endif  // __LUNA_GLAPI_H__