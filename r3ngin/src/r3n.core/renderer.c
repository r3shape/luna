#include <r3ngin/include/r3n.core/renderer.h>
#include <r3ngin/include/r3n.core/platform.h>
#include <r3ngin/include/r3n.core/api/gl.h>
#include <r3ngin/include/r3n.core/api/gpu.h>

static struct R3N_Renderer_Internal {
    R3N_GPU_Backend backend;
    
    // renderer resource state + metadata
    R3N_GPU_Pipeline pipelinev[R3N_GPU_RESOURCE_MAX];
    R3N_GPU_Program programv[R3N_GPU_RESOURCE_MAX];
    R3N_GPU_Buffer bufferv[R3N_GPU_RESOURCE_MAX];
    R3N_GPU_Frame frame;
    u32 pipelines;
    u32 programs;
    u32 buffers;
    u32 frames;
    Mat4 projection;
} R3N_Renderer_Internal = {0};

static f32 R3N_Screen_Quad_Vertices[] = {
    // location         // tcoord
    -1.0,  1.0, 0.0,    0.0, 1.0,
    -1.0, -1.0, 0.0,    0.0, 0.0,
    1.0,  -1.0, 0.0,    1.0, 0.0,

    -1.0,  1.0, 0.0,    0.0, 1.0,
     1.0, -1.0, 0.0,    1.0, 0.0,
     1.0,  1.0, 0.0,    1.0, 1.0
};
static R3N_GPU_Handle R3N_Screen_Quad_Buffer_Handle = R3N_GPU_RESOURCE_MAX;
static R3N_GPU_Handle R3N_Screen_Quad_Program_Handle = R3N_GPU_RESOURCE_MAX;

// global dispatch table ptr
R3N_Renderer* R3N_Renderer_Ptr = NULL;


R3N_GPU_Handle create_frame_impl(none) {
    R3N_Renderer_Internal.frame.handle = R3N_Renderer_Internal.frames++;
    R3N_Renderer_Internal.frame.phases = 0;
    R3N_Renderer_Internal.frame.calls = 0;
    
    r3_arena_reset(&R3N_Renderer_Internal.frame.arena);
    if (!r3_mem_set(sizeof(R3N_Renderer_Internal.frame.callv), 0, &R3N_Renderer_Internal.frame.callv) ||
        !r3_mem_set(sizeof(R3N_Renderer_Internal.frame.phasev), 0, &R3N_Renderer_Internal.frame.phasev)) {
            r3_log_stdout(WARN_LOG, "[R3N_Renderer] failed to zero frame resource arrays!\n");
    }

    if (!R3N_Renderer_Internal.frame.buffer || R3N_Renderer_Internal.frame.buffer >= R3N_Renderer_Internal.buffers || R3N_Renderer_Internal.frame.buffer >= R3N_GPU_RESOURCE_MAX) {
        R3N_Renderer_Internal.frame.buffer = R3N_Renderer_Ptr->create_buffer((R3N_GPU_Buffer){ .type = R3N_BUFFER_FRAME });
    }

    return R3N_Renderer_Internal.frame.handle;
}


R3N_GPU_Handle create_call_impl(R3N_GPU_Call call) {
    if ((R3N_Renderer_Internal.frame.calls + 1) > R3N_GPU_RESOURCE_MAX) {
        r3_log_stdoutf(WARN_LOG, "[R3N_Renderer] gpu call max reached: (frame)%d\n", R3N_Renderer_Internal.frame.handle);
        return I32_MAX;
    }

    if (call.uniforms > R3N_GPU_CALL_UNIFORM_MAX) {
        r3_log_stdoutf(WARN_LOG, "[R3N_Renderer] gpu call with invalid number of gpu uniforms: (uniforms)%d (max)%d\n", call.uniforms, R3N_GPU_CALL_UNIFORM_MAX);
        return I32_MAX;
    }

    if (call.phase >= I32_MAX) {
        r3_log_stdoutf(WARN_LOG, "[R3N_Renderer] gpu call to invalid gpu phase: (phase)%d\n", call.phase);
        return I32_MAX;
    }
    
    if (call.pipeline >= I32_MAX) {
        r3_log_stdoutf(WARN_LOG, "[R3N_Renderer] gpu call to invalid gpu pipeline: (pipeline)%d\n", call.pipeline);
        return I32_MAX;
    }
    
    R3N_GPU_Handle handle = R3N_Renderer_Internal.frame.calls++;
    R3N_GPU_Call* call_ptr = &R3N_Renderer_Internal.frame.callv[handle];
    call_ptr->handle = handle;
    *call_ptr = call;

    R3N_GPU_Pipeline* pipeline_ptr = &R3N_Renderer_Internal.pipelinev[call_ptr->pipeline];
    R3N_GPU_Program* program_ptr = &R3N_Renderer_Internal.programv[pipeline_ptr->program];
    
    FOR(u8, uniform, 0, call_ptr->uniforms, 1) {
        R3N_GPU_Ptr->set_uniform(&call_ptr->uniformv[uniform], program_ptr);
    }

    r3_log_stdoutf(INFO_LOG, "[R3N_Renderer] created gpu call: (handle)%d (phase)%d (pipeline)%d (vertex buffer)%d (element buffer)%d (uniforms)%d\n", R3N_Renderer_Internal.frame.calls, pipeline_ptr->handle, call_ptr->phase, call_ptr->vertex_buffer, call_ptr->element_buffer, call_ptr->uniforms);

    return handle;
}


