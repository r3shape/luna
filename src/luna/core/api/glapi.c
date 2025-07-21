#define _LUNA_INTERNAL_

#include <include/luna/core/platform.h>
#include <include/luna/core/renderer.h>
#include <include/luna/core/api/gl.h>
#include <include/luna/core/api/gpu.h>

#define STB_IMAGE_IMPLEMENTATION
#include <include/STB/stb_image.h>

// dispatch table ptrs
LunaGpuApi* lunaGpuApi = NULL;
LunaOpenGL* lunaOpenGL = NULL;

u8 glClearDepthBufferImpl(f32 depth) {
    lunaOpenGL->glClearDepth(depth);
    lunaOpenGL->glClear(GL_DEPTH_BUFFER_BIT);
    return 1;
}

u8 glClearColorBufferImpl(Vec4 color) {
    lunaOpenGL->glClearColor(color.data[0] / 255, color.data[1] / 255, color.data[2] / 255, color.data[3] / 255);
    lunaOpenGL->glClear(GL_COLOR_BUFFER_BIT);
    return 1;
}


u8 glCreateProgramImpl(LunaGpuProgram* program) {
    if (!program || !program->vertex_buffer.data || !program->fragment_buffer.data) {
        r3_log_stdout(ERROR_LOG, "[LunaOpenGL] invalid program passed to CreateProgram()\n");
        return 0;
    }

    s32 link_status = 0;
    s32 compile_status = 0;
    
    s32 vertex_shader = lunaOpenGL->glCreateShader(GLAPI_VERTEX_SHADER);
    lunaOpenGL->glShaderSource(vertex_shader, 1, &(cstr){program->vertex_buffer.data}, NULL);
    lunaOpenGL->glCompileShader(vertex_shader);
    
    lunaOpenGL->glGetShaderiv(vertex_shader, GLAPI_COMPILE_STATUS, &compile_status);
    if (!compile_status) {
        r3_log_stdout(INFO_LOG, "[LunaOpenGL] failed to compile vertex shader\n");
        lunaOpenGL->glDeleteShader(vertex_shader);
        return 0;
    } else { r3_log_stdout(INFO_LOG, "[LunaOpenGL] compiled vertex shader\n"); }
    
    s32 fragment_shader = lunaOpenGL->glCreateShader(GLAPI_FRAGMENT_SHADER);
    lunaOpenGL->glShaderSource(fragment_shader, 1, &(cstr){program->fragment_buffer.data}, NULL);
    lunaOpenGL->glCompileShader(fragment_shader);
    
    lunaOpenGL->glGetShaderiv(fragment_shader, GLAPI_COMPILE_STATUS, &compile_status);
    if (!compile_status) {
        r3_log_stdout(INFO_LOG, "[LunaOpenGL] failed to compile fragment shader\n");
        lunaOpenGL->glDeleteShader(vertex_shader);
        lunaOpenGL->glDeleteShader(fragment_shader);
        return 0;
    } else { r3_log_stdout(INFO_LOG, "[LunaOpenGL] compiled fragment shader\n"); }
        
    program->program = lunaOpenGL->glCreateProgram();
    lunaOpenGL->glAttachShader(program->program, vertex_shader);
    lunaOpenGL->glAttachShader(program->program, fragment_shader);
    lunaOpenGL->glLinkProgram(program->program);

    lunaOpenGL->glGetProgramiv(program->program, GLAPI_LINK_STATUS, &link_status);
    if (!link_status) {
        r3_log_stdout(INFO_LOG, "[LunaOpenGL] failed to link program\n");
        lunaOpenGL->glDeleteShader(vertex_shader);
        lunaOpenGL->glDeleteShader(fragment_shader);
        return 0;
    }

    lunaOpenGL->glDeleteShader(vertex_shader);
    lunaOpenGL->glDeleteShader(fragment_shader);
    return 1;
}

u8 glDestroyProgramImpl(LunaGpuProgram* program) {
    if (!program || !program->program || !program->uniformv.data) {
        r3_log_stdout(ERROR_LOG, "[LunaOpenGL] invalid program passed to DestroyProgram()\n");
        return 0;
    }
    r3_arr_hashed_dealloc(&program->uniformv);
    lunaOpenGL->glDeleteProgram(program->program);
    return 1;
}


