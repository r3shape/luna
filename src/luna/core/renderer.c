#define _LUNA_INTERNAL_
#include <include/luna/core/renderer.h>
#include <include/luna/core/platform.h>
#include <include/luna/core/api/gl.h>
#include <include/luna/core/api/gpu.h>

static struct LunaRendererInternal {
    LunaGpuBackend backend;
    
    // renderer resource state + metadata
    LunaGpuPipeline pipelinev[LUNA_GPU_RESOURCE_MAX];
    LunaGpuProgram programv[LUNA_GPU_RESOURCE_MAX];
    LunaGpuBuffer bufferv[LUNA_GPU_RESOURCE_MAX];
    LunaGpuFrame frame;
    u32 pipelines;
    u32 programs;
    u32 buffers;
    u32 frames;
    Mat4 projection;
} LunaRendererInternal = {0};

static f32 LunaScreenQuadVertices[] = {
    // location         // tcoord
    -1.0,  1.0, 0.0,    0.0, 1.0,
    -1.0, -1.0, 0.0,    0.0, 0.0,
    1.0,  -1.0, 0.0,    1.0, 0.0,

    -1.0,  1.0, 0.0,    0.0, 1.0,
     1.0, -1.0, 0.0,    1.0, 0.0,
     1.0,  1.0, 0.0,    1.0, 1.0
};
static LunaGpuHandle LunaScreenQuadBufferHandle = LUNA_GPU_RESOURCE_MAX;
static LunaGpuHandle LunaScreenQuadProgramHandle = LUNA_GPU_RESOURCE_MAX;

// global dispatch table ptr
LunaRenderApi* lunaRenderApi = NULL;


LunaGpuHandle createFrameImpl(none) {
    LunaRendererInternal.frame.handle = LunaRendererInternal.frames++;
    LunaRendererInternal.frame.phases = 0;
    LunaRendererInternal.frame.calls = 0;
    
    r3_arena_reset(&LunaRendererInternal.frame.arena);
    if (!r3_mem_set(sizeof(LunaRendererInternal.frame.callv), 0, &LunaRendererInternal.frame.callv) ||
        !r3_mem_set(sizeof(LunaRendererInternal.frame.phasev), 0, &LunaRendererInternal.frame.phasev)) {
            r3_log_stdout(WARN_LOG, "[LunaRenderer] failed to zero frame resource arrays!\n");
    }

    if (!LunaRendererInternal.frame.buffer || LunaRendererInternal.frame.buffer >= LunaRendererInternal.buffers || LunaRendererInternal.frame.buffer >= LUNA_GPU_RESOURCE_MAX) {
        LunaRendererInternal.frame.buffer = lunaRenderApi->createBuffer((LunaGpuBuffer){ .type = LUNA_BUFFER_FRAME });
    }

    return LunaRendererInternal.frame.handle;
}


LunaGpuHandle createCallImpl(LunaGpuCall call) {
    if ((LunaRendererInternal.frame.calls + 1) > LUNA_GPU_RESOURCE_MAX) {
        r3_log_stdoutf(WARN_LOG, "[LunaRenderer] gpu call max reached: (frame)%d\n", LunaRendererInternal.frame.handle);
        return I32_MAX;
    }

    if (call.uniforms > LUNA_GPU_CALL_UNIFORM_MAX) {
        r3_log_stdoutf(WARN_LOG, "[LunaRenderer] gpu call with invalid number of gpu uniforms: (uniforms)%d (max)%d\n", call.uniforms, LUNA_GPU_CALL_UNIFORM_MAX);
        return I32_MAX;
    }

    if (call.phase >= I32_MAX) {
        r3_log_stdoutf(WARN_LOG, "[LunaRenderer] gpu call to invalid gpu phase: (phase)%d\n", call.phase);
        return I32_MAX;
    }
    
    if (call.pipeline >= I32_MAX) {
        r3_log_stdoutf(WARN_LOG, "[LunaRenderer] gpu call to invalid gpu pipeline: (pipeline)%d\n", call.pipeline);
        return I32_MAX;
    }
    
    LunaGpuHandle handle = LunaRendererInternal.frame.calls++;
    LunaGpuCall* call_ptr = &LunaRendererInternal.frame.callv[handle];
    call_ptr->handle = handle;
    *call_ptr = call;

    LunaGpuPipeline* pipeline_ptr = &LunaRendererInternal.pipelinev[call_ptr->pipeline];
    LunaGpuProgram* program_ptr = &LunaRendererInternal.programv[pipeline_ptr->program];
    
    FOR(u8, uniform, 0, call_ptr->uniforms, 1) {
        lunaGpuApi->setUniform(&call_ptr->uniformv[uniform], program_ptr);
    }

    r3_log_stdoutf(INFO_LOG, "[LunaRenderer] created gpu call: (handle)%d (phase)%d (pipeline)%d (vertex buffer)%d (element buffer)%d (uniforms)%d\n", LunaRendererInternal.frame.calls, pipeline_ptr->handle, call_ptr->phase, call_ptr->vertex_buffer, call_ptr->element_buffer, call_ptr->uniforms);

    return handle;
}


