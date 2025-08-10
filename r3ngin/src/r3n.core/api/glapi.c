#include <r3ngin/include/r3n.core/platform.h>
#include <r3ngin/include/r3n.core/renderer.h>
#include <r3ngin/include/r3n.core/api/gl.h>
#include <r3ngin/include/r3n.core/api/gpu.h>

#define STB_IMAGE_IMPLEMENTATION
#include <STB/include/stb_image.h>

// dispatch table ptrs
R3N_GPU* R3N_GPU_Ptr = NULL;
R3N_GL* R3N_GL_Ptr = NULL;

u8 gl_clear_depth_buffer_impl(f32 depth) {
    R3N_GL_Ptr->clear_depth(depth);
    R3N_GL_Ptr->clear(GL_DEPTH_BUFFER_BIT);
    return 1;
}

u8 gl_clear_color_buffer_impl(Vec4 color) {
    R3N_GL_Ptr->clear_color(color.data[0] / 255, color.data[1] / 255, color.data[2] / 255, color.data[3] / 255);
    R3N_GL_Ptr->clear(GL_COLOR_BUFFER_BIT);
    return 1;
}


u8 gl_create_program_impl(R3N_GPU_Program* program) {
    if (!program || !program->vertex_buffer.data || !program->fragment_buffer.data) {
        r3_log_stdout(ERROR_LOG, "[R3N_GL] invalid program passed to CreateProgram()\n");
        return 0;
    }

    s32 link_status = 0;
    s32 compile_status = 0;
    
    s32 vertex_shader = R3N_GL_Ptr->create_shader(GL_VERTEX_SHADER);
    R3N_GL_Ptr->shader_source(vertex_shader, 1, &(cstr){program->vertex_buffer.data}, NULL);
    R3N_GL_Ptr->compile_shader(vertex_shader);
    
    R3N_GL_Ptr->get_shaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
    if (!compile_status) {
        r3_log_stdout(INFO_LOG, "[R3N_GL] failed to compile vertex shader\n");
        R3N_GL_Ptr->delete_shader(vertex_shader);
        return 0;
    } else { r3_log_stdout(INFO_LOG, "[R3N_GL] compiled vertex shader\n"); }
    
    s32 fragment_shader = R3N_GL_Ptr->create_shader(GL_FRAGMENT_SHADER);
    R3N_GL_Ptr->shader_source(fragment_shader, 1, &(cstr){program->fragment_buffer.data}, NULL);
    R3N_GL_Ptr->compile_shader(fragment_shader);
    
    R3N_GL_Ptr->get_shaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
    if (!compile_status) {
        r3_log_stdout(INFO_LOG, "[R3N_GL] failed to compile fragment shader\n");
        R3N_GL_Ptr->delete_shader(vertex_shader);
        R3N_GL_Ptr->delete_shader(fragment_shader);
        return 0;
    } else { r3_log_stdout(INFO_LOG, "[R3N_GL] compiled fragment shader\n"); }
        
    program->program = R3N_GL_Ptr->create_program();
    R3N_GL_Ptr->attach_shader(program->program, vertex_shader);
    R3N_GL_Ptr->attach_shader(program->program, fragment_shader);
    R3N_GL_Ptr->link_program(program->program);

    R3N_GL_Ptr->get_programiv(program->program, GL_LINK_STATUS, &link_status);
    if (!link_status) {
        r3_log_stdout(INFO_LOG, "[R3N_GL] failed to link program\n");
        R3N_GL_Ptr->delete_shader(vertex_shader);
        R3N_GL_Ptr->delete_shader(fragment_shader);
        return 0;
    }

    R3N_GL_Ptr->delete_shader(vertex_shader);
    R3N_GL_Ptr->delete_shader(fragment_shader);
    return 1;
}

u8 gl_destroy_program_impl(R3N_GPU_Program* program) {
    if (!program || !program->program || !program->uniformv.data) {
        r3_log_stdout(ERROR_LOG, "[R3N_GL] invalid program passed to DestroyProgram()\n");
        return 0;
    }
    r3_arr_hashed_dealloc(&program->uniformv);
    R3N_GL_Ptr->delete_program(program->program);
    return 1;
}