R3N_GPU_Handle create_phase_impl(R3N_GPU_Phase phase) {
    if ((R3N_Renderer_Internal.frame.phases + 1) > R3N_GPU_RESOURCE_MAX) {
        r3_log_stdoutf(WARN_LOG, "[R3N_Renderer] gpu phase max reached: (frame)%d\n", R3N_Renderer_Internal.frame.handle);
        return I32_MAX;
    }

    if (phase.type >= I32_MAX || phase.type >= R3N_PHASE_TYPES) {
        r3_log_stdoutf(ERROR_LOG, "[R3N_Renderer] invalid phase type: (type)%d\n", phase.type);
        return I32_MAX;
    }
    
    if (phase.uniforms > R3N_GPU_PHASE_UNIFORM_MAX) {
        r3_log_stdoutf(WARN_LOG, "[R3N_Renderer] gpu phase with invalid number of gpu uniforms: (uniforms)%d (max)%d\n", phase.uniforms, R3N_GPU_PHASE_UNIFORM_MAX);
        return I32_MAX;
    }

    R3N_GPU_Handle handle = R3N_Renderer_Internal.frame.phases++;
    R3N_GPU_Phase* phase_ptr = &R3N_Renderer_Internal.frame.phasev[handle];
    
    phase_ptr->handle = handle;
    phase_ptr->writes = 0;
    phase_ptr->reads = 0;
    *phase_ptr = phase;

    if (!r3_mem_set(sizeof(R3N_GPU_Handle) * 8, 0, phase_ptr->readv) ||
        !r3_mem_set(sizeof(R3N_GPU_Handle) * 8, 0, phase_ptr->writev)) {
            r3_log_stdout(WARN_LOG, "[R3N_Renderer] failed to zero phase dependency arrays!\n");
    }
    
    switch (phase.type) {
        case (R3N_PHASE_DEPTH): {
            phase_ptr->depth.clear_depth = phase.depth.clear_depth;
        } break;
        case (R3N_PHASE_OPAQUE): {
            phase_ptr->opaque.clear_color = phase.opaque.clear_color;
        } break;
        case (R3N_PHASE_LIGHT):    // fall-through
        case (R3N_PHASE_SHADOW):   // fall-through
        case (R3N_PHASE_INVALID):  // fall-through
        case (R3N_PHASE_TYPES):    // fall-through
        default: return I32_MAX;
    }

    r3_log_stdoutf(INFO_LOG, "[R3N_Renderer] created gpu phase: (handle)%d (type)%d (uniforms)%d\n", phase_ptr->handle, phase_ptr->type, phase_ptr->uniforms);
    return handle;
}

none destroy_phase_impl(R3N_GPU_Handle phase) {
    if (phase >= R3N_Renderer_Internal.frame.phases || phase >= R3N_GPU_RESOURCE_MAX) {
        r3_log_stdoutf(ERROR_LOG, "[R3N_Renderer] invalid phase handle passed to destroyPhase(): (handle)%d\n", phase);
        return;
    }

    R3N_GPU_Phase* phase_ptr = &R3N_Renderer_Internal.frame.phasev[phase];
    if (!r3_mem_set(sizeof(R3N_GPU_Phase), 0, phase_ptr)) {
        r3_log_stdoutf(ERROR_LOG, "[R3N_Renderer] failed to destroy phase: (handle)%d\n", phase);
        return;
    } else { r3_log_stdoutf(ERROR_LOG, "[R3N_Renderer] destroyed phase: (handle)%d\n", phase); }

    return;
}