LunaGpuHandle createPhaseImpl(LunaGpuPhase phase) {
    if ((LunaRendererInternal.frame.phases + 1) > LUNA_GPU_RESOURCE_MAX) {
        r3_log_stdoutf(WARN_LOG, "[LunaRenderer] gpu phase max reached: (frame)%d\n", LunaRendererInternal.frame.handle);
        return I32_MAX;
    }

    if (phase.type >= I32_MAX || phase.type >= LUNA_PHASE_TYPES) {
        r3_log_stdoutf(ERROR_LOG, "[LunaRenderer] invalid phase type: (type)%d\n", phase.type);
        return I32_MAX;
    }
    
    if (phase.uniforms > LUNA_GPU_PHASE_UNIFORM_MAX) {
        r3_log_stdoutf(WARN_LOG, "[LunaRenderer] gpu phase with invalid number of gpu uniforms: (uniforms)%d (max)%d\n", phase.uniforms, LUNA_GPU_PHASE_UNIFORM_MAX);
        return I32_MAX;
    }

    LunaGpuHandle handle = LunaRendererInternal.frame.phases++;
    LunaGpuPhase* phase_ptr = &LunaRendererInternal.frame.phasev[handle];

    if (!r3_mem_set(sizeof(LunaGpuHandle) * 8, 0, phase_ptr->readv) ||
        !r3_mem_set(sizeof(LunaGpuHandle) * 8, 0, phase_ptr->writev) ||
        !r3_mem_set(sizeof(LunaGpuUniform) * 4, 0, phase_ptr->uniformv)) {
            r3_log_stdout(WARN_LOG, "[LunaRenderer] failed to zero phase dependency arrays!\n");
    }
    
    *phase_ptr = (LunaGpuPhase){ .handle = handle, .type = phase.type, .writes = 0, .reads = 0, .uniforms = phase.uniforms };

    if (phase_ptr->uniforms && !r3_mem_write((sizeof(LunaGpuUniform) * phase_ptr->uniforms), phase.uniformv, &phase_ptr->uniformv)) {
        r3_log_stdout(ERROR_LOG, "[LunaRenderer] failed to write uniform array into memory\n");;
        return I32_MAX;
    }

    switch (phase.type) {
        case (LUNA_PHASE_DEPTH): {
            phase_ptr->depth.clear_depth = phase.depth.clear_depth;
        } break;
        case (LUNA_PHASE_OPAQUE): {
            phase_ptr->opaque.clear_color = phase.opaque.clear_color;
        } break;
        case (LUNA_PHASE_LIGHT):    // fall-through
        case (LUNA_PHASE_SHADOW):   // fall-through
        case (LUNA_PHASE_INVALID):  // fall-through
        case (LUNA_PHASE_TYPES):    // fall-through
        default: return I32_MAX;
    }

    r3_log_stdoutf(INFO_LOG, "[LunaRenderer] created gpu phase: (handle)%d (type)%d (uniforms)%d\n", phase_ptr->handle, phase_ptr->type, phase_ptr->uniforms);
    return handle;
}