u8 gl_send_uniform_impl(cstr name, R3N_GPU_Program* program) {
    if (!program || !program->program || !program->uniformv.data) {
        r3_log_stdout(ERROR_LOG, "[R3N_GL] invalid program passed to SendUniform()\n");
        return 0;
    } 
    
    R3N_GPU_Uniform uniform = {0};
    if (!r3_arr_hashed_read(name, &uniform, &program->uniformv)){
        r3_log_stdoutf(ERROR_LOG, "[R3N_GL]  failed to read from uniform array: (uniform)%s\n", name);
        return 0;
    }
        
    if (strcmp(uniform.name, name) != 0) {
        r3_log_stdoutf(ERROR_LOG, "[R3N_GL]  uniform not found: (uniform)%s\n", name);
        return 0;
    }

    switch (uniform.type) {
        case R3N_UNIFORM_INVALID: break;
        case R3N_UNIFORM_TYPES: break;
        case R3N_UNIFORM_INT: {
            R3N_GL_Ptr->uniform1i(uniform.location, uniform.s32);
            r3_log_stdoutf(SUCCESS_LOG, "[R3N_GL] sent int uniform: (uniform)%s (location)%d\n", name, uniform.location);
            return 1;
        }
        case R3N_UNIFORM_FLOAT: {
            R3N_GL_Ptr->uniform1f(uniform.location, uniform.f32);
            r3_log_stdoutf(SUCCESS_LOG, "[R3N_GL] sent float uniform: (uniform)%s (location)%d\n", name, uniform.location);
            return 1;
        }
        case R3N_UNIFORM_VEC2: {
            R3N_GL_Ptr->uniform2fv(uniform.location, 1, ((f32*)&uniform.vec2.data[0]));
            r3_log_stdoutf(SUCCESS_LOG, "[R3N_GL] sent vec2 uniform: (uniform)%s (location)%d\n", name, uniform.location);
            return 1;
        }
        case R3N_UNIFORM_VEC3: {
            R3N_GL_Ptr->uniform3fv(uniform.location, 1, ((f32*)&uniform.vec3.data[0]));
            r3_log_stdoutf(SUCCESS_LOG, "[R3N_GL] sent vec3 uniform: (uniform)%s (location)%d\n", name, uniform.location);
            return 1;
        }
        case R3N_UNIFORM_VEC4: {
            R3N_GL_Ptr->uniform4fv(uniform.location, 1, ((f32*)&uniform.vec4.data[0]));
            r3_log_stdoutf(SUCCESS_LOG, "[R3N_GL] sent vec4 uniform: (uniform)%s (location)%d\n", name, uniform.location);
            return 1;
        }
        case R3N_UNIFORM_MAT4: {
            R3N_GL_Ptr->uniform_matrix4fv(uniform.location, 1, 0, ((f32*)&uniform.mat4.data[0]));
            r3_log_stdoutf(SUCCESS_LOG, "[R3N_GL] sent mat4 uniform: (uniform)%s (location)%d\n", name, uniform.location);
            return 1;
        }
        default: break;
    }
    
    return 0;
}

u8 gl_set_uniform_impl(R3N_GPU_Uniform* uniform, R3N_GPU_Program* program) {
    if (!program || !program->program || !program->uniformv.data) {
        r3_log_stdout(ERROR_LOG, "[R3N_GL] invalid program passed to SetUniform()\n");
        return 0;
    } if (!uniform || !uniform->name || !uniform->type || uniform->type >= R3N_UNIFORM_TYPES) {
        r3_log_stdout(ERROR_LOG, "[R3N_GL] invalid uniform passed to SetUniform()\n");
        return 0;
    }

    if (!uniform->location) {
        uniform->location = R3N_GL_Ptr->get_uniform_location(program->program, uniform->name);
        if (uniform->location < 0) {
            r3_log_stdoutf(ERROR_LOG, "[R3N_GL] failed to get uniform location: (uniform)%s (type)%d\n", uniform->name, uniform->type);
            return 0;
        }
    }

    if (!r3_arr_hashed_write(uniform->name, uniform, &program->uniformv)) {
        r3_log_stdoutf(ERROR_LOG, "[R3N_GL] failed to write to uniform array: (uniform)%s (type)%d\n", uniform->name, uniform->type);
        return 0;
    } else { r3_log_stdoutf(INFO_LOG, "[R3N_GL] wrote to uniform array: (uniform)%s (type)%d\n", uniform->name, uniform->type); }

    return 1;
}