R3N_GPU_Handle create_buffer_impl(R3N_GPU_Buffer buffer) {
    if (buffer.type >= I32_MAX || buffer.type >= R3N_BUFFER_TYPES) {
        r3_log_stdoutf(ERROR_LOG, "[R3N_Renderer] invalid buffer type: (type)%d\n", buffer.type);
        return I32_MAX;
    }

    R3N_GPU_Handle handle = R3N_Renderer_Internal.buffers++;
    R3N_GPU_Buffer* buf = &R3N_Renderer_Internal.bufferv[handle];

    buf->type = buffer.type;
    buf->handle = handle;

    switch (buffer.type) {
        case (R3N_BUFFER_VERTEX): {
            if (!buffer.vertex.size) {
                r3_log_stdoutf(ERROR_LOG, "[R3N_Renderer] invalid vertex buffer size: (type)%d\n", buffer.vertex.size);
                return 0;
            }
            buf->vertex.size = buffer.vertex.size;
            buf->vertex.vertices = buffer.vertex.size / sizeof(f32);
            buf->vertex.vertexv = buffer.vertex.vertexv;
            buf->vertex.attribs = buffer.vertex.attribs;
            buf->vertex.vao = 0;
            buf->vertex.vbo = 0;

            R3N_GPU_Ptr->create_vertex_buffer(buf);

            r3_log_stdoutf(SUCCESS_LOG, "[R3N_Renderer] created vertex buffer: (vbo)%d (vao)%d (vertices)%d\n", buf->vertex.vbo, buf->vertex.vao, buf->vertex.vertices);
            return buf->handle;
        }
        case (R3N_BUFFER_ELEMENT): {
            if (!buffer.element.size) {
                r3_log_stdoutf(ERROR_LOG, "[R3N_Renderer] invalid element buffer size: (type)%d\n", buffer.element.size);
                return 0;
            }

            buf->element.elements = buffer.element.elements;
            buf->element.elementv = buffer.element.elementv;
            buf->element.ebo = 0;

            R3N_GPU_Ptr->create_element_buffer(buf);
            
            r3_log_stdoutf(SUCCESS_LOG, "[R3N_Renderer] created element buffer: (ebo)%d (elements)%d\n", buf->element.ebo, buf->element.elements);
            return buf->handle;
        }
        case (R3N_BUFFER_FRAME): {
            R3N_GPU_Ptr->create_frame_buffer(buf);

            r3_log_stdoutf(SUCCESS_LOG, "[R3N_Renderer] created frame buffer: (frame buffer object)%d (texture buffer object)%d (render buffer object) %d\n",
                buf->frame.fbo, buf->frame.tbo, buf->frame.rbo);
            return buf->handle;
        }
        case (R3N_BUFFER_TEXTURE): // fall-through
        default: break;
    }

    return I32_MAX;
}

none destroy_buffer_impl(R3N_GPU_Handle buffer) {
    if (buffer >= R3N_Renderer_Internal.buffers || buffer >= R3N_GPU_RESOURCE_MAX) {
        r3_log_stdoutf(ERROR_LOG, "[R3N_Renderer] invalid buffer handle passed to destroyBuffer(): (handle)%d\n", buffer);
        return;
    }

    R3N_GPU_Buffer* buffer_ptr = &R3N_Renderer_Internal.bufferv[buffer];

    switch (buffer_ptr->type) {
        case (R3N_BUFFER_VERTEX):  { R3N_GPU_Ptr->destroy_vertex_buffer(buffer_ptr); }    break;
        case (R3N_BUFFER_ELEMENT): { R3N_GPU_Ptr->destroy_element_buffer(buffer_ptr); }   break;
        case (R3N_BUFFER_TEXTURE): { R3N_GPU_Ptr->destroy_texture_buffer(buffer_ptr); }   break;
        case (R3N_BUFFER_FRAME):   { R3N_GPU_Ptr->destroy_frame_buffer(buffer_ptr); }     break;
        default: break;
    }

    if (!r3_mem_set(sizeof(R3N_GPU_Buffer), 0, buffer_ptr)) {
        r3_log_stdoutf(ERROR_LOG, "[R3N_Renderer] failed to destroy buffer: (handle)%d\n", buffer);
        return;
    } else { r3_log_stdoutf(ERROR_LOG, "[R3N_Renderer] destroyed buffer: (handle)%d\n", buffer); }

    return;
}