none destroyPhaseImpl(LunaGpuHandle phase) {
    if (phase >= LunaRendererInternal.frame.phases || phase >= LUNA_GPU_RESOURCE_MAX) {
        r3_log_stdoutf(ERROR_LOG, "[LunaRenderer] invalid phase handle passed to destroyPhase(): (handle)%d\n", phase);
        return;
    }

    LunaGpuPhase* phase_ptr = &LunaRendererInternal.frame.phasev[phase];
    if (!r3_mem_set(sizeof(LunaGpuPhase), 0, phase_ptr)) {
        r3_log_stdoutf(ERROR_LOG, "[LunaRenderer] failed to destroy phase: (handle)%d\n", phase);
        return;
    } else { r3_log_stdoutf(ERROR_LOG, "[LunaRenderer] destroyed phase: (handle)%d\n", phase); }

    return;
}


LunaGpuHandle createBufferImpl(LunaGpuBuffer buffer) {
    if (buffer.type >= I32_MAX || buffer.type >= LUNA_BUFFER_TYPES) {
        r3_log_stdoutf(ERROR_LOG, "[LunaRenderer] invalid buffer type: (type)%d\n", buffer.type);
        return I32_MAX;
    }

    LunaGpuHandle handle = LunaRendererInternal.buffers++;
    LunaGpuBuffer* buf = &LunaRendererInternal.bufferv[handle];

    buf->type = buffer.type;
    buf->handle = handle;

    switch (buffer.type) {
        case (LUNA_BUFFER_VERTEX): {
            if (!buffer.vertex.size) {
                r3_log_stdoutf(ERROR_LOG, "[LunaRenderer] invalid vertex buffer size: (type)%d\n", buffer.vertex.size);
                return 0;
            }
            buf->vertex.size = buffer.vertex.size;
            buf->vertex.vertices = buffer.vertex.size / sizeof(f32);
            buf->vertex.vertexv = buffer.vertex.vertexv;
            buf->vertex.attribs = buffer.vertex.attribs;
            buf->vertex.vao = 0;
            buf->vertex.vbo = 0;

            lunaGpuApi->createVertexBuffer(buf);

            r3_log_stdoutf(SUCCESS_LOG, "[LunaRenderer] created vertex buffer: (vbo)%d (vao)%d (vertices)%d\n", buf->vertex.vbo, buf->vertex.vao, buf->vertex.vertices);
            return buf->handle;
        }
        case (LUNA_BUFFER_ELEMENT): {
            if (!buffer.element.size) {
                r3_log_stdoutf(ERROR_LOG, "[LunaRenderer] invalid element buffer size: (type)%d\n", buffer.element.size);
                return 0;
            }

            buf->element.elements = buffer.element.elements;
            buf->element.elementv = buffer.element.elementv;
            buf->element.ebo = 0;

            lunaGpuApi->createElementBuffer(buf);
            
            r3_log_stdoutf(SUCCESS_LOG, "[LunaRenderer] created element buffer: (ebo)%d (elements)%d\n", buf->element.ebo, buf->element.elements);
            return buf->handle;
        }
        case (LUNA_BUFFER_FRAME): {
            lunaGpuApi->createFrameBuffer(buf);

            r3_log_stdoutf(SUCCESS_LOG, "[LunaRenderer] created frame buffer: (frame buffer object)%d (texture buffer object)%d (render buffer object) %d\n",
                buf->frame.fbo, buf->frame.tbo, buf->frame.rbo);
            return buf->handle;
        }
        case (LUNA_BUFFER_TEXTURE): // fall-through
        default: break;
    }

    return I32_MAX;
}

none destroyBufferImpl(LunaGpuHandle buffer) {
    if (buffer >= LunaRendererInternal.buffers || buffer >= LUNA_GPU_RESOURCE_MAX) {
        r3_log_stdoutf(ERROR_LOG, "[LunaRenderer] invalid buffer handle passed to destroyBuffer(): (handle)%d\n", buffer);
        return;
    }

    LunaGpuBuffer* buffer_ptr = &LunaRendererInternal.bufferv[buffer];

    switch (buffer_ptr->type) {
        case (LUNA_BUFFER_VERTEX):  { lunaGpuApi->destroyVertexBuffer(buffer_ptr); }    break;
        case (LUNA_BUFFER_ELEMENT): { lunaGpuApi->destroyElementBuffer(buffer_ptr); }   break;
        case (LUNA_BUFFER_TEXTURE): { lunaGpuApi->destroyTextureBuffer(buffer_ptr); }   break;
        case (LUNA_BUFFER_FRAME):   { lunaGpuApi->destroyFrameBuffer(buffer_ptr); }     break;
        default: break;
    }

    if (!r3_mem_set(sizeof(LunaGpuBuffer), 0, buffer_ptr)) {
        r3_log_stdoutf(ERROR_LOG, "[LunaRenderer] failed to destroy buffer: (handle)%d\n", buffer);
        return;
    } else { r3_log_stdoutf(ERROR_LOG, "[LunaRenderer] destroyed buffer: (handle)%d\n", buffer); }

    return;
}