u8 gl_create_vertex_buffer_impl(R3N_GPU_Buffer* buffer) {
    if (
        !buffer ||
        buffer->type != R3N_BUFFER_VERTEX ||
        !buffer->vertex.vertexv || !buffer->vertex.attribs || 
        !buffer->vertex.vertices || ((buffer->vertex.attribs & ~((1 << R3N_VERTEX_ATTRIBUTES) - 1)) != 0)
    ) {
        r3_log_stdout(ERROR_LOG, "[R3N_GL] invalid gpu buffer passed to CreateVertexBuffer()\n");
        return 0;
    }

    R3N_GL_Ptr->gen_vertex_arrays(1, &buffer->vertex.vao);
    R3N_GL_Ptr->gen_buffers(1, &buffer->vertex.vbo);

    R3N_GL_Ptr->bind_vertex_array(buffer->vertex.vao);
    R3N_GL_Ptr->bind_buffer(GL_ARRAY_BUFFER, buffer->vertex.vbo);
    R3N_GL_Ptr->buffer_data(GL_ARRAY_BUFFER, buffer->vertex.size, buffer->vertex.vertexv, GL_STATIC_DRAW);
    
    // configure vertex attributes
    u32 stride = 0;
    u32 attrib_offsets[R3N_VERTEX_ATTRIBUTES] = {0};

    // location (vec3), texture (vec2), normal (vec3), color (vec3)
    const u32 attrib_sizes[R3N_VERTEX_ATTRIBUTES] = {3, 2, 3, 3};

    FOR_I(0, R3N_VERTEX_ATTRIBUTES, 1) {
        if ((buffer->vertex.attribs & (1 << i)) != 0) {
            // accumulate stride for enabled vertex attributes
            attrib_offsets[i] = stride;
            stride += attrib_sizes[i];
        }
    }

    buffer->vertex.vertices = buffer->vertex.vertices / stride;

    // enable vertex attributes
    FOR_I(0, R3N_VERTEX_ATTRIBUTES, 1) {
        if ((buffer->vertex.attribs & (1 << i)) != 0) {
            R3N_GL_Ptr->vertex_attrib_pointer(
                i,
                attrib_sizes[i],
                GL_FLOAT,
                GL_FALSE,
                stride * sizeof(f32),
                (ptr)(attrib_offsets[i] * sizeof(f32))
            ); R3N_GL_Ptr->enable_vertex_attrib_array(i);
        }
    }

    R3N_GL_Ptr->bind_buffer(GL_ARRAY_BUFFER, 0);
    R3N_GL_Ptr->bind_vertex_array(0);

    return 1;
}

u8 gl_destroy_vertex_buffer_impl(R3N_GPU_Buffer* buffer) {
    if (!buffer || buffer->type != R3N_BUFFER_VERTEX || !buffer->vertex.vertices) {
        r3_log_stdout(ERROR_LOG, "[R3N_GL] invalid gpu buffer passed to DestroyVertexBuffer()\n");
        return 0;
    };

    R3N_GL_Ptr->delete_vertex_arrays(1, &buffer->vertex.vao);
    R3N_GL_Ptr->delete_buffers(1, &buffer->vertex.vbo);

    buffer->vertex.vbo = 0;
    buffer->vertex.vao = 0;
    buffer->vertex.attribs = 0;
    buffer->vertex.vertices = 0;
    buffer->vertex.vertexv = NULL;

    return 1;
}


u8 gl_create_element_buffer_impl(R3N_GPU_Buffer* buffer) {
    if (!buffer || buffer->type != R3N_BUFFER_ELEMENT || !!buffer->element.elements || !buffer->element.elementv) {
        r3_log_stdout(ERROR_LOG, "[R3N_GL] invalid gpu buffer passed to CreateElementBuffer()\n");
        return 0;
    }
    R3N_GL_Ptr->gen_buffers(1, &buffer->element.ebo);
    R3N_GL_Ptr->bind_buffer(GL_ELEMENT_BUFFER, buffer->element.ebo);
    R3N_GL_Ptr->buffer_data(GL_ELEMENT_BUFFER, buffer->element.size, buffer->element.elementv, GL_STATIC_DRAW);
    R3N_GL_Ptr->bind_buffer(GL_ELEMENT_BUFFER, 0);
    return 1;
}

u8 gl_destroy_element_buffer_impl(R3N_GPU_Buffer* buffer) {
    if (!buffer || buffer->type != R3N_BUFFER_ELEMENT || !!buffer->element.elements || !buffer->element.elementv) {
        r3_log_stdout(ERROR_LOG, "[R3N_GL] invalid gpu buffer passed to DestroyElementBuffer()\n");
        return 0;
    }

    R3N_GL_Ptr->delete_buffers(1, &buffer->element.ebo);
    buffer->element.ebo = 0;
    buffer->element.size = 0;
    buffer->element.elements = 0;
    return 1;
}


