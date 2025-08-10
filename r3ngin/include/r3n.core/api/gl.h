#ifndef __R3N_GL_H__
#define __R3N_GL_H__

#include <r3ngin/include/r3n.core/defines.h>

#include <gl/GL.h>
#include <gl/GLU.h>

// GL equivalent defines
#define GL_STATIC_DRAW                0x88E4
#define GL_ARRAY_BUFFER               0x8892
#define GL_ELEMENT_BUFFER             0x8893

#define GL_VERTEX_SHADER              0x8B31
#define GL_FRAGMENT_SHADER            0x8B30

#define GL_LINK_STATUS                0x8B82
#define GL_COMPILE_STATUS             0x8B81
#define GL_ACTIVE_ATTRIBUTES          0x8B89
#define GL_SHADING_LANGUAGE_VERSION   0x8B8C

#define GL_TEXT_UNIT0                 0x84C0
#define GL_TEXT_UNIT1                 0x84C1
#define GL_TEXT_UNIT2                 0x84C2
#define GL_TEXT_UNIT3                 0x84C3
#define GL_ACTIVE_TEXTURE             0x84E0
#define GL_MAX_TEXTURE_UNITS          0x84E2

#define GL_LINE                       0x1B01
#define GL_FRONT_AND_BACK             0x0408

#define GL_FRAMEBUFFER 0x8D40
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_COLOR_ATTACHMENT1 0x8CE1
#define GL_COLOR_ATTACHMENT2 0x8CE2
#define GL_COLOR_ATTACHMENT3 0x8CE3
#define GL_COLOR_ATTACHMENT4 0x8CE4
#define GL_COLOR_ATTACHMENT5 0x8CE5
#define GL_COLOR_ATTACHMENT6 0x8CE6
#define GL_COLOR_ATTACHMENT7 0x8CE7
#define GL_COLOR_ATTACHMENT8 0x8CE8
#define GL_COLOR_ATTACHMENT9 0x8CE9
#define GL_COLOR_ATTACHMENT10 0x8CEA
#define GL_COLOR_ATTACHMENT11 0x8CEB
#define GL_COLOR_ATTACHMENT12 0x8CEC
#define GL_COLOR_ATTACHMENT13 0x8CED
#define GL_COLOR_ATTACHMENT14 0x8CEE
#define GL_COLOR_ATTACHMENT15 0x8CEF
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_STENCIL_ATTACHMENT 0x8D20
#define GL_DEPTH_STENCIL_ATTACHMENT 0x821A

#define GL_RENDERBUFFER 0x8D41
#define GL_DEPTH24_STENCIL8 0x88F0