LunaGpuHandle createProgramImpl(str vertex_path, str fragment_path) {

    LunaGpuHandle handle = LunaRendererInternal.programs++;
    LunaGpuProgram* program = &LunaRendererInternal.programv[handle];

    // allocate vertex buffer
    if (!r3_buf_alloc(LUNA_GPU_PROGRAM_BUFFER_SIZE, &program->vertex_buffer)) {
        r3_log_stdout(ERROR_LOG, "[LunaRenderer] failed to allocate vertex program buffer\n");
        return I32_MAX;
    } else { r3_log_stdoutf(INFO_LOG, "[LunaRenderer] allocated vertex program buffer: (size)%d\n", LUNA_GPU_PROGRAM_BUFFER_SIZE); }

    // load vertex buffer
    if (!r3_buf_load(vertex_path, &program->vertex_buffer)) {
        r3_log_stdout(ERROR_LOG, "[LunaRenderer] failed to load vertex program buffer\n");
        r3_buf_dealloc(&program->vertex_buffer);
        return I32_MAX;
    } else {
        u16 wrote = r3_buf_field(BUFFER_WROTE_FIELD, &program->vertex_buffer);
        u16 writes = r3_buf_field(BUFFER_WRITES_FIELD, &program->vertex_buffer);
        r3_buf_rewind(&program->vertex_buffer);
        ((char*)program->vertex_buffer.data)[r3_buf_field(BUFFER_WROTE_FIELD, &program->vertex_buffer)] = '\0';
        r3_log_stdoutf(INFO_LOG, "[LunaRenderer] loaded vertex buffer: (wrote)%d (writes)%d \n(source)\n%s\n\n", wrote, writes, (cstr)program->vertex_buffer.data);
    }
    
    // allocate fragment buffer
    if (!r3_buf_alloc(LUNA_GPU_PROGRAM_BUFFER_SIZE, &program->fragment_buffer)) {
        r3_log_stdout(ERROR_LOG, "[LunaRenderer] failed to allocate fragment program buffer\n");
        r3_buf_dealloc(&program->vertex_buffer);
        return I32_MAX;
    } else { r3_log_stdoutf(INFO_LOG, "[LunaRenderer] allocated fragment program buffer: (size)%d\n", LUNA_GPU_PROGRAM_BUFFER_SIZE); }
    
    // load fragment buffer
    if (!r3_buf_load(fragment_path, &program->fragment_buffer)) {
        r3_log_stdout(ERROR_LOG, "[LunaRenderer] failed to load fragment program buffer\n");
        r3_buf_dealloc(&program->vertex_buffer);
        r3_buf_dealloc(&program->fragment_buffer);
        return I32_MAX;
    } else {
        u16 wrote = r3_buf_field(BUFFER_WROTE_FIELD, &program->fragment_buffer);
        u16 writes = r3_buf_field(BUFFER_WRITES_FIELD, &program->fragment_buffer);
        r3_buf_rewind(&program->fragment_buffer);
        ((char*)program->fragment_buffer.data)[r3_buf_field(BUFFER_WROTE_FIELD, &program->fragment_buffer)] = '\0';
        r3_log_stdoutf(INFO_LOG, "[LunaRenderer] loaded fragment buffer: (wrote)%d (writes)%d \n(source)\n%s\n\n", wrote, writes, (cstr)program->fragment_buffer.data);
    }

    // create gpu api program
    if (!lunaGpuApi->createProgram(program) || !program->program) {
        r3_log_stdout(ERROR_LOG, "[LunaRenderer] failed to create gpu program\n");
        r3_buf_dealloc(&program->vertex_buffer);
        r3_buf_dealloc(&program->fragment_buffer);
        r3_arr_hashed_dealloc(&program->uniformv);
        return I32_MAX;
    }

    // allocate uniform array
    if (!r3_arr_hashed_alloc(16, sizeof(LunaGpuUniform), &program->uniformv)) {
        r3_log_stdout(ERROR_LOG, "[LunaRenderer] failed to allocate program uniform array\n");
        r3_buf_dealloc(&program->vertex_buffer);
        r3_buf_dealloc(&program->fragment_buffer);
        return I32_MAX;
    } else { r3_log_stdout(INFO_LOG, "[LunaRenderer] allocated program uniform array\n"); }

    program->handle = handle;

    r3_log_stdoutf(INFO_LOG, "[LunaRenderer] created program: (handle)%d (program)%d\n", handle, program->program);
    return handle;
}