R3N_GPU_Handle create_program_impl(str vertex_path, str fragment_path) {

    R3N_GPU_Handle handle = R3N_Renderer_Internal.programs++;
    R3N_GPU_Program* program = &R3N_Renderer_Internal.programv[handle];

    // allocate vertex buffer
    if (!r3_buf_alloc(R3N_GPU_PROGRAM_BUFFER_SIZE, &program->vertex_buffer)) {
        r3_log_stdout(ERROR_LOG, "[R3N_Renderer] failed to allocate vertex program buffer\n");
        return I32_MAX;
    } else { r3_log_stdoutf(INFO_LOG, "[R3N_Renderer] allocated vertex program buffer: (size)%d\n", R3N_GPU_PROGRAM_BUFFER_SIZE); }

    // load vertex buffer
    if (!r3_buf_load(vertex_path, &program->vertex_buffer)) {
        r3_log_stdout(ERROR_LOG, "[R3N_Renderer] failed to load vertex program buffer\n");
        r3_buf_dealloc(&program->vertex_buffer);
        return I32_MAX;
    } else {
        u16 wrote = r3_buf_field(BUFFER_WROTE_FIELD, &program->vertex_buffer);
        u16 writes = r3_buf_field(BUFFER_WRITES_FIELD, &program->vertex_buffer);
        r3_buf_rewind(&program->vertex_buffer);
        ((char*)program->vertex_buffer.data)[r3_buf_field(BUFFER_WROTE_FIELD, &program->vertex_buffer)] = '\0';
        r3_log_stdoutf(INFO_LOG, "[R3N_Renderer] loaded vertex buffer: (wrote)%d (writes)%d \n(source)\n%s\n\n", wrote, writes, (cstr)program->vertex_buffer.data);
    }
    
    // allocate fragment buffer
    if (!r3_buf_alloc(R3N_GPU_PROGRAM_BUFFER_SIZE, &program->fragment_buffer)) {
        r3_log_stdout(ERROR_LOG, "[R3N_Renderer] failed to allocate fragment program buffer\n");
        r3_buf_dealloc(&program->vertex_buffer);
        return I32_MAX;
    } else { r3_log_stdoutf(INFO_LOG, "[R3N_Renderer] allocated fragment program buffer: (size)%d\n", R3N_GPU_PROGRAM_BUFFER_SIZE); }
    
    // load fragment buffer
    if (!r3_buf_load(fragment_path, &program->fragment_buffer)) {
        r3_log_stdout(ERROR_LOG, "[R3N_Renderer] failed to load fragment program buffer\n");
        r3_buf_dealloc(&program->vertex_buffer);
        r3_buf_dealloc(&program->fragment_buffer);
        return I32_MAX;
    } else {
        u16 wrote = r3_buf_field(BUFFER_WROTE_FIELD, &program->fragment_buffer);
        u16 writes = r3_buf_field(BUFFER_WRITES_FIELD, &program->fragment_buffer);
        r3_buf_rewind(&program->fragment_buffer);
        ((char*)program->fragment_buffer.data)[r3_buf_field(BUFFER_WROTE_FIELD, &program->fragment_buffer)] = '\0';
        r3_log_stdoutf(INFO_LOG, "[R3N_Renderer] loaded fragment buffer: (wrote)%d (writes)%d \n(source)\n%s\n\n", wrote, writes, (cstr)program->fragment_buffer.data);
    }

    // create gpu api program
    if (!R3N_GPU_Ptr->create_program(program) || !program->program) {
        r3_log_stdout(ERROR_LOG, "[R3N_Renderer] failed to create gpu program\n");
        r3_buf_dealloc(&program->vertex_buffer);
        r3_buf_dealloc(&program->fragment_buffer);
        r3_arr_hashed_dealloc(&program->uniformv);
        return I32_MAX;
    }

    // allocate uniform array
    if (!r3_arr_hashed_alloc(16, sizeof(R3N_GPU_Uniform), &program->uniformv)) {
        r3_log_stdout(ERROR_LOG, "[R3N_Renderer] failed to allocate program uniform array\n");
        r3_buf_dealloc(&program->vertex_buffer);
        r3_buf_dealloc(&program->fragment_buffer);
        return I32_MAX;
    } else { r3_log_stdout(INFO_LOG, "[R3N_Renderer] allocated program uniform array\n"); }

    program->handle = handle;

    r3_log_stdoutf(INFO_LOG, "[R3N_Renderer] created program: (handle)%d (program)%d\n", handle, program->program);
    return handle;
}