u8 gl_create_texture_buffer_impl(R3N_GPU_Buffer* buffer) {
    if (
        !buffer ||
        !buffer->texture.path || !buffer->texture.format || !buffer->texture.channels ||
        buffer->type != R3N_BUFFER_TEXTURE || !buffer->texture.width || !buffer->texture.height ||
        buffer->texture.channels > 4 || buffer->texture.format == R3N_TEXTURE_FORMAT_INVALID) {
        r3_log_stdout(ERROR_LOG, "[R3N_GL] invalid gpu buffer passed to CreateTextureBuffer()\n");
        return 0;
    }

    stbi_set_flip_vertically_on_load(1);
    buffer->texture.data = stbi_load(buffer->texture.path, &buffer->texture.width, &buffer->texture.height, &buffer->texture.channels, 0);
    if (buffer->texture.data == NULL) {
        r3_log_stdoutf(ERROR_LOG, "[R3N_GL] failed to load texture: (width)%d (height)%d (channels)%d (format) %d (path)%s\n",
            buffer->texture.width, buffer->texture.height, buffer->texture.channels, buffer->texture.format, buffer->texture.path);
        buffer->texture.data = NULL;
        return 0;
    }

    R3N_GL_Ptr->gen_textures(1, &buffer->texture.tbo);
    switch(buffer->texture.type) {
        case (R3N_TEXTURE_2D): {
            R3N_GL_Ptr->bind_texture(GL_TEXTURE_2D, buffer->texture.tbo);

            // generate the texture
            R3N_GL_Ptr->tex_image2d(GL_TEXTURE_2D, 0,
                buffer->texture.format,
                buffer->texture.width,
                buffer->texture.height,
                0,
                buffer->texture.format,
                GL_UNSIGNED_BYTE,
                buffer->texture.data
            );
            R3N_GL_Ptr->generate_mipmap(GL_TEXTURE_2D);

            // set texture wrapping options
            R3N_GL_Ptr->tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // x-axis
            R3N_GL_Ptr->tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // y-axis

            // set texture filtering options (scaling up/down)
            R3N_GL_Ptr->tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // minification
            R3N_GL_Ptr->tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // magnification
            
            R3N_GL_Ptr->bind_texture(GL_TEXTURE_2D, 0);
            r3_log_stdoutf(INFO_LOG, "[R3N_GL] loaded texture 2D: (width)%d (height)%d (channels)%d (format) %d (path)%s\n",
                buffer->texture.width, buffer->texture.height, buffer->texture.channels, buffer->texture.format, buffer->texture.path);
        } break;
        case (R3N_TEXTURE_3D): // fall-through
        default: return 0;
    }

    return 1;
}

u8 gl_destroy_texture_buffer_impl(R3N_GPU_Buffer* buffer) {
    if (!buffer || buffer->type != R3N_BUFFER_ELEMENT || !!buffer->element.elements || !buffer->element.elementv) {
        r3_log_stdout(ERROR_LOG, "[R3N_GL] invalid gpu buffer passed to DestroyTextureBuffer()\n");
        return 0;
    }
    
    R3N_GL_Ptr->delete_textures(1, &buffer->texture.tbo);
    stbi_image_free(buffer->texture.data);
    
    buffer->texture.tbo = 0;
    buffer->texture.width = 0;
    buffer->texture.height = 0;
    buffer->texture.format = 0;
    buffer->texture.path = NULL;
    buffer->texture.data = NULL;
    buffer->texture.channels = 0;

    return 1;
}


u8 gl_create_frame_buffer_impl(R3N_GPU_Buffer* buffer) {
    if (!buffer || buffer->type != R3N_BUFFER_FRAME) {
        r3_log_stdout(ERROR_LOG, "[R3N_GL] invalid gpu buffer passed to CreateFrameBuffer()\n");
        return 0;
    }

    R3N_GL_Ptr->gen_framebuffers(1, &buffer->frame.fbo);
    R3N_GL_Ptr->bind_framebuffer(GL_FRAMEBUFFER, buffer->frame.fbo);

    // create the texture/color buffer
    R3N_Window* window_ptr = R3N_Platform_Ptr->get_window();
    if (window_ptr == NULL) {
        r3_log_stdout(ERROR_LOG, "[R3N_GL] failed to retrieve R3N_Window pointer!\n");
        return 0;
    }
    
    R3N_GL_Ptr->gen_textures(1, &buffer->frame.tbo);
    R3N_GL_Ptr->bind_texture(GL_TEXTURE_2D, buffer->frame.tbo);

    R3N_GL_Ptr->tex_image2d(GL_TEXTURE_2D, 0,
        GL_RGB,
        window_ptr->size.data[0],
        window_ptr->size.data[1],
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        NULL
    );
    // set texture filtering options (scaling up/down)
    R3N_GL_Ptr->tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // minification
    R3N_GL_Ptr->tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // magnification    

    // attach the texture/color buffer
    R3N_GL_Ptr->framebuffer_texture2d(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer->frame.tbo, 0);
    R3N_GL_Ptr->bind_texture(GL_TEXTURE_2D, 0);

    // create the render buffer
    R3N_GL_Ptr->gen_renderbuffers(1, &buffer->frame.rbo);
    R3N_GL_Ptr->bind_renderbuffer(GL_RENDERBUFFER, buffer->frame.rbo);
    R3N_GL_Ptr->renderbuffer_storage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window_ptr->size.data[0], window_ptr->size.data[1]);

    // attach the render buffer
    R3N_GL_Ptr->framebuffer_renderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, buffer->frame.rbo);
    R3N_GL_Ptr->bind_renderbuffer(GL_RENDERBUFFER, 0);
    
    R3N_GL_Ptr->bind_framebuffer(GL_FRAMEBUFFER, 0);
    return 1;
}