u8 glSendUniformImpl(cstr name, LunaGpuProgram* program) {
    if (!program || !program->program || !program->uniformv.data) {
        r3_log_stdout(ERROR_LOG, "[LunaOpenGL] invalid program passed to SendUniform()\n");
        return 0;
    } 
    
    LunaGpuUniform uniform = {0};
    if (!r3_arr_hashed_read(name, &uniform, &program->uniformv)){
        r3_log_stdoutf(ERROR_LOG, "[LunaOpenGL]  failed to read from uniform array: (uniform)%s\n", name);
        return 0;
    }
        
    if (strcmp(uniform.name, name) != 0) {
        r3_log_stdoutf(ERROR_LOG, "[LunaOpenGL]  uniform not found: (uniform)%s\n", name);
        return 0;
    }

    switch (uniform.type) {
        case LUNA_UNIFORM_INVALID: break;
        case LUNA_UNIFORM_TYPES: break;
        case LUNA_UNIFORM_INT: {
            lunaOpenGL->glUniform1i(uniform.location, uniform.s32);
            r3_log_stdoutf(SUCCESS_LOG, "[LunaOpenGL] sent int uniform: (uniform)%s\n", name);
            return 1;
        }
        case LUNA_UNIFORM_FLOAT: {
            lunaOpenGL->glUniform1f(uniform.location, uniform.f32);
            r3_log_stdoutf(SUCCESS_LOG, "[LunaOpenGL] sent float uniform: (uniform)%s\n", name);
            return 1;
        }
        case LUNA_UNIFORM_VEC2: {
            lunaOpenGL->glUniform2fv(uniform.location, 1, (f32*)uniform.vec2.data);
            r3_log_stdoutf(SUCCESS_LOG, "[LunaOpenGL] sent vec2 uniform: (uniform)%s\n", name);
            return 1;
        }
        case LUNA_UNIFORM_VEC3: {
            lunaOpenGL->glUniform2fv(uniform.location, 1, (f32*)uniform.vec3.data);
            r3_log_stdoutf(SUCCESS_LOG, "[LunaOpenGL] sent vec3 uniform: (uniform)%s\n", name);
            return 1;
        }
        case LUNA_UNIFORM_VEC4: {
            lunaOpenGL->glUniform2fv(uniform.location, 1, (f32*)uniform.vec4.data);
            r3_log_stdoutf(SUCCESS_LOG, "[LunaOpenGL] sent vec4 uniform: (uniform)%s\n", name);
            return 1;
        }
        case LUNA_UNIFORM_MAT4: {
            lunaOpenGL->glUniformMatrix4fv(uniform.location, 1, 0, (f32*)uniform.mat4.data);
            r3_log_stdoutf(SUCCESS_LOG, "[LunaOpenGL] sent mat4 uniform: (uniform)%s\n", name);
            return 1;
        }
        default: break;
    }
    
    return 0;
}

u8 glSetUniformImpl(LunaGpuUniform* uniform, LunaGpuProgram* program) {
    if (!program || !program->program || !program->uniformv.data) {
        r3_log_stdout(ERROR_LOG, "[LunaOpenGL] invalid program passed to SetUniform()\n");
        return 0;
    } if (!uniform || !uniform->name || !uniform->type || uniform->type >= LUNA_UNIFORM_TYPES) {
        r3_log_stdout(ERROR_LOG, "[LunaOpenGL] invalid uniform passed to SetUniform()\n");
        return 0;
    }

    if (!uniform->location) { uniform->location = lunaOpenGL->glGetUniformLocation(program->program, uniform->name); }

    if (!r3_arr_hashed_write(uniform->name, uniform, &program->uniformv)) {
        r3_log_stdoutf(ERROR_LOG, "[LunaOpenGL] failed to write to uniform array: (uniform)%s (type)%d\n", uniform->name, uniform->type);
        return 0;
    } else { r3_log_stdoutf(INFO_LOG, "[LunaOpenGL] wrote to uniform array: (uniform)%s (type)%d\n", uniform->name, uniform->type); }

    return 1;
}