none destroy_program_impl(R3N_GPU_Handle program) {
    if (program >= R3N_Renderer_Internal.programs || program >= R3N_GPU_RESOURCE_MAX) {
        r3_log_stdoutf(ERROR_LOG, "[R3N_Renderer] invalid program handle passed to destroyProgram(): (handle)%d\n", program);
        return;
    }

    R3N_GPU_Program* program_ptr = &R3N_Renderer_Internal.programv[program];
    if (!R3N_GPU_Ptr->destroy_program(program_ptr) || !r3_mem_set(sizeof(R3N_GPU_Program), 0, program_ptr)) {
        r3_log_stdoutf(ERROR_LOG, "[R3N_Renderer] failed to destroy program: (handle)%d\n", program);
        return;
    } else { r3_log_stdoutf(ERROR_LOG, "[R3N_Renderer] destroyed program: (handle)%d\n", program); }

    return;
}


R3N_GPU_Handle create_pipeline_impl(R3N_GPU_Pipeline pipeline) {
    if ((R3N_Renderer_Internal.pipelines + 1) > R3N_GPU_RESOURCE_MAX) {
        r3_log_stdoutf(WARN_LOG, "[R3N_Renderer] gpu pipeline max reached: (frame)%d\n", R3N_Renderer_Internal.frame.handle);
        return I32_MAX;
    }

    if (pipeline.uniforms > R3N_GPU_PIPELINE_UNIFORM_MAX) {
        r3_log_stdoutf(WARN_LOG, "[R3N_Renderer] gpu pipeline with invalid number of gpu uniforms: (uniforms)%d (max)%d\n", pipeline.uniforms, R3N_GPU_PIPELINE_UNIFORM_MAX);
        return I32_MAX;
    }

    R3N_GPU_Handle handle = R3N_Renderer_Internal.pipelines++;
    R3N_GPU_Pipeline* pipeline_ptr = &R3N_Renderer_Internal.pipelinev[handle];
    pipeline_ptr->handle = handle;
    *pipeline_ptr = pipeline;

    R3N_GPU_Program* program_ptr = &R3N_Renderer_Internal.programv[pipeline_ptr->program];
    FOR(u8, uniform, 0, pipeline_ptr->uniforms, 1) {
        R3N_GPU_Ptr->set_uniform(&pipeline_ptr->uniformv[uniform], program_ptr);
    }

    r3_log_stdoutf(INFO_LOG, "[R3N_Renderer] created gpu pipeline: (handle)%d (program)%d (uniforms)%d (binds)%d\n", pipeline_ptr->handle, pipeline_ptr->program, pipeline_ptr->uniforms, pipeline_ptr->binds);
    return handle;
}

none destroy_pipeline_impl(R3N_GPU_Handle pipeline) {
    if (pipeline >= R3N_Renderer_Internal.pipelines || pipeline >= R3N_GPU_RESOURCE_MAX) {
        r3_log_stdoutf(ERROR_LOG, "[R3N_Renderer] invalid pipeline handle passed to destroyPipeline(): (handle)%d\n", pipeline);
        return;
    }

    R3N_GPU_Pipeline* pipeline_ptr = &R3N_Renderer_Internal.pipelinev[pipeline];

    // TODO: implement this function (R3N_GPU_Ptr->unbind_program) !!!
    // R3N_GPU_Ptr->unbind_program(&R3N_Renderer_Internal.programv[pipeline_ptr->program])

    // TODO: implement this function (R3N_GPU_Ptr->unbind_buffer) !!!
    // FOR(u32, b, 0, pipeline_ptr->binds, 1) {
    //     R3N_GPU_Ptr->unbind_buffer(&R3N_Renderer_Internal.bufferv[pipeline_ptr->bindv[b]]);
    // }

    if (!r3_mem_set(sizeof(R3N_GPU_Pipeline), 0, pipeline_ptr)) {
        r3_log_stdoutf(ERROR_LOG, "[R3N_Renderer] failed to destroy pipeline: (handle)%d\n", pipeline);
        return;
    } else { r3_log_stdoutf(ERROR_LOG, "[R3N_Renderer] destroyed pipeline: (handle)%d\n", pipeline); }

    return;
}