u8 gl_destroy_frame_buffer_impl(R3N_GPU_Buffer* buffer) {
    if (!buffer || buffer->type != R3N_BUFFER_FRAME) {
        r3_log_stdout(ERROR_LOG, "[R3N_GL] invalid gpu buffer passed to CreateFrameBuffer()\n");
        return 0;
    }

    R3N_GL_Ptr->delete_textures(1, &buffer->frame.tbo);
    R3N_GL_Ptr->delete_framebuffers(1, &buffer->frame.fbo);
    R3N_GL_Ptr->delete_renderbuffers(1, &buffer->frame.rbo);
    
    buffer->frame.tbo = 0;
    buffer->frame.fbo = 0;
    buffer->frame.rbo = 0;

    return 1;
}


u8 gl_bind_buffer_impl(R3N_GPU_Buffer* buffer) {
    if (!buffer) {
        r3_log_stdout(ERROR_LOG, "[R3N_GL] invalid gpu buffer passed to BindBuffer()\n");
        return 0;
    }

    switch(buffer->type) {
        case (R3N_BUFFER_ELEMENT): {
            R3N_GL_Ptr->bind_buffer(GL_ELEMENT_BUFFER, buffer->element.ebo);
            r3_log_stdoutf(INFO_LOG, "[R3N_GL] bound element buffer: (handle)%d (element buffer object)%d (elements)%d\n",
            buffer->handle, buffer->element.ebo, buffer->element.elements);
        } break;
        case (R3N_BUFFER_VERTEX): {
            R3N_GL_Ptr->bind_vertex_array(buffer->vertex.vao);
            R3N_GL_Ptr->bind_buffer(GL_ARRAY_BUFFER, buffer->vertex.vbo);
            r3_log_stdoutf(INFO_LOG, "[R3N_GL] bound vertex buffer: (handle)%d (vertex array object)%d (vertex buffer object)%d (vertices)%d\n",
                buffer->handle, buffer->vertex.vao, buffer->vertex.vbo, buffer->vertex.vertices);
        } break;
        case (R3N_BUFFER_TEXTURE): {
            R3N_GL_Ptr->active_texture(buffer->texture.slot);
            switch(buffer->texture.type) {
                case (R3N_TEXTURE_2D): {
                    R3N_GL_Ptr->bind_texture(GL_TEXTURE_2D, buffer->texture.tbo);
                    r3_log_stdoutf(INFO_LOG, "[R3N_GL] bound texture buffer: (handle)%d (slot)%d (texture buffer object)%d\n",
                        buffer->handle, buffer->texture.slot, buffer->texture.tbo);
                }
                case (R3N_TEXTURE_3D): // fall-through
                default: break;
            }
        } break;
        case (R3N_BUFFER_FRAME): {
            R3N_GL_Ptr->bind_framebuffer(GL_FRAMEBUFFER, buffer->frame.fbo);
            r3_log_stdoutf(INFO_LOG, "[R3N_GL] bound frame buffer: (handle)%d (frame buffer object)%d (texture buffer object)%d (render buffer object)%d\n",
                buffer->handle, buffer->frame.fbo, buffer->frame.tbo, buffer->frame.rbo);
        } break;
        case (R3N_BUFFER_INVALID): // fall-through
        case (R3N_BUFFER_TYPES):   // fall-through
        default: return 0;
    }

    return 1;
}