u8 glCreateVertexBufferImpl(LunaGpuBuffer* buffer) {
    if (
        !buffer ||
        buffer->type != LUNA_BUFFER_VERTEX ||
        !buffer->vertex.vertexv || !buffer->vertex.attribs || 
        !buffer->vertex.vertices || ((buffer->vertex.attribs & ~((1 << LUNA_VERTEX_ATTRIBUTES) - 1)) != 0)
    ) {
        r3_log_stdout(ERROR_LOG, "[LunaOpenGL] invalid gpu buffer passed to CreateVertexBuffer()\n");
        return 0;
    }

    lunaOpenGL->glGenVertexArrays(1, &buffer->vertex.vao);
    lunaOpenGL->glGenBuffers(1, &buffer->vertex.vbo);

    lunaOpenGL->glBindVertexArray(buffer->vertex.vao);
    lunaOpenGL->glBindBuffer(GLAPI_ARRAY_BUFFER, buffer->vertex.vbo);
    lunaOpenGL->glBufferData(GLAPI_ARRAY_BUFFER, buffer->vertex.size, buffer->vertex.vertexv, GLAPI_STATIC_DRAW);
    
    // configure vertex attributes
    u32 stride = 0;
    u32 attrib_offsets[LUNA_VERTEX_ATTRIBUTES] = {0};

    // location (vec3), texture (vec2), normal (vec3), color (vec3)
    const u32 attrib_sizes[LUNA_VERTEX_ATTRIBUTES] = {3, 2, 3, 3};

    FOR_I(0, LUNA_VERTEX_ATTRIBUTES, 1) {
        if ((buffer->vertex.attribs & (1 << i)) != 0) {
            // accumulate stride for enabled vertex attributes
            attrib_offsets[i] = stride;
            stride += attrib_sizes[i];
        }
    }

    buffer->vertex.vertices = buffer->vertex.vertices / stride;

    // enable vertex attributes
    FOR_I(0, LUNA_VERTEX_ATTRIBUTES, 1) {
        if ((buffer->vertex.attribs & (1 << i)) != 0) {
            lunaOpenGL->glVertexAttribPointer(
                i,
                attrib_sizes[i],
                GL_FLOAT,
                GL_FALSE,
                stride * sizeof(f32),
                (ptr)(attrib_offsets[i] * sizeof(f32))
            ); lunaOpenGL->glEnableVertexAttribArray(i);
        }
    }

    lunaOpenGL->glBindBuffer(GLAPI_ARRAY_BUFFER, 0);
    lunaOpenGL->glBindVertexArray(0);

    return 1;
}

u8 glDestroyVertexBufferImpl(LunaGpuBuffer* buffer) {
    if (!buffer || buffer->type != LUNA_BUFFER_VERTEX || !buffer->vertex.vertices) {
        r3_log_stdout(ERROR_LOG, "[LunaOpenGL] invalid gpu buffer passed to DestroyVertexBuffer()\n");
        return 0;
    };

    lunaOpenGL->glDeleteVertexArrays(1, &buffer->vertex.vao);
    lunaOpenGL->glDeleteBuffers(1, &buffer->vertex.vbo);

    buffer->vertex.vbo = 0;
    buffer->vertex.vao = 0;
    buffer->vertex.attribs = 0;
    buffer->vertex.vertices = 0;
    buffer->vertex.vertexv = NULL;

    return 1;
}


u8 glCreateElementBufferImpl(LunaGpuBuffer* buffer) {
    if (!buffer || buffer->type != LUNA_BUFFER_ELEMENT || !!buffer->element.elements || !buffer->element.elementv) {
        r3_log_stdout(ERROR_LOG, "[LunaOpenGL] invalid gpu buffer passed to CreateElementBuffer()\n");
        return 0;
    }
    lunaOpenGL->glGenBuffers(1, &buffer->element.ebo);
    lunaOpenGL->glBindBuffer(GLAPI_ELEMENT_BUFFER, buffer->element.ebo);
    lunaOpenGL->glBufferData(GLAPI_ELEMENT_BUFFER, buffer->element.size, buffer->element.elementv, GLAPI_STATIC_DRAW);
    lunaOpenGL->glBindBuffer(GLAPI_ELEMENT_BUFFER, 0);
    return 1;
}

u8 glDestroyElementBufferImpl(LunaGpuBuffer* buffer) {
    if (!buffer || buffer->type != LUNA_BUFFER_ELEMENT || !!buffer->element.elements || !buffer->element.elementv) {
        r3_log_stdout(ERROR_LOG, "[LunaOpenGL] invalid gpu buffer passed to DestroyElementBuffer()\n");
        return 0;
    }

    lunaOpenGL->glDeleteBuffers(1, &buffer->element.ebo);
    buffer->element.ebo = 0;
    buffer->element.size = 0;
    buffer->element.elements = 0;
    return 1;
}