none render_impl(none) {
    // lazy load screen quad
    if (R3N_Screen_Quad_Buffer_Handle == R3N_GPU_RESOURCE_MAX) {
        R3N_Screen_Quad_Buffer_Handle = R3N_Renderer_Ptr->create_buffer((R3N_GPU_Buffer){
            .type = R3N_BUFFER_VERTEX,
            .vertex.vertices = 6,
            .vertex.vertexv = R3N_Screen_Quad_Vertices,
            .vertex.size = sizeof(R3N_Screen_Quad_Vertices),
            .vertex.attribs = R3N_VERTEX_LOCATION_ATTRIBUTE | R3N_VERTEX_TEXTURE_ATTRIBUTE
        });
        R3N_Screen_Quad_Program_Handle = R3N_Renderer_Ptr->create_program(R3N_SCREEN_QUAD_VERTEX_PROGRAM, R3N_SCREEN_QUAD_FRAGMENT_PROGRAM);
        r3_log_stdoutf(INFO_LOG, "[R3N_Renderer] loaded screen quad buffer: (buffer handle)%d (program handle)%d\n", R3N_Screen_Quad_Buffer_Handle, R3N_Screen_Quad_Program_Handle);
    }
    
    r3_log_stdoutf(INFO_LOG, "[R3N_Renderer] begin frame: (handle)%d (calls)%d (phases)%d\n", R3N_Renderer_Internal.frame.handle, R3N_Renderer_Internal.frame.calls, R3N_Renderer_Internal.frame.phases);

    // default clear color
    Vec4 clear_color = (Vec4){ .data={150, 50, 100, 255} };

    // retrieve current frame
    R3N_GPU_Frame* frame = &R3N_Renderer_Internal.frame;
    
    // retrieve current frame buffer
    R3N_GPU_Buffer* frame_buffer = &R3N_Renderer_Internal.bufferv[frame->buffer];
    FOR(u32, p, 0, R3N_Renderer_Internal.frame.phases, 1) {
        R3N_GPU_Phase* phase = &frame->phasev[p];
        r3_log_stdoutf(INFO_LOG, "[R3N_Renderer] begin gpu phase: (handle)%d (type)%d (uniforms)%d\n", phase->handle, phase->type, phase->uniforms);

        // bind frame buffer
        R3N_GPU_Ptr->bind_buffer(frame_buffer);

        // clear color/depth buffers
        if (phase->type == R3N_PHASE_DEPTH) {
            if (R3N_Renderer_Internal.backend == R3N_BACKEND_OPENGL) glEnable(GL_DEPTH_TEST);
            R3N_GPU_Ptr->clear_depth_buffer(phase->depth.clear_depth);
            r3_log_stdoutf(INFO_LOG, "[R3N_Renderer] clear depth: %0.3f\n", phase->depth.clear_depth);
        } else {
            clear_color = phase->opaque.clear_color;
            R3N_GPU_Ptr->clear_color_buffer(phase->opaque.clear_color);
            r3_vec4_log("[R3N_Renderer] clear color:", phase->opaque.clear_color);
        }

        FOR(u32, c, 0, frame->calls, 1) {
            R3N_GPU_Call* call = &frame->callv[c];
            if (call->phase != phase->handle) continue;
            r3_log_stdoutf(INFO_LOG, "[R3N_Renderer] begin call: (phase)%d (pipeline)%d (uniforms)%d (vertex buffer)%d (element buffer)%d\n", call->phase, call->pipeline, call->uniforms, call->vertex_buffer, call->element_buffer);

            // retrieve gpu call program pipeline
            R3N_GPU_Pipeline* pipeline = &R3N_Renderer_Internal.pipelinev[call->pipeline];

            // retrieve pipeline program
            R3N_GPU_Program* program = &R3N_Renderer_Internal.programv[pipeline->program];

            // bind gpu pipeline program
            R3N_GPU_Ptr->bind_program(program);

            // bind gpu pipeline buffers
            FOR(u32, b, 0, pipeline->binds, 1) {
                R3N_GPU_Ptr->bind_buffer(&R3N_Renderer_Internal.bufferv[pipeline->bindv[b]]);
            }
            
            // send per-pipeline uniforms
            FOR(u32, pu, 0, pipeline->uniforms, 1) {
                R3N_GPU_Ptr->send_uniform(pipeline->uniformv[pu].name, program);
            }
            
            // set + send per-phase uniforms
            FOR(u32, phu, 0, phase->uniforms, 1) {
                R3N_GPU_Ptr->set_uniform(&phase->uniformv[phu], program);
                R3N_GPU_Ptr->send_uniform(phase->uniformv[phu].name, program);
            }

            // send per-call uniforms
            FOR(u32, cu, 0, call->uniforms, 1) {
                R3N_GPU_Ptr->send_uniform(call->uniformv[cu].name, program);
            }

            // retrieve gpu call vertex buffer
            R3N_GPU_Buffer* vertex_buffer = NULL;
            if (call->vertex_buffer < R3N_Renderer_Internal.buffers && call->vertex_buffer <= R3N_GPU_RESOURCE_MAX) {
                vertex_buffer = &R3N_Renderer_Internal.bufferv[call->vertex_buffer];
            } else { r3_log_stdoutf(WARN_LOG, "[R3N_Renderer] invalid vertex buffer handle assigned to call: (frame)%d (call handle)%d (vertex buffer)%d\n", frame->handle, call->handle, call->vertex_buffer); }
            
            // retrieve gpu call element buffer
            R3N_GPU_Buffer* element_buffer = NULL;
            if (call->element_buffer < R3N_Renderer_Internal.buffers && call->element_buffer <= R3N_GPU_RESOURCE_MAX) {
                element_buffer = &R3N_Renderer_Internal.bufferv[call->element_buffer];
            } else { r3_log_stdoutf(WARN_LOG, "[R3N_Renderer] invalid element buffer handle assigned to call: (frame)%d (call handle)%d (element buffer)%d\n", frame->handle, call->handle, call->element_buffer); }


            // TODO: increment phase buffer reads + append handle to readv array
            // bind + read vertex/element buffers
            R3N_GPU_Ptr->bind_buffer(vertex_buffer);
            if (element_buffer != NULL && element_buffer->element.elementv != NULL && element_buffer->element.elements != 0) {
                R3N_GPU_Ptr->read_buffer(element_buffer);
            } else R3N_GPU_Ptr->read_buffer(vertex_buffer);

            r3_log_stdoutf(INFO_LOG, "[R3N_Renderer] end gpu call: (phase)%d (pipeline)%d (uniforms)%d (vertex buffer)%d (element buffer)%d\n", call->phase, call->pipeline, call->uniforms, call->vertex_buffer, call->element_buffer);
        }

        // GL frame buffer handling
        // stack alloc'd resources are safe here -- reclaimed by OS on iteration
        if (R3N_Renderer_Internal.backend == R3N_BACKEND_OPENGL){
            // bind default frame buffer
            R3N_GPU_Ptr->bind_buffer(&(R3N_GPU_Buffer){
                .type = R3N_BUFFER_FRAME,
                .frame.fbo = 0
            });

            // preserve screen-space quad from depth-test discard
            glDisable(GL_DEPTH_TEST);

            R3N_GPU_Ptr->clear_color_buffer(clear_color);
            r3_vec4_log("[R3N_Renderer] clear color:", clear_color);

            // TODO: frame buffer could own its color buffer and render buffer structures rather than lazy alloc here
            // bind texture buffer containing frame buffer's color buffer attachment
            R3N_GPU_Ptr->bind_buffer(&(R3N_GPU_Buffer){
                .type = R3N_BUFFER_TEXTURE,
                .texture.slot = 0,
                .texture. type = R3N_TEXTURE_2D,
                .texture.tbo = frame_buffer->frame.tbo
            });

            // render screen quad sampling frame buffer's color buffer
            R3N_GPU_Ptr->bind_program(&R3N_Renderer_Internal.programv[R3N_Screen_Quad_Program_Handle]);
            R3N_GPU_Ptr->bind_buffer(&R3N_Renderer_Internal.bufferv[R3N_Screen_Quad_Buffer_Handle]);
            R3N_GPU_Ptr->read_buffer(&R3N_Renderer_Internal.bufferv[R3N_Screen_Quad_Buffer_Handle]);
        }
        r3_log_stdoutf(INFO_LOG, "[R3N_Renderer] end gpu phase: (handle)%d (type)%d\n", phase->handle, phase->type);
    }
    r3_log_stdoutf(INFO_LOG, "[R3N_Renderer] end gpu frame: (handle)%d (calls)%d (phases)%d\n", R3N_Renderer_Internal.frame.handle, R3N_Renderer_Internal.frame.calls, R3N_Renderer_Internal.frame.phases);
    return;
}