u8 gl_read_buffer_impl(R3N_GPU_Buffer* buffer) {
    if (!buffer) {
        r3_log_stdout(ERROR_LOG, "[R3N_GL] invalid gpu buffer passed to ReadBuffer()\n");
        return 0;
    }

    switch(buffer->type) {
        case (R3N_BUFFER_ELEMENT): {
            r3_log_stdoutf(INFO_LOG, "[R3N_GL] reading element buffer: (handle)%d (elements)%d\n", buffer->handle, buffer->element.elements);
            R3N_GL_Ptr->draw_elements(R3N_MODE_TRIANGLE, buffer->element.elements, GL_UNSIGNED_INT, buffer->element.elementv);
        } break;
        case (R3N_BUFFER_VERTEX): {
            r3_log_stdoutf(INFO_LOG, "[R3N_GL] reading vertex buffer: (handle)%d (vertices)%d\n", buffer->handle, buffer->vertex.vertices);
            R3N_GL_Ptr->draw_arrays(R3N_MODE_TRIANGLE, 0, buffer->vertex.vertices);
        } break;
        case (R3N_BUFFER_TEXTURE): // fall-through
        case (R3N_BUFFER_FRAME):   // fall-through
        case (R3N_BUFFER_INVALID): // fall-through
        case (R3N_BUFFER_TYPES):   // fall-through
        default: return 0;
    }

    return 1;
}

// TODO: implement this function (glWriteBufferImpl) !!!!
u8 gl_write_buffer_impl(R3N_GPU_Buffer* buffer) {
    return 0;
}

u8 gl_bind_program_impl(R3N_GPU_Program* program) {
    if (!program) {
        r3_log_stdout(ERROR_LOG, "[R3N_GL] invalid gpu program passed to BindProgram()\n");
        return 0;
    }
    
    R3N_GL_Ptr->use_program(program->program);
    
    r3_log_stdoutf(INFO_LOG, "[R3N_GL] bound gpu program: (handle)%d (program object)%d\n", program->handle, program->program);
    return 1;
}