none destroyProgramImpl(LunaGpuHandle program) {
    if (program >= LunaRendererInternal.programs || program >= LUNA_GPU_RESOURCE_MAX) {
        r3_log_stdoutf(ERROR_LOG, "[LunaRenderer] invalid program handle passed to destroyProgram(): (handle)%d\n", program);
        return;
    }

    LunaGpuProgram* program_ptr = &LunaRendererInternal.programv[program];
    if (!lunaGpuApi->destroyProgram(program_ptr) || !r3_mem_set(sizeof(LunaGpuProgram), 0, program_ptr)) {
        r3_log_stdoutf(ERROR_LOG, "[LunaRenderer] failed to destroy program: (handle)%d\n", program);
        return;
    } else { r3_log_stdoutf(ERROR_LOG, "[LunaRenderer] destroyed program: (handle)%d\n", program); }

    return;
}


LunaGpuHandle createPipelineImpl(LunaGpuPipeline pipeline) {
    if ((LunaRendererInternal.pipelines + 1) > LUNA_GPU_RESOURCE_MAX) {
        r3_log_stdoutf(WARN_LOG, "[LunaRenderer] gpu pipeline max reached: (frame)%d\n", LunaRendererInternal.frame.handle);
        return I32_MAX;
    }

    if (pipeline.uniforms > LUNA_GPU_PIPELINE_UNIFORM_MAX) {
        r3_log_stdoutf(WARN_LOG, "[LunaRenderer] gpu pipeline with invalid number of gpu uniforms: (uniforms)%d (max)%d\n", pipeline.uniforms, LUNA_GPU_PIPELINE_UNIFORM_MAX);
        return I32_MAX;
    }

    LunaGpuHandle handle = LunaRendererInternal.pipelines++;
    LunaGpuPipeline* pipeline_ptr = &LunaRendererInternal.pipelinev[handle];
    pipeline_ptr->handle = handle;
    *pipeline_ptr = pipeline;

    LunaGpuProgram* program_ptr = &LunaRendererInternal.programv[pipeline_ptr->program];
    FOR(u8, uniform, 0, pipeline_ptr->uniforms, 1) {
        lunaGpuApi->setUniform(&pipeline_ptr->uniformv[uniform], program_ptr);
    }

    r3_log_stdoutf(INFO_LOG, "[LunaRenderer] created gpu pipeline: (handle)%d (program)%d (uniforms)%d (binds)%d\n", pipeline_ptr->handle, pipeline_ptr->program, pipeline_ptr->uniforms, pipeline_ptr->binds);
    return handle;
}

none destroyPipelineImpl(LunaGpuHandle pipeline) {
    if (pipeline >= LunaRendererInternal.pipelines || pipeline >= LUNA_GPU_RESOURCE_MAX) {
        r3_log_stdoutf(ERROR_LOG, "[LunaRenderer] invalid pipeline handle passed to destroyPipeline(): (handle)%d\n", pipeline);
        return;
    }

    LunaGpuPipeline* pipeline_ptr = &LunaRendererInternal.pipelinev[pipeline];

    // TODO: implement this function (lunaGpuApi->unbindProgram) !!!
    // lunaGpuApi->unbindProgram(&LunaRendererInternal.programv[pipeline_ptr->program])

    // TODO: implement this function (lunaGpuApi->unbindBuffer) !!!
    // FOR(u32, b, 0, pipeline_ptr->binds, 1) {
    //     lunaGpuApi->unbindBuffer(&LunaRendererInternal.bufferv[pipeline_ptr->bindv[b]]);
    // }

    if (!r3_mem_set(sizeof(LunaGpuPipeline), 0, pipeline_ptr)) {
        r3_log_stdoutf(ERROR_LOG, "[LunaRenderer] failed to destroy pipeline: (handle)%d\n", pipeline);
        return;
    } else { r3_log_stdoutf(ERROR_LOG, "[LunaRenderer] destroyed pipeline: (handle)%d\n", pipeline); }

    return;
}