u8 r3n_init_renderer(R3N_GPU_Backend backend) {
    if (R3N_Platform_Ptr == NULL) {
        r3_log_stdout(ERROR_LOG, "[R3N_Renderer] R3N_Platform not initialized!\n");
        return 0;
    }

    if (R3N_Renderer_Ptr == NULL) {
        R3N_Renderer_Ptr = r3_mem_alloc(sizeof(R3N_Renderer), 8);
        if (R3N_Renderer_Ptr == NULL) {
            r3_log_stdout(ERROR_LOG, "[R3N_Renderer] failed to allocate R3N_Renderer_Ptr dispatch table!\n");
            return 0;
        }
    }
    
    switch(backend) {
        case R3N_BACKEND_OPENGL: {
            if (!r3n_init_gl()) {
                r3_log_stdout(ERROR_LOG, "[R3N_Renderer] failed to initialize gpu backend: OpenGL\n");
            } else r3_log_stdout(SUCCESS_LOG, "[R3N_Renderer] initialized gpu backend: OpenGL\n");
        } break;
        case R3N_BACKEND_VULKAN:  // fall-through
        case R3N_BACKEND_DIRECTX:  // fall-through
        case R3N_BACKEND_INVALID:  // fall-through
        default: break;
    }
    R3N_Renderer_Internal.backend = backend;
    
    // initialize R3N_Renderer dispatch table
    R3N_Renderer_Ptr->render = render_impl;
    R3N_Renderer_Ptr->create_call = create_call_impl;
    R3N_Renderer_Ptr->create_frame = create_frame_impl;
    R3N_Renderer_Ptr->create_phase = create_phase_impl;
    R3N_Renderer_Ptr->create_buffer = create_buffer_impl;
    R3N_Renderer_Ptr->create_program = create_program_impl;
    R3N_Renderer_Ptr->create_pipeline = create_pipeline_impl;
    r3_log_stdout(SUCCESS_LOG, "[R3N_Renderer] R3N_Renderer_Ptr dispatch table initialized\n");

    // configure default gpu frame
    r3_arena_alloc(KiB, &R3N_Renderer_Internal.frame.arena);
    if (!R3N_Renderer_Internal.frame.arena.buffer) {
        r3_log_stdout(ERROR_LOG, "[R3N_Renderer] failed to allocate frame arena\n");
        r3n_deinit_renderer();
        return 0;
    } else { r3_log_stdout(SUCCESS_LOG, "[R3N_Renderer] allocated frame arena\n"); }
    
    // zero gpu frame call + phase arrays
    r3_mem_set(sizeof(R3N_GPU_Call), 0, &R3N_Renderer_Internal.frame.callv);
    r3_mem_set(sizeof(R3N_GPU_Phase), 0, &R3N_Renderer_Internal.frame.phasev);
    
    r3_log_stdout(SUCCESS_LOG, "[R3N_Renderer] renderer initialized\n");
    return 1;
}

u8 r3n_deinit_renderer(none) {
    switch(R3N_Renderer_Internal.backend) {
        case R3N_BACKEND_OPENGL: {
            if (!r3n_deinit_gl()) {
                r3_log_stdout(ERROR_LOG, "[R3N_Renderer] failed to deinitialize gpu backend: OpenGL\n");
            } else r3_log_stdout(SUCCESS_LOG, "[R3N_Renderer] deinitialized gpu backend: OpenGL\n");
        } break;
        case R3N_BACKEND_VULKAN:  // fall-through
        case R3N_BACKEND_DIRECTX:  // fall-through
        case R3N_BACKEND_INVALID:  // fall-through
        default: break;
    }

    if (R3N_Renderer_Ptr != NULL) {
        r3_mem_dealloc(R3N_Renderer_Ptr);
        R3N_Renderer_Ptr = NULL;
        r3_log_stdout(SUCCESS_LOG, "[R3N_Renderer] deinitialized R3N_Renderer_Ptr dispatch table\n");
    }
    
    R3N_Renderer_Internal.backend = R3N_BACKEND_INVALID;

    r3_log_stdout(SUCCESS_LOG, "[R3N_Renderer] renderer deinitialized\n");
    return 1;
}