u8 r3n_init_gl(none) {
    if (R3N_Platform_Ptr == NULL) {
        r3_log_stdout(ERROR_LOG, "[R3N_GL] LunaPlatformApi not initialized!\n");
        return 0;
    }

    if (R3N_GPU_Ptr == NULL) {
        R3N_GPU_Ptr = r3_mem_alloc(sizeof(R3N_GPU), 8);
        if (R3N_GPU_Ptr == NULL) {
            r3_log_stdout(ERROR_LOG, "[R3N_GL] failed to allocate R3N_GPU_Ptr dispatch table!\n");
            return 0;
        }
    }

    if (R3N_GL_Ptr == NULL) {
        R3N_GL_Ptr = r3_mem_alloc(sizeof(R3N_GL), 8);
        if (R3N_GL_Ptr == NULL) {
            r3_log_stdout(ERROR_LOG, "[R3N_GL] failed to allocate R3N_GL_Ptr dispatch table!\n");
            r3_mem_dealloc(R3N_GPU_Ptr);
            return 0;
        }
    }

    if (!R3N_Platform_Ptr->create_gl_context()) {
        r3_log_stdout(ERROR_LOG, "[R3N_GL] failed to create opengl context\n");
        return 0;
    } else r3_log_stdout(SUCCESS_LOG, "[R3N_GL] created opengl context\n");
    
    // glapi
    R3N_Library opengl32;
    if (!R3N_Platform_Ptr->load_library(NULL, "opengl32", &opengl32)) {
        r3_log_stdout(ERROR_LOG, "[R3N_GL] failed to load opengl\n");
        return 0;
    }  else r3_log_stdoutf(SUCCESS_LOG, "[R3N_GL] loaded opengl v%s %s %s\n", glGetString(GL_VERSION), glGetString(GL_VENDOR), glGetString(GL_RENDERER));

    struct gl_func {
        ptr* function;
        str name;
    } functions[] = {
        // BUFFER FUNCTIONS
        {(ptr*)&R3N_GL_Ptr->gen_buffers, "glGenBuffers"},
        {(ptr*)&R3N_GL_Ptr->bind_buffer, "glBindBuffer"},
        {(ptr*)&R3N_GL_Ptr->buffer_data, "glBufferData"},
        {(ptr*)&R3N_GL_Ptr->map_buffer, "glMapBuffer"},
        {(ptr*)&R3N_GL_Ptr->unmap_buffer, "glUnmapBuffer"},
        {(ptr*)&R3N_GL_Ptr->buffer_sub_data, "glBufferSubData"},
        {(ptr*)&R3N_GL_Ptr->get_buffer_parameteriv, "glGetBufferParameteriv"},
        {(ptr*)&R3N_GL_Ptr->delete_buffers, "glDeleteBuffers"},

        // VAO FUNCTIONS
        {(ptr*)&R3N_GL_Ptr->gen_vertex_arrays, "glGenVertexArrays"},
        {(ptr*)&R3N_GL_Ptr->bind_vertex_array, "glBindVertexArray"},
        {(ptr*)&R3N_GL_Ptr->delete_vertex_arrays, "glDeleteVertexArrays"},
        {(ptr*)&R3N_GL_Ptr->enable_vertex_attrib_array, "glEnableVertexAttribArray"},
        {(ptr*)&R3N_GL_Ptr->disable_vertex_attrib_array, "glDisableVertexAttribArray"},
        {(ptr*)&R3N_GL_Ptr->vertex_attrib_pointer, "glVertexAttribPointer"},

        // SHADER FUNCTIONS
        {(ptr*)&R3N_GL_Ptr->create_shader, "glCreateShader"},
        {(ptr*)&R3N_GL_Ptr->shader_source, "glShaderSource"},
        {(ptr*)&R3N_GL_Ptr->compile_shader, "glCompileShader"},
        {(ptr*)&R3N_GL_Ptr->delete_shader, "glDeleteShader"},
        {(ptr*)&R3N_GL_Ptr->get_shaderiv, "glGetShaderiv"},
        {(ptr*)&R3N_GL_Ptr->get_shader_info_log, "glGetShaderInfoLog"},

        {(ptr*)&R3N_GL_Ptr->create_program, "glCreateProgram"},
        {(ptr*)&R3N_GL_Ptr->attach_shader, "glAttachShader"},
        {(ptr*)&R3N_GL_Ptr->detach_shader, "glDetachShader"},
        {(ptr*)&R3N_GL_Ptr->link_program, "glLinkProgram"},
        {(ptr*)&R3N_GL_Ptr->use_program, "glUseProgram"},
        {(ptr*)&R3N_GL_Ptr->delete_program, "glDeleteProgram"},
        {(ptr*)&R3N_GL_Ptr->get_programiv, "glGetProgramiv"},
        {(ptr*)&R3N_GL_Ptr->get_program_info_log, "glGetProgramInfoLog"},
        {(ptr*)&R3N_GL_Ptr->get_uniform_location, "glGetUniformLocation"},

        // UNIFORMS
        {(ptr*)&R3N_GL_Ptr->uniform1i, "glUniform1i"},
        {(ptr*)&R3N_GL_Ptr->uniform1f, "glUniform1f"},
        {(ptr*)&R3N_GL_Ptr->uniform2fv, "glUniform2fv"},
        {(ptr*)&R3N_GL_Ptr->uniform3fv, "glUniform3fv"},
        {(ptr*)&R3N_GL_Ptr->uniform4fv, "glUniform4fv"},
        {(ptr*)&R3N_GL_Ptr->uniform_matrix4fv, "glUniformMatrix4fv"},

        // TEXTURES
        {(ptr*)&R3N_GL_Ptr->gen_textures, "glGenTextures"},
        {(ptr*)&R3N_GL_Ptr->bind_texture, "glBindTexture"},
        {(ptr*)&R3N_GL_Ptr->tex_parameteri, "glTexParameteri"},
        {(ptr*)&R3N_GL_Ptr->tex_image2d, "glTexImage2D"},
        {(ptr*)&R3N_GL_Ptr->active_texture, "glActiveTexture"},
        {(ptr*)&R3N_GL_Ptr->delete_textures, "glDeleteTextures"},
        {(ptr*)&R3N_GL_Ptr->generate_mipmap, "glGenerateMipmap"},

        // FRAMEBUFFERS + RENDERBUFFERS
        {(ptr*)&R3N_GL_Ptr->gen_framebuffers, "glGenFramebuffers"},
        {(ptr*)&R3N_GL_Ptr->bind_framebuffer, "glBindFramebuffer"},
        {(ptr*)&R3N_GL_Ptr->framebuffer_texture2d, "glFramebufferTexture2D"},
        {(ptr*)&R3N_GL_Ptr->framebuffer_renderbuffer, "glFramebufferRenderbuffer"},
        {(ptr*)&R3N_GL_Ptr->check_framebuffer_status, "glCheckFramebufferStatus"},
        {(ptr*)&R3N_GL_Ptr->delete_framebuffers, "glDeleteFramebuffers"},

        {(ptr*)&R3N_GL_Ptr->gen_renderbuffers, "glGenRenderbuffers"},
        {(ptr*)&R3N_GL_Ptr->bind_renderbuffer, "glBindRenderbuffer"},
        {(ptr*)&R3N_GL_Ptr->renderbuffer_storage, "glRenderbufferStorage"},
        {(ptr*)&R3N_GL_Ptr->delete_renderbuffers, "glDeleteRenderbuffers"},

        // DRAWING
        {(ptr*)&R3N_GL_Ptr->draw_arrays, "glDrawArrays"},
        {(ptr*)&R3N_GL_Ptr->draw_elements, "glDrawElements"},

        // STATE
        {(ptr*)&R3N_GL_Ptr->enable, "glEnable"},
        {(ptr*)&R3N_GL_Ptr->disable, "glDisable"},
        {(ptr*)&R3N_GL_Ptr->blend_func, "glBlendFunc"},
        {(ptr*)&R3N_GL_Ptr->cull_face, "glCullFace"},
        {(ptr*)&R3N_GL_Ptr->depth_func, "glDepthFunc"},
        {(ptr*)&R3N_GL_Ptr->viewport, "glViewport"},
        {(ptr*)&R3N_GL_Ptr->polygon_mode, "glPolygonMode"},
        {(ptr*)&R3N_GL_Ptr->clear, "glClear"},
        {(ptr*)&R3N_GL_Ptr->clear_color, "glClearColor"},
        {(ptr*)&R3N_GL_Ptr->clear_depth, "glClearDepth"},

        // INFO / DEBUG
        {(ptr*)&R3N_GL_Ptr->get_error, "glGetError"},
        {(ptr*)&R3N_GL_Ptr->get_string, "glGetString"}
    };

    FOR_I(0, sizeof(functions) / sizeof(functions[0]), 1) {
        if (!R3N_Platform_Ptr->load_library_symbol(functions[i].name, functions[i].function, &opengl32) || *functions[i].function == NULL) {
            r3_log_stdoutf(WARN_LOG, "[R3N_GL] failed to load function: (name)%s\n", functions[i].name);
        } else r3_log_stdoutf(SUCCESS_LOG, "[R3N_GL] loaded function: (name)%s (ptr)%p\n", functions[i].name, *(functions[i].function));
    }

    R3N_Platform_Ptr->unload_library(&opengl32);

    // gpuapi
    R3N_GPU_Ptr->clear_depth_buffer = gl_clear_depth_buffer_impl;
    R3N_GPU_Ptr->clear_color_buffer = gl_clear_color_buffer_impl;
    
    R3N_GPU_Ptr->create_program = gl_create_program_impl;
    R3N_GPU_Ptr->destroy_program = gl_destroy_program_impl;
    
    R3N_GPU_Ptr->send_uniform = gl_send_uniform_impl;
    R3N_GPU_Ptr->set_uniform = gl_set_uniform_impl;
    
    R3N_GPU_Ptr->create_vertex_buffer = gl_create_vertex_buffer_impl;
    R3N_GPU_Ptr->destroy_vertex_buffer = gl_destroy_vertex_buffer_impl;
    
    R3N_GPU_Ptr->create_element_buffer = gl_create_element_buffer_impl;
    R3N_GPU_Ptr->destroy_element_buffer = gl_destroy_element_buffer_impl;
    
    R3N_GPU_Ptr->create_texture_buffer = gl_create_texture_buffer_impl;
    R3N_GPU_Ptr->destroy_texture_buffer = gl_destroy_texture_buffer_impl;
    
    R3N_GPU_Ptr->create_frame_buffer = gl_create_frame_buffer_impl;
    R3N_GPU_Ptr->destroy_frame_buffer = gl_destroy_frame_buffer_impl;
    
    R3N_GPU_Ptr->bind_buffer = gl_bind_buffer_impl;
    R3N_GPU_Ptr->read_buffer = gl_read_buffer_impl;
    R3N_GPU_Ptr->write_buffer = gl_write_buffer_impl;
    R3N_GPU_Ptr->bind_program = gl_bind_program_impl;

    r3_log_stdout(SUCCESS_LOG, "[R3N_GL] initialized R3N_GL + R3N_GPU\n");
    return 1;
}

u8 r3n_deinit_gl(none) {
    if (R3N_GPU_Ptr != NULL) {
        r3_mem_dealloc(R3N_GPU_Ptr);
        R3N_GPU_Ptr = NULL;
    }
    
    if (R3N_GL_Ptr != NULL) {
        r3_mem_dealloc(R3N_GL_Ptr);
        R3N_GL_Ptr = NULL;
    }

    R3N_Platform_Ptr->destroy_gl_context();

    r3_log_stdout(SUCCESS_LOG, "[R3N_GL] deinitialized R3N_GL + R3N_GPU\n");
    return 1;
}