u8 glCreateTextureBufferImpl(LunaGpuBuffer* buffer) {
    if (
        !buffer ||
        !buffer->texture.path || !buffer->texture.format || !buffer->texture.channels ||
        buffer->type != LUNA_BUFFER_TEXTURE || !buffer->texture.width || !buffer->texture.height ||
        buffer->texture.channels > 4 || buffer->texture.format == LUNA_TEXTURE_FORMAT_INVALID) {
        r3_log_stdout(ERROR_LOG, "[LunaOpenGL] invalid gpu buffer passed to CreateTextureBuffer()\n");
        return 0;
    }

    stbi_set_flip_vertically_on_load(1);
    buffer->texture.data = stbi_load(buffer->texture.path, &buffer->texture.width, &buffer->texture.height, &buffer->texture.channels, 0);
    if (buffer->texture.data == NULL) {
        r3_log_stdoutf(ERROR_LOG, "[LunaOpenGL] failed to load texture: (width)%d (height)%d (channels)%d (format) %d (path)%s\n",
            buffer->texture.width, buffer->texture.height, buffer->texture.channels, buffer->texture.format, buffer->texture.path);
        buffer->texture.data = NULL;
        return 0;
    }

    lunaOpenGL->glGenTextures(1, &buffer->texture.tbo);
    switch(buffer->texture.type) {
        case (LUNA_TEXTURE_2D): {
            lunaOpenGL->glBindTexture(GL_TEXTURE_2D, buffer->texture.tbo);

            // generate the texture
            lunaOpenGL->glTexImage2D(GL_TEXTURE_2D, 0,
                buffer->texture.format,
                buffer->texture.width,
                buffer->texture.height,
                0,
                buffer->texture.format,
                GL_UNSIGNED_BYTE,
                buffer->texture.data
            );
            lunaOpenGL->glGenerateMipmap(GL_TEXTURE_2D);

            // set texture wrapping options
            lunaOpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // x-axis
            lunaOpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // y-axis

            // set texture filtering options (scaling up/down)
            lunaOpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // minification
            lunaOpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // magnification
            
            lunaOpenGL->glBindTexture(GL_TEXTURE_2D, 0);
            r3_log_stdoutf(INFO_LOG, "[LunaOpenGL] loaded texture 2D: (width)%d (height)%d (channels)%d (format) %d (path)%s\n",
                buffer->texture.width, buffer->texture.height, buffer->texture.channels, buffer->texture.format, buffer->texture.path);
        } break;
        case (LUNA_TEXTURE_3D): // fall-through
        default: return 0;
    }

    return 1;
}