none renderImpl(none) {
    // lazy load screen quad
    if (LunaScreenQuadBufferHandle == LUNA_GPU_RESOURCE_MAX) {
        LunaScreenQuadBufferHandle = lunaRenderApi->createBuffer((LunaGpuBuffer){
            .type = LUNA_BUFFER_VERTEX,
            .vertex.vertices = 6,
            .vertex.vertexv = LunaScreenQuadVertices,
            .vertex.size = sizeof(LunaScreenQuadVertices),
            .vertex.attribs = LUNA_VERTEX_LOCATION_ATTRIBUTE | LUNA_VERTEX_TEXTURE_ATTRIBUTE
        });
        LunaScreenQuadProgramHandle = lunaRenderApi->createProgram(LUNA_SCREEN_QUAD_VERTEX_PROGRAM, LUNA_SCREEN_QUAD_FRAGMENT_PROGRAM);
        r3_log_stdoutf(INFO_LOG, "[LunaRenderer] loaded screen quad buffer: (buffer handle)%d (program handle)%d\n", LunaScreenQuadBufferHandle, LunaScreenQuadProgramHandle);
    }
    
    r3_log_stdoutf(INFO_LOG, "[LunaRenderer] begin frame: (handle)%d (calls)%d (phases)%d\n", LunaRendererInternal.frame.handle, LunaRendererInternal.frame.calls, LunaRendererInternal.frame.phases);

    // default clear color
    Vec4 clear_color = (Vec4){ .data={150, 50, 100, 255} };

    // retrieve current frame
    LunaGpuFrame* frame = &LunaRendererInternal.frame;
    
    // retrieve current frame buffer
    LunaGpuBuffer* frame_buffer = &LunaRendererInternal.bufferv[frame->buffer];
    FOR(u32, p, 0, LunaRendererInternal.frame.phases, 1) {
        LunaGpuPhase* phase = &frame->phasev[p];
        r3_log_stdoutf(INFO_LOG, "[LunaRenderer] begin gpu phase: (handle)%d (type)%d (uniforms)%d\n", phase->handle, phase->type, phase->uniforms);

        // bind frame buffer
        lunaGpuApi->bindBuffer(frame_buffer);

        // clear color/depth buffers
        if (phase->type == LUNA_PHASE_DEPTH) {
            if (LunaRendererInternal.backend == LUNA_BACKEND_OPENGL) glEnable(GL_DEPTH_TEST);
            lunaGpuApi->clearDepthBuffer(phase->depth.clear_depth);
            r3_log_stdoutf(INFO_LOG, "[LunaRenderer] clear depth: %0.3f\n", phase->depth.clear_depth);
        } else {
            clear_color = phase->opaque.clear_color;
            lunaGpuApi->clearColorBuffer(phase->opaque.clear_color);
            r3_vec4_log("[LunaRenderer] clear color:", phase->opaque.clear_color);
        }

        FOR(u32, c, 0, frame->calls, 1) {
            LunaGpuCall* call = &frame->callv[c];
            if (call->phase != phase->handle) continue;
            r3_log_stdoutf(INFO_LOG, "[LunaRenderer] begin call: (phase)%d (pipeline)%d (uniforms)%d (vertex buffer)%d (element buffer)%d\n", call->phase, call->pipeline, call->uniforms, call->vertex_buffer, call->element_buffer);

            // retrieve gpu call program pipeline
            LunaGpuPipeline* pipeline = &LunaRendererInternal.pipelinev[call->pipeline];

            // retrieve pipeline program
            LunaGpuProgram* program = &LunaRendererInternal.programv[pipeline->program];

            // bind gpu pipeline program
            lunaGpuApi->bindProgram(program);

            // bind gpu pipeline buffers
            FOR(u32, b, 0, pipeline->binds, 1) {
                lunaGpuApi->bindBuffer(&LunaRendererInternal.bufferv[pipeline->bindv[b]]);
            }
            
            // send per-phase uniforms
            FOR(u32, phu, 0, phase->uniforms, 1) {
                lunaGpuApi->setUniform(&phase->uniformv[phu], program);
                lunaGpuApi->sendUniform(phase->uniformv[phu].name, program);
            }

            // send per-call uniforms
            FOR(u32, cu, 0, call->uniforms, 1) {
                lunaGpuApi->setUniform(&call->uniformv[cu], program);
                lunaGpuApi->sendUniform(call->uniformv[cu].name, program);
            }

            // retrieve gpu call vertex buffer
            LunaGpuBuffer* vertex_buffer = NULL;
            if (call->vertex_buffer < LunaRendererInternal.buffers && call->vertex_buffer <= LUNA_GPU_RESOURCE_MAX) {
                vertex_buffer = &LunaRendererInternal.bufferv[call->vertex_buffer];
            } else { r3_log_stdoutf(WARN_LOG, "[LunaRenderer] invalid vertex buffer handle assigned to call: (frame)%d (call handle)%d (vertex buffer)%d\n", frame->handle, call->handle, call->vertex_buffer); }
            
            // retrieve gpu call element buffer
            LunaGpuBuffer* element_buffer = NULL;
            if (call->element_buffer < LunaRendererInternal.buffers && call->element_buffer <= LUNA_GPU_RESOURCE_MAX) {
                element_buffer = &LunaRendererInternal.bufferv[call->element_buffer];
            } else { r3_log_stdoutf(WARN_LOG, "[LunaRenderer] invalid element buffer handle assigned to call: (frame)%d (call handle)%d (element buffer)%d\n", frame->handle, call->handle, call->element_buffer); }


            // TODO: increment phase buffer reads + append handle to readv array
            // bind + read vertex/element buffers
            lunaGpuApi->bindBuffer(vertex_buffer);
            if (element_buffer != NULL && element_buffer->element.elementv != NULL && element_buffer->element.elements != 0) {
                lunaGpuApi->readBuffer(element_buffer);
            } else lunaGpuApi->readBuffer(vertex_buffer);

            r3_log_stdoutf(INFO_LOG, "[LunaRenderer] end gpu call: (phase)%d (pipeline)%d (uniforms)%d (vertex buffer)%d (element buffer)%d\n", call->phase, call->pipeline, call->uniforms, call->vertex_buffer, call->element_buffer);
        }

        // GL frame buffer handling
        // stack alloc'd resources are safe here -- reclaimed by OS on iteration
        if (LunaRendererInternal.backend == LUNA_BACKEND_OPENGL){
            // bind default frame buffer
            lunaGpuApi->bindBuffer(&(LunaGpuBuffer){
                .type = LUNA_BUFFER_FRAME,
                .frame.fbo = 0
            });

            // preserve screen-space quad from depth-test discard
            glDisable(GL_DEPTH_TEST);

            lunaGpuApi->clearColorBuffer(clear_color);
            r3_vec4_log("[LunaRenderer] clear color:", clear_color);

            // TODO: frame buffer could own its color buffer and render buffer structures rather than lazy alloc here
            // bind texture buffer containing frame buffer's color buffer attachment
            lunaGpuApi->bindBuffer(&(LunaGpuBuffer){
                .type = LUNA_BUFFER_TEXTURE,
                .texture.slot = 0,
                .texture. type = LUNA_TEXTURE_2D,
                .texture.tbo = frame_buffer->frame.tbo
            });

            // render screen quad sampling frame buffer's color buffer
            lunaGpuApi->bindProgram(&LunaRendererInternal.programv[LunaScreenQuadProgramHandle]);
            lunaGpuApi->bindBuffer(&LunaRendererInternal.bufferv[LunaScreenQuadBufferHandle]);
            lunaGpuApi->readBuffer(&LunaRendererInternal.bufferv[LunaScreenQuadBufferHandle]);
        }
        r3_log_stdoutf(INFO_LOG, "[LunaRenderer] end gpu phase: (handle)%d (type)%d\n", phase->handle, phase->type);
    }
    r3_log_stdoutf(INFO_LOG, "[LunaRenderer] end gpu frame: (handle)%d (calls)%d (phases)%d\n", LunaRendererInternal.frame.handle, LunaRendererInternal.frame.calls, LunaRendererInternal.frame.phases);
    return;
}