typedef struct R3N_GL {
    // BUFFER FUNCTIONS
    R3N_FNPTR(none, gen_buffers, s32 n, u32* buffers);
    R3N_FNPTR(none, bind_buffer, u32 target, u32 buffer);
    R3N_FNPTR(none, buffer_data, u32 target, s32 size, const ptr data, u32 usage);
    R3N_FNPTR(ptr, map_buffer, u32 target, u32 access);
    R3N_FNPTR(none, unmap_buffer, u32 target);
    R3N_FNPTR(none, buffer_sub_data, u32 target, s32 offset, s32 size, const ptr data);
    R3N_FNPTR(none, get_buffer_parameteriv, u32 target, u32 pname, s32* params);
    R3N_FNPTR(none, delete_buffers, s32 n, const u32* buffers);

    // VAO FUNCTIONS
    R3N_FNPTR(none, gen_vertex_arrays, s32 n, u32* arrays);
    R3N_FNPTR(none, bind_vertex_array, u32 array);
    R3N_FNPTR(none, delete_vertex_arrays, s32 n, const u32* arrays);
    R3N_FNPTR(none, enable_vertex_attrib_array, u32 index);
    R3N_FNPTR(none, disable_vertex_attrib_array, u32 index);
    R3N_FNPTR(none, vertex_attrib_pointer, u32 index, s32 size, u32 type, u32 normalized, s32 stride, const ptr pointer);

    // SHADER FUNCTIONS
    R3N_FNPTR(u32, create_shader, u32 type);
    R3N_FNPTR(none, shader_source, u32 shader, s32 count, const cstr* string, const s32* length);
    R3N_FNPTR(none, compile_shader, u32 shader);
    R3N_FNPTR(none, delete_shader, u32 shader);
    R3N_FNPTR(none, get_shaderiv, u32 shader, u32 pname, s32* params);
    R3N_FNPTR(none, get_shader_info_log, u32 shader, s32 maxLength, s32* length, str infoLog);

    R3N_FNPTR(u32, create_program, void);
    R3N_FNPTR(none, attach_shader, u32 program, u32 shader);
    R3N_FNPTR(none, detach_shader, u32 program, u32 shader);
    R3N_FNPTR(none, link_program, u32 program);
    R3N_FNPTR(none, use_program, u32 program);
    R3N_FNPTR(none, delete_program, u32 program);
    R3N_FNPTR(none, get_programiv, u32 program, u32 pname, s32* params);
    R3N_FNPTR(none, get_program_info_log, u32 program, s32 maxLength, s32* length, str infoLog);
    R3N_FNPTR(s32, get_uniform_location, u32 program, cstr name);

    // UNIFORMS
    R3N_FNPTR(none, uniform1i, s32 location, s32 v0);
    R3N_FNPTR(none, uniform1f, s32 location, f32 v0);
    R3N_FNPTR(none, uniform2fv, s32 location, s32 count, const f32* value);
    R3N_FNPTR(none, uniform3fv, s32 location, s32 count, const f32* value);
    R3N_FNPTR(none, uniform4fv, s32 location, s32 count, const f32* value);
    R3N_FNPTR(none, uniform_matrix4fv, s32 location, s32 count, u32 transpose, const f32* value);

    // TEXTURES
    R3N_FNPTR(none, gen_textures, s32 n, u32* textures);
    R3N_FNPTR(none, bind_texture, u32 target, u32 texture);
    R3N_FNPTR(none, tex_parameteri, u32 target, u32 pname, s32 param);
    R3N_FNPTR(none, tex_image2d, u32 target, s32 level, s32 internalFormat, s32 width, s32 height, s32 border, u32 format, u32 type, const ptr data);
    R3N_FNPTR(none, active_texture, u32 texture);
    R3N_FNPTR(none, delete_textures, s32 n, const u32* textures);
    R3N_FNPTR(none, generate_mipmap, u32 target);

    // FRAMEBUFFERS + RENDERBUFFERS
    R3N_FNPTR(none, gen_framebuffers, s32 n, u32* ids);
    R3N_FNPTR(none, bind_framebuffer, u32 target, u32 framebuffer);
    R3N_FNPTR(none, framebuffer_texture2d, u32 target, u32 attachment, u32 textarget, u32 texture, s32 level);
    R3N_FNPTR(none, framebuffer_renderbuffer, u32 target, u32 attachment, u32 renderbuffertarget, u32 renderbuffer);
    R3N_FNPTR(u32, check_framebuffer_status, u32 target);
    R3N_FNPTR(none, delete_framebuffers, s32 n, const u32* ids);

    R3N_FNPTR(none, gen_renderbuffers, s32 n, u32* renderbuffers);
    R3N_FNPTR(none, bind_renderbuffer, u32 target, u32 renderbuffer);
    R3N_FNPTR(none, renderbuffer_storage, u32 target, u32 internalFormat, s32 width, s32 height);
    R3N_FNPTR(none, delete_renderbuffers, s32 n, const u32* renderbuffers);

    // DRAWING
    R3N_FNPTR(none, draw_arrays, u32 mode, s32 first, s32 count);
    R3N_FNPTR(none, draw_elements, u32 mode, s32 count, u32 type, const ptr indices);

    // STATE
    R3N_FNPTR(none, enable, u32 cap);
    R3N_FNPTR(none, disable, u32 cap);
    R3N_FNPTR(none, blend_func, u32 sfactor, u32 dfactor);
    R3N_FNPTR(none, cull_face, u32 mode);
    R3N_FNPTR(none, depth_func, u32 func);
    R3N_FNPTR(none, viewport, s32 x, s32 y, s32 width, s32 height);
    R3N_FNPTR(none, polygon_mode, u32 face, u32 mode);
    R3N_FNPTR(none, clear, u32 mask);
    R3N_FNPTR(none, clear_color, f32 r, f32 g, f32 b, f32 a);
    R3N_FNPTR(none, clear_depth, f64 depth);

    // INFO / DEBUG
    R3N_FNPTR(u32, get_error, void);
    R3N_FNPTR(cstr, get_string, u32 name);
} R3N_GL;
extern R3N_GL* R3N_GL_Ptr;

R3N_API u8 r3n_init_gl(none);
R3N_API u8 r3n_deinit_gl(none);

#endif  // __R3N_GL_H__