u8 glDestroyTextureBufferImpl(LunaGpuBuffer* buffer) {
    if (!buffer || buffer->type != LUNA_BUFFER_ELEMENT || !!buffer->element.elements || !buffer->element.elementv) {
        r3_log_stdout(ERROR_LOG, "[LunaOpenGL] invalid gpu buffer passed to DestroyTextureBuffer()\n");
        return 0;
    }
    
    lunaOpenGL->glDeleteTextures(1, &buffer->texture.tbo);
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


u8 glCreateFrameBufferImpl(LunaGpuBuffer* buffer) {
    if (!buffer || buffer->type != LUNA_BUFFER_FRAME) {
        r3_log_stdout(ERROR_LOG, "[LunaOpenGL] invalid gpu buffer passed to CreateFrameBuffer()\n");
        return 0;
    }

    lunaOpenGL->glGenFramebuffers(1, &buffer->frame.fbo);
    lunaOpenGL->glBindFramebuffer(GLAPI_FRAMEBUFFER, buffer->frame.fbo);

    // create the texture/color buffer
    LunaWindow* window_ptr = lunaPlatformApi->getWindow();
    if (window_ptr == NULL) {
        r3_log_stdout(ERROR_LOG, "[LunaOpenGL] failed to retrieve LunaWindow pointer!\n");
        return 0;
    }
    
    lunaOpenGL->glGenTextures(1, &buffer->frame.tbo);
    lunaOpenGL->glBindTexture(GL_TEXTURE_2D, buffer->frame.tbo);

    lunaOpenGL->glTexImage2D(GL_TEXTURE_2D, 0,
        GL_RGB,
        window_ptr->size.data[0],
        window_ptr->size.data[1],
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        NULL
    );
    // set texture filtering options (scaling up/down)
    lunaOpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // minification
    lunaOpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // magnification    

    // attach the texture/color buffer
    lunaOpenGL->glFramebufferTexture2D(GLAPI_FRAMEBUFFER, GLAPI_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer->frame.tbo, 0);
    lunaOpenGL->glBindTexture(GL_TEXTURE_2D, 0);

    // create the render buffer
    lunaOpenGL->glGenRenderbuffers(1, &buffer->frame.rbo);
    lunaOpenGL->glBindRenderbuffer(GLAPI_RENDERBUFFER, buffer->frame.rbo);
    lunaOpenGL->glRenderbufferStorage(GLAPI_RENDERBUFFER, GLAPI_DEPTH24_STENCIL8, window_ptr->size.data[0], window_ptr->size.data[1]);

    // attach the render buffer
    lunaOpenGL->glFramebufferRenderbuffer(GLAPI_FRAMEBUFFER, GLAPI_DEPTH_STENCIL_ATTACHMENT, GLAPI_RENDERBUFFER, buffer->frame.rbo);
    lunaOpenGL->glBindRenderbuffer(GLAPI_RENDERBUFFER, 0);
    
    lunaOpenGL->glBindFramebuffer(GLAPI_FRAMEBUFFER, 0);
    return 1;
}

u8 glDestroyFrameBufferImpl(LunaGpuBuffer* buffer) {
    if (!buffer || buffer->type != LUNA_BUFFER_FRAME) {
        r3_log_stdout(ERROR_LOG, "[LunaOpenGL] invalid gpu buffer passed to CreateFrameBuffer()\n");
        return 0;
    }

    lunaOpenGL->glDeleteTextures(1, &buffer->frame.tbo);
    lunaOpenGL->glDeleteFramebuffers(1, &buffer->frame.fbo);
    lunaOpenGL->glDeleteRenderbuffers(1, &buffer->frame.rbo);
    
    buffer->frame.tbo = 0;
    buffer->frame.fbo = 0;
    buffer->frame.rbo = 0;

    return 1;
}


u8 glBindBufferImpl(LunaGpuBuffer* buffer) {
    if (!buffer) {
        r3_log_stdout(ERROR_LOG, "[LunaOpenGL] invalid gpu buffer passed to BindBuffer()\n");
        return 0;
    }

    switch(buffer->type) {
        case (LUNA_BUFFER_ELEMENT): {
            lunaOpenGL->glBindBuffer(GLAPI_ELEMENT_BUFFER, buffer->element.ebo);
            r3_log_stdoutf(INFO_LOG, "[LunaOpenGL] bound element buffer: (handle)%d (element buffer object)%d (elements)%d\n",
            buffer->handle, buffer->element.ebo, buffer->element.elements);
        } break;
        case (LUNA_BUFFER_VERTEX): {
            lunaOpenGL->glBindVertexArray(buffer->vertex.vao);
            lunaOpenGL->glBindBuffer(GLAPI_ARRAY_BUFFER, buffer->vertex.vbo);
            r3_log_stdoutf(INFO_LOG, "[LunaOpenGL] bound vertex buffer: (handle)%d (vertex array object)%d (vertex buffer object)%d (vertices)%d\n",
                buffer->handle, buffer->vertex.vao, buffer->vertex.vbo, buffer->vertex.vertices);
        } break;
        case (LUNA_BUFFER_TEXTURE): {
            lunaOpenGL->glActiveTexture(buffer->texture.slot);
            switch(buffer->texture.type) {
                case (LUNA_TEXTURE_2D): {
                    lunaOpenGL->glBindTexture(GL_TEXTURE_2D, buffer->texture.tbo);
                    r3_log_stdoutf(INFO_LOG, "[LunaOpenGL] bound texture buffer: (handle)%d (slot)%d (texture buffer object)%d\n",
                        buffer->handle, buffer->texture.slot, buffer->texture.tbo);
                }
                case (LUNA_TEXTURE_3D): // fall-through
                default: break;
            }
        } break;
        case (LUNA_BUFFER_FRAME): {
            lunaOpenGL->glBindFramebuffer(GLAPI_FRAMEBUFFER, buffer->frame.fbo);
            r3_log_stdoutf(INFO_LOG, "[LunaOpenGL] bound frame buffer: (handle)%d (frame buffer object)%d (texture buffer object)%d (render buffer object)%d\n",
                buffer->handle, buffer->frame.fbo, buffer->frame.tbo, buffer->frame.rbo);
        } break;
        case (LUNA_BUFFER_INVALID): // fall-through
        case (LUNA_BUFFER_TYPES):   // fall-through
        default: return 0;
    }

    return 1;
}

u8 glReadBufferImpl(LunaGpuBuffer* buffer) {
    if (!buffer) {
        r3_log_stdout(ERROR_LOG, "[LunaOpenGL] invalid gpu buffer passed to ReadBuffer()\n");
        return 0;
    }

    switch(buffer->type) {
        case (LUNA_BUFFER_ELEMENT): {
            r3_log_stdoutf(INFO_LOG, "[LunaOpenGL] reading element buffer: (handle)%d (elements)%d\n", buffer->handle, buffer->element.elements);
            lunaOpenGL->glDrawElements(LUNA_MODE_TRIANGLE, buffer->element.elements, GL_UNSIGNED_INT, buffer->element.elementv);
        } break;
        case (LUNA_BUFFER_VERTEX): {
            r3_log_stdoutf(INFO_LOG, "[LunaOpenGL] reading vertex buffer: (handle)%d (vertices)%d\n", buffer->handle, buffer->vertex.vertices);
            lunaOpenGL->glDrawArrays(LUNA_MODE_TRIANGLE, 0, buffer->vertex.vertices);
        } break;
        case (LUNA_BUFFER_TEXTURE): // fall-through
        case (LUNA_BUFFER_FRAME):   // fall-through
        case (LUNA_BUFFER_INVALID): // fall-through
        case (LUNA_BUFFER_TYPES):   // fall-through
        default: return 0;
    }

    return 1;
}

// TODO: implement this function (glWriteBufferImpl) !!!!
u8 glWriteBufferImpl(LunaGpuBuffer* buffer) {
    return 0;
}

u8 glBindProgramImpl(LunaGpuProgram* program) {
    if (!program) {
        r3_log_stdout(ERROR_LOG, "[LunaOpenGL] invalid gpu program passed to BindProgram()\n");
        return 0;
    }
    
    lunaOpenGL->glUseProgram(program->program);
    
    r3_log_stdoutf(INFO_LOG, "[LunaOpenGL] bound gpu program: (handle)%d (program object)%d\n", program->handle, program->program);
    return 1;
}


u8 lunaInitOpenGL(none) {
    if (lunaPlatformApi == NULL) {
        r3_log_stdout(ERROR_LOG, "[LunaOpenGL] LunaPlatformApi not initialized!\n");
        return 0;
    }

    if (lunaGpuApi == NULL) {
        lunaGpuApi = r3_mem_alloc(sizeof(LunaGpuApi), 8);
        if (lunaGpuApi == NULL) {
            r3_log_stdout(ERROR_LOG, "[LunaOpenGL] failed to allocate lunaGpuApi dispatch table!\n");
            return 0;
        }
    }

    if (lunaOpenGL == NULL) {
        lunaOpenGL = r3_mem_alloc(sizeof(LunaOpenGL), 8);
        if (lunaOpenGL == NULL) {
            r3_log_stdout(ERROR_LOG, "[LunaOpenGL] failed to allocate lunaOpenGL dispatch table!\n");
            r3_mem_dealloc(lunaGpuApi);
            return 0;
        }
    }

    if (!lunaPlatformApi->createGLContext()) {
        r3_log_stdout(ERROR_LOG, "[LunaOpenGL] failed to create opengl context\n");
        return 0;
    } else r3_log_stdout(SUCCESS_LOG, "[LunaOpenGL] created opengl context\n");
    
    // glapi
    LunaLibrary opengl32;
    if (!lunaPlatformApi->loadLibrary(NULL, "opengl32", &opengl32)) {
        r3_log_stdout(ERROR_LOG, "[LunaOpenGL] failed to load opengl\n");
        return 0;
    }  else r3_log_stdoutf(SUCCESS_LOG, "[LunaOpenGL] loaded opengl v%s %s %s\n", glGetString(GL_VERSION), glGetString(GL_VENDOR), glGetString(GL_RENDERER));

    struct gl_func {
        ptr* function;
        str name;
    } functions[] = {
        // BUFFER FUNCTIONS
        {(ptr*)&lunaOpenGL->glGenBuffers, "glGenBuffers"},
        {(ptr*)&lunaOpenGL->glBindBuffer, "glBindBuffer"},
        {(ptr*)&lunaOpenGL->glBufferData, "glBufferData"},
        {(ptr*)&lunaOpenGL->glMapBuffer, "glMapBuffer"},
        {(ptr*)&lunaOpenGL->glUnmapBuffer, "glUnmapBuffer"},
        {(ptr*)&lunaOpenGL->glBufferSubData, "glBufferSubData"},
        {(ptr*)&lunaOpenGL->glGetBufferParameteriv, "glGetBufferParameteriv"},
        {(ptr*)&lunaOpenGL->glDeleteBuffers, "glDeleteBuffers"},

        // VAO FUNCTIONS
        {(ptr*)&lunaOpenGL->glGenVertexArrays, "glGenVertexArrays"},
        {(ptr*)&lunaOpenGL->glBindVertexArray, "glBindVertexArray"},
        {(ptr*)&lunaOpenGL->glDeleteVertexArrays, "glDeleteVertexArrays"},
        {(ptr*)&lunaOpenGL->glEnableVertexAttribArray, "glEnableVertexAttribArray"},
        {(ptr*)&lunaOpenGL->glDisableVertexAttribArray, "glDisableVertexAttribArray"},
        {(ptr*)&lunaOpenGL->glVertexAttribPointer, "glVertexAttribPointer"},

        // SHADER FUNCTIONS
        {(ptr*)&lunaOpenGL->glCreateShader, "glCreateShader"},
        {(ptr*)&lunaOpenGL->glShaderSource, "glShaderSource"},
        {(ptr*)&lunaOpenGL->glCompileShader, "glCompileShader"},
        {(ptr*)&lunaOpenGL->glDeleteShader, "glDeleteShader"},
        {(ptr*)&lunaOpenGL->glGetShaderiv, "glGetShaderiv"},
        {(ptr*)&lunaOpenGL->glGetShaderInfoLog, "glGetShaderInfoLog"},

        {(ptr*)&lunaOpenGL->glCreateProgram, "glCreateProgram"},
        {(ptr*)&lunaOpenGL->glAttachShader, "glAttachShader"},
        {(ptr*)&lunaOpenGL->glDetachShader, "glDetachShader"},
        {(ptr*)&lunaOpenGL->glLinkProgram, "glLinkProgram"},
        {(ptr*)&lunaOpenGL->glUseProgram, "glUseProgram"},
        {(ptr*)&lunaOpenGL->glDeleteProgram, "glDeleteProgram"},
        {(ptr*)&lunaOpenGL->glGetProgramiv, "glGetProgramiv"},
        {(ptr*)&lunaOpenGL->glGetProgramInfoLog, "glGetProgramInfoLog"},
        {(ptr*)&lunaOpenGL->glGetUniformLocation, "glGetUniformLocation"},

        // UNIFORMS
        {(ptr*)&lunaOpenGL->glUniform1i, "glUniform1i"},
        {(ptr*)&lunaOpenGL->glUniform1f, "glUniform1f"},
        {(ptr*)&lunaOpenGL->glUniform2fv, "glUniform2fv"},
        {(ptr*)&lunaOpenGL->glUniform3fv, "glUniform3fv"},
        {(ptr*)&lunaOpenGL->glUniform4fv, "glUniform4fv"},
        {(ptr*)&lunaOpenGL->glUniformMatrix4fv, "glUniformMatrix4fv"},

        // TEXTURES
        {(ptr*)&lunaOpenGL->glGenTextures, "glGenTextures"},
        {(ptr*)&lunaOpenGL->glBindTexture, "glBindTexture"},
        {(ptr*)&lunaOpenGL->glTexParameteri, "glTexParameteri"},
        {(ptr*)&lunaOpenGL->glTexImage2D, "glTexImage2D"},
        {(ptr*)&lunaOpenGL->glActiveTexture, "glActiveTexture"},
        {(ptr*)&lunaOpenGL->glDeleteTextures, "glDeleteTextures"},
        {(ptr*)&lunaOpenGL->glGenerateMipmap, "glGenerateMipmap"},

        // FRAMEBUFFERS + RENDERBUFFERS
        {(ptr*)&lunaOpenGL->glGenFramebuffers, "glGenFramebuffers"},
        {(ptr*)&lunaOpenGL->glBindFramebuffer, "glBindFramebuffer"},
        {(ptr*)&lunaOpenGL->glFramebufferTexture2D, "glFramebufferTexture2D"},
        {(ptr*)&lunaOpenGL->glFramebufferRenderbuffer, "glFramebufferRenderbuffer"},
        {(ptr*)&lunaOpenGL->glCheckFramebufferStatus, "glCheckFramebufferStatus"},
        {(ptr*)&lunaOpenGL->glDeleteFramebuffers, "glDeleteFramebuffers"},

        {(ptr*)&lunaOpenGL->glGenRenderbuffers, "glGenRenderbuffers"},
        {(ptr*)&lunaOpenGL->glBindRenderbuffer, "glBindRenderbuffer"},
        {(ptr*)&lunaOpenGL->glRenderbufferStorage, "glRenderbufferStorage"},
        {(ptr*)&lunaOpenGL->glDeleteRenderbuffers, "glDeleteRenderbuffers"},

        // DRAWING
        {(ptr*)&lunaOpenGL->glDrawArrays, "glDrawArrays"},
        {(ptr*)&lunaOpenGL->glDrawElements, "glDrawElements"},

        // STATE
        {(ptr*)&lunaOpenGL->glEnable, "glEnable"},
        {(ptr*)&lunaOpenGL->glDisable, "glDisable"},
        {(ptr*)&lunaOpenGL->glBlendFunc, "glBlendFunc"},
        {(ptr*)&lunaOpenGL->glCullFace, "glCullFace"},
        {(ptr*)&lunaOpenGL->glDepthFunc, "glDepthFunc"},
        {(ptr*)&lunaOpenGL->glViewport, "glViewport"},
        {(ptr*)&lunaOpenGL->glPolygonMode, "glPolygonMode"},
        {(ptr*)&lunaOpenGL->glClear, "glClear"},
        {(ptr*)&lunaOpenGL->glClearColor, "glClearColor"},
        {(ptr*)&lunaOpenGL->glClearDepth, "glClearDepth"},

        // INFO / DEBUG
        {(ptr*)&lunaOpenGL->glGetError, "glGetError"},
        {(ptr*)&lunaOpenGL->glGetString, "glGetString"}
    };

    FOR_I(0, sizeof(functions) / sizeof(functions[0]), 1) {
        if (!lunaPlatformApi->loadLibrarySymbol(functions[i].name, functions[i].function, &opengl32) || *functions[i].function == NULL) {
            r3_log_stdoutf(WARN_LOG, "[LunaOpenGL] failed to load function: (name)%s\n", functions[i].name);
        } else r3_log_stdoutf(SUCCESS_LOG, "[LunaOpenGL] loaded function: (name)%s (ptr)%p\n", functions[i].name, *(functions[i].function));
    }

    lunaPlatformApi->unloadLibrary(&opengl32);

    // gpuapi
    lunaGpuApi->clearDepthBuffer = glClearDepthBufferImpl;
    lunaGpuApi->clearColorBuffer = glClearColorBufferImpl;
    
    lunaGpuApi->createProgram = glCreateProgramImpl;
    lunaGpuApi->destroyProgram = glDestroyProgramImpl;
    
    lunaGpuApi->sendUniform = glSendUniformImpl;
    lunaGpuApi->setUniform = glSetUniformImpl;
    
    lunaGpuApi->createVertexBuffer = glCreateVertexBufferImpl;
    lunaGpuApi->destroyVertexBuffer = glDestroyVertexBufferImpl;
    
    lunaGpuApi->createElementBuffer = glCreateElementBufferImpl;
    lunaGpuApi->destroyElementBuffer = glDestroyElementBufferImpl;
    
    lunaGpuApi->createTextureBuffer = glCreateTextureBufferImpl;
    lunaGpuApi->destroyTextureBuffer = glDestroyTextureBufferImpl;
    
    lunaGpuApi->createFrameBuffer = glCreateFrameBufferImpl;
    lunaGpuApi->destroyFrameBuffer = glDestroyFrameBufferImpl;
    
    lunaGpuApi->bindBuffer = glBindBufferImpl;
    lunaGpuApi->readBuffer = glReadBufferImpl;
    lunaGpuApi->writeBuffer = glWriteBufferImpl;
    lunaGpuApi->bindProgram = glBindProgramImpl;

    r3_log_stdout(SUCCESS_LOG, "[LunaOpenGL] initialized LunaOpenGL + LunaGpuApi\n");
    return 1;
}

u8 lunaDeinitOpenGL(none) {
    if (lunaGpuApi != NULL) {
        r3_mem_dealloc(lunaGpuApi);
        lunaGpuApi = NULL;
    }
    
    if (lunaOpenGL != NULL) {
        r3_mem_dealloc(lunaOpenGL);
        lunaOpenGL = NULL;
    }

    lunaPlatformApi->destroyGLContext();

    r3_log_stdout(SUCCESS_LOG, "[LunaOpenGL] deinitialized LunaOpenGL + LunaGpuApi\n");
    return 1;
}