u8 lunaInitRenderer(LunaGpuBackend backend) {
    if (lunaPlatformApi == NULL) {
        r3_log_stdout(ERROR_LOG, "[LunaRenderer] LunaPlatformApi not initialized!\n");
        return 0;
    }

    if (lunaRenderApi == NULL) {
        lunaRenderApi = r3_mem_alloc(sizeof(LunaRenderApi), 8);
        if (lunaRenderApi == NULL) {
            r3_log_stdout(ERROR_LOG, "[LunaRenderer] failed to allocate lunaRenderApi dispatch table!\n");
            return 0;
        }
    }
    
    switch(backend) {
        case LUNA_BACKEND_OPENGL: {
            if (!lunaInitOpenGL()) {
                r3_log_stdout(ERROR_LOG, "[LunaRenderer] failed to initialize gpu backend: OpenGL\n");
            } else r3_log_stdout(SUCCESS_LOG, "[LunaRenderer] initialized gpu backend: OpenGL\n");
        } break;
        case LUNA_BACKEND_VULKAN:  // fall-through
        case LUNA_BACKEND_DIRECTX:  // fall-through
        case LUNA_BACKEND_INVALID:  // fall-through
        default: break;
    }
    LunaRendererInternal.backend = backend;
    
    // initialize LunaRenderer dispatch table
    lunaRenderApi->render = renderImpl;
    lunaRenderApi->createCall = createCallImpl;
    lunaRenderApi->createFrame = createFrameImpl;
    lunaRenderApi->createPhase = createPhaseImpl;
    lunaRenderApi->createBuffer = createBufferImpl;
    lunaRenderApi->createProgram = createProgramImpl;
    lunaRenderApi->createPipeline = createPipelineImpl;
    r3_log_stdout(SUCCESS_LOG, "[LunaRenderer] lunaRenderApi dispatch table initialized\n");

    // configure default gpu frame
    r3_arena_alloc(KiB, &LunaRendererInternal.frame.arena);
    if (!LunaRendererInternal.frame.arena.buffer) {
        r3_log_stdout(ERROR_LOG, "[LunaRenderer] failed to allocate frame arena\n");
        lunaDeinitRenderer();
        return 0;
    } else { r3_log_stdout(SUCCESS_LOG, "[LunaRenderer] allocated frame arena\n"); }
    
    // zero gpu frame call + phase arrays
    r3_mem_set(sizeof(LunaGpuCall), 0, &LunaRendererInternal.frame.callv);
    r3_mem_set(sizeof(LunaGpuPhase), 0, &LunaRendererInternal.frame.phasev);
    
    r3_log_stdout(SUCCESS_LOG, "[LunaRenderer] renderer initialized\n");
    return 1;
}

u8 lunaDeinitRenderer(none) {
    switch(LunaRendererInternal.backend) {
        case LUNA_BACKEND_OPENGL: {
            if (!lunaDeinitOpenGL()) {
                r3_log_stdout(ERROR_LOG, "[LunaRenderer] failed to deinitialize gpu backend: OpenGL\n");
            } else r3_log_stdout(SUCCESS_LOG, "[LunaRenderer] deinitialized gpu backend: OpenGL\n");
        } break;
        case LUNA_BACKEND_VULKAN:  // fall-through
        case LUNA_BACKEND_DIRECTX:  // fall-through
        case LUNA_BACKEND_INVALID:  // fall-through
        default: break;
    }

    if (lunaRenderApi != NULL) {
        r3_mem_dealloc(lunaRenderApi);
        lunaRenderApi = NULL;
        r3_log_stdout(SUCCESS_LOG, "[LunaRenderer] deinitialized lunaRenderApi dispatch table\n");
    }
    
    LunaRendererInternal.backend = LUNA_BACKEND_INVALID;

    r3_log_stdout(SUCCESS_LOG, "[LunaRenderer] renderer deinitialized\n");
    return 1;
}
