#include <include/luna/core/platform.h>
#include <include/luna/core/renderer.h>
#include <include/luna/core/api/glapi.h>
#include <include/luna/core/api/gpuapi.h>

#include <include/r3kt/math.h>
#include <include/r3kt/io/log.h>
#include <include/r3kt/mem/arena.h>

#define STB_IMAGE_IMPLEMENTATION
#include <include/STB/stb_image.h>

// define null fnptrs
LUNA_GLFNPTR(GLGenBuffers);
LUNA_GLFNPTR(GLBindBuffer);
LUNA_GLFNPTR(GLBufferData);
LUNA_GLFNPTR(GLMapBuffer);
LUNA_GLFNPTR(GLUnmapBuffer);
LUNA_GLFNPTR(GLBufferSubData);
LUNA_GLFNPTR(GLGetBufferParameteriv);
LUNA_GLFNPTR(GLDeleteBuffers);
LUNA_GLFNPTR(GLGenVertexArrays);
LUNA_GLFNPTR(GLBindVertexArray);
LUNA_GLFNPTR(GLDeleteVertexArrays);
LUNA_GLFNPTR(GLEnableVertexAttribArray);
LUNA_GLFNPTR(GLDisableVertexAttribArray);
LUNA_GLFNPTR(GLVertexAttribPointer);
LUNA_GLFNPTR(GLCreateShader);
LUNA_GLFNPTR(GLShaderSource);
LUNA_GLFNPTR(GLCompileShader);
LUNA_GLFNPTR(GLDeleteShader);
LUNA_GLFNPTR(GLGetShaderiv);
LUNA_GLFNPTR(GLGetShaderInfoLog);
LUNA_GLFNPTR(GLCreateProgram);
LUNA_GLFNPTR(GLAttachShader);
LUNA_GLFNPTR(GLDetachShader);
LUNA_GLFNPTR(GLLinkProgram);
LUNA_GLFNPTR(GLUseProgram);
LUNA_GLFNPTR(GLDeleteProgram);
LUNA_GLFNPTR(GLGetProgramiv);
LUNA_GLFNPTR(GLGetProgramInfoLog);
LUNA_GLFNPTR(GLGetUniformLocation);
LUNA_GLFNPTR(GLUniform1i);
LUNA_GLFNPTR(GLUniform1f);
LUNA_GLFNPTR(GLUniform2fv);
LUNA_GLFNPTR(GLUniform3fv);
LUNA_GLFNPTR(GLUniform4fv);
LUNA_GLFNPTR(GLUniformMatrix4fv);
LUNA_GLFNPTR(GLGenTextures);
LUNA_GLFNPTR(GLBindTexture);
LUNA_GLFNPTR(GLTexParameteri);
LUNA_GLFNPTR(GLTexImage2D);
LUNA_GLFNPTR(GLActiveTexture);
LUNA_GLFNPTR(GLDeleteTextures);
LUNA_GLFNPTR(GLGenerateMipmap);
LUNA_GLFNPTR(GLGenFramebuffers);
LUNA_GLFNPTR(GLBindFramebuffer);
LUNA_GLFNPTR(GLFramebufferTexture2D);
LUNA_GLFNPTR(GLFramebufferRenderbuffer);
LUNA_GLFNPTR(GLCheckFramebufferStatus);
LUNA_GLFNPTR(GLDeleteFramebuffers);
LUNA_GLFNPTR(GLGenRenderbuffers);
LUNA_GLFNPTR(GLBindRenderbuffer);
LUNA_GLFNPTR(GLRenderbufferStorage);
LUNA_GLFNPTR(GLDeleteRenderbuffers);
LUNA_GLFNPTR(GLDrawArrays);
LUNA_GLFNPTR(GLDrawElements);
LUNA_GLFNPTR(GLEnable);
LUNA_GLFNPTR(GLDisable);
LUNA_GLFNPTR(GLBlendFunc);
LUNA_GLFNPTR(GLCullFace);
LUNA_GLFNPTR(GLDepthFunc);
LUNA_GLFNPTR(GLViewport);
LUNA_GLFNPTR(GLPolygonMode);
LUNA_GLFNPTR(GLClear);
LUNA_GLFNPTR(GLClearColor);
LUNA_GLFNPTR(GLClearDepth);
LUNA_GLFNPTR(GLGetError);
LUNA_GLFNPTR(GLGetString);

// internal dispatch table ptrs
static LunaPlatform* glapiPlatform = NULL;

none clearDepthBufferImpl(f32 depth) {
    return;
}

none clearColorBufferImpl(Vec3 color) {
    return;
}


none createProgramImpl(LunaGpuProgram* program) {
    return;
}

none destroyProgramImpl(LunaGpuProgram* program) {
    return;
}


none sendUniformImpl(str name, LunaGpuProgram* program) {
    return;
}

none setUniformImpl(LunaGpuUniform* uniform, LunaGpuProgram* program) {
    return;
}


none createVertexBufferImpl(LunaGpuBuffer* buffer) {
    return;
}

none destroyVertexBufferImpl(LunaGpuBuffer* buffer) {
    return;
}


none createElementBufferImpl(LunaGpuBuffer* buffer) {
    return;
}

none destroyElementBufferImpl(LunaGpuBuffer* buffer) {
    return;
}


none createTextureBufferImpl(LunaGpuBuffer* buffer) {
    return;
}

none destroyTextureBufferImpl(LunaGpuBuffer* buffer) {
    return;
}


none createFrameBufferImpl(LunaGpuBuffer* buffer) {
    return;
}

none destroyFrameBufferImpl(LunaGpuBuffer* buffer) {
    return;
}


none bindBufferImpl(LunaGpuBuffer* buffer) {
    return;
}

none readBufferImpl(LunaGpuBuffer* buffer) {
    return;
}

none writeBufferImpl(LunaGpuBuffer* buffer) {
    return;
}

none bindProgramImpl(LunaGpuProgram* program) {
    return;
}


byte lunaInitGlApi(LunaGpuApi* table, ptr platform_table) {
    if (!table || !platform_table) {
        r3_log_stdout(ERROR_LOG, "[glapi] invalid table ptr :: lunaInitGpuBackend()\n");
        return 0;
    }

    // assign internal dispatch table ptr
    glapiPlatform = (LunaPlatform*)platform_table;

    if (!glapiPlatform->createGLContext()) {
        r3_log_stdout(ERROR_LOG, "[glapi] failed to create opengl context\n");
        return 0;
    } else r3_log_stdout(SUCCESS_LOG, "[glapi] created opengl context\n");

    // glapi
    LunaLibrary opengl32;
    if (!glapiPlatform->loadLibrary(NULL, "opengl32", &opengl32)) {
        r3_log_stdout(ERROR_LOG, "[glapi] failed to load opengl\n");
        return 0;
    }  else r3_log_stdoutf(SUCCESS_LOG, "[glapi] loaded opengl v%s", glGetString(GL_VERSION));

    struct gl_func {
        ptr* function;
        str name;
    } functions[] = {
        // BUFFER FUNCTIONS
        {(ptr*)&GLGenBuffers, "glGenBuffers"},
        {(ptr*)&GLBindBuffer, "glBindBuffer"},
        {(ptr*)&GLBufferData, "glBufferData"},
        {(ptr*)&GLMapBuffer, "glMapBuffer"},
        {(ptr*)&GLUnmapBuffer, "glUnmapBuffer"},
        {(ptr*)&GLBufferSubData, "glBufferSubData"},
        {(ptr*)&GLGetBufferParameteriv, "glGetBufferParameteriv"},
        {(ptr*)&GLDeleteBuffers, "glDeleteBuffers"},

        // VAO FUNCTIONS
        {(ptr*)&GLGenVertexArrays, "glGenVertexArrays"},
        {(ptr*)&GLBindVertexArray, "glBindVertexArray"},
        {(ptr*)&GLDeleteVertexArrays, "glDeleteVertexArrays"},
        {(ptr*)&GLEnableVertexAttribArray, "glEnableVertexAttribArray"},
        {(ptr*)&GLDisableVertexAttribArray, "glDisableVertexAttribArray"},
        {(ptr*)&GLVertexAttribPointer, "glVertexAttribPointer"},

        // SHADER FUNCTIONS
        {(ptr*)&GLCreateShader, "glCreateShader"},
        {(ptr*)&GLShaderSource, "glShaderSource"},
        {(ptr*)&GLCompileShader, "glCompileShader"},
        {(ptr*)&GLDeleteShader, "glDeleteShader"},
        {(ptr*)&GLGetShaderiv, "glGetShaderiv"},
        {(ptr*)&GLGetShaderInfoLog, "glGetShaderInfoLog"},

        {(ptr*)&GLCreateProgram, "glCreateProgram"},
        {(ptr*)&GLAttachShader, "glAttachShader"},
        {(ptr*)&GLDetachShader, "glDetachShader"},
        {(ptr*)&GLLinkProgram, "glLinkProgram"},
        {(ptr*)&GLUseProgram, "glUseProgram"},
        {(ptr*)&GLDeleteProgram, "glDeleteProgram"},
        {(ptr*)&GLGetProgramiv, "glGetProgramiv"},
        {(ptr*)&GLGetProgramInfoLog, "glGetProgramInfoLog"},
        {(ptr*)&GLGetUniformLocation, "glGetUniformLocation"},

        // UNIFORMS
        {(ptr*)&GLUniform1i, "glUniform1i"},
        {(ptr*)&GLUniform1f, "glUniform1f"},
        {(ptr*)&GLUniform2fv, "glUniform2fv"},
        {(ptr*)&GLUniform3fv, "glUniform3fv"},
        {(ptr*)&GLUniform4fv, "glUniform4fv"},
        {(ptr*)&GLUniformMatrix4fv, "glUniformMatrix4fv"},

        // TEXTURES
        {(ptr*)&GLGenTextures, "glGenTextures"},
        {(ptr*)&GLBindTexture, "glBindTexture"},
        {(ptr*)&GLTexParameteri, "glTexParameteri"},
        {(ptr*)&GLTexImage2D, "glTexImage2D"},
        {(ptr*)&GLActiveTexture, "glActiveTexture"},
        {(ptr*)&GLDeleteTextures, "glDeleteTextures"},
        {(ptr*)&GLGenerateMipmap, "glGenerateMipmap"},

        // FRAMEBUFFERS + RENDERBUFFERS
        {(ptr*)&GLGenFramebuffers, "glGenFramebuffers"},
        {(ptr*)&GLBindFramebuffer, "glBindFramebuffer"},
        {(ptr*)&GLFramebufferTexture2D, "glFramebufferTexture2D"},
        {(ptr*)&GLFramebufferRenderbuffer, "glFramebufferRenderbuffer"},
        {(ptr*)&GLCheckFramebufferStatus, "glCheckFramebufferStatus"},
        {(ptr*)&GLDeleteFramebuffers, "glDeleteFramebuffers"},

        {(ptr*)&GLGenRenderbuffers, "glGenRenderbuffers"},
        {(ptr*)&GLBindRenderbuffer, "glBindRenderbuffer"},
        {(ptr*)&GLRenderbufferStorage, "glRenderbufferStorage"},
        {(ptr*)&GLDeleteRenderbuffers, "glDeleteRenderbuffers"},

        // DRAWING
        {(ptr*)&GLDrawArrays, "glDrawArrays"},
        {(ptr*)&GLDrawElements, "glDrawElements"},

        // STATE
        {(ptr*)&GLEnable, "glEnable"},
        {(ptr*)&GLDisable, "glDisable"},
        {(ptr*)&GLBlendFunc, "glBlendFunc"},
        {(ptr*)&GLCullFace, "glCullFace"},
        {(ptr*)&GLDepthFunc, "glDepthFunc"},
        {(ptr*)&GLViewport, "glViewport"},
        {(ptr*)&GLPolygonMode, "glPolygonMode"},
        {(ptr*)&GLClear, "glClear"},
        {(ptr*)&GLClearColor, "glClearColor"},
        {(ptr*)&GLClearDepth, "glClearDepth"},

        // INFO / DEBUG
        {(ptr*)&GLGetError, "glGetError"},
        {(ptr*)&GLGetString, "glGetString"}
    };

    FOR_I(0, sizeof(functions) / sizeof(functions[0]), 1) {
        if (!glapiPlatform->loadLibrarySymbol(functions[i].name, functions[i].function, &opengl32)) {
            r3_log_stdoutf(WARN_LOG, "[glapi] failed to load function: %s\n", functions[i].name);
        } else r3_log_stdoutf(SUCCESS_LOG, "[glapi] loaded function: %s\n", functions[i].name);
    }
    
    glapiPlatform->unloadLibrary(&opengl32);

    // gpuapi
    table->clearDepthBuffer = clearDepthBufferImpl;
    table->clearColorBuffer = clearColorBufferImpl;
    
    table->createProgram = createProgramImpl;
    table->destroyProgram = destroyProgramImpl;
    
    table->sendUniform = sendUniformImpl;
    table->setUniform = setUniformImpl;
    
    table->createVertexBuffer = createVertexBufferImpl;
    table->destroyVertexBuffer = destroyVertexBufferImpl;
    
    table->createElementBuffer = createElementBufferImpl;
    table->destroyElementBuffer = destroyElementBufferImpl;
    
    table->createTextureBuffer = createTextureBufferImpl;
    table->destroyTextureBuffer = destroyTextureBufferImpl;
    
    table->createFrameBuffer = createFrameBufferImpl;
    table->destroyFrameBuffer = destroyFrameBufferImpl;
    
    table->bindBuffer = bindBufferImpl;
    table->readBuffer = readBufferImpl;
    table->writeBuffer = writeBufferImpl;
    table->bindProgram = bindProgramImpl;

    return 1;
}

byte lunaDeinitGlApi(LunaGpuApi* table) {
    if (!table) {
        r3_log_stdout(WARN_LOG, "[glapi] invalid table ptr :: lunaInitGpuBackend()\n");
        return 0;
    }

    glapiPlatform->destroyGLContext();

    table->clearDepthBuffer = NULL;
    table->clearColorBuffer = NULL;

    table->createProgram = NULL;
    table->destroyProgram = NULL;

    table->sendUniform = NULL;
    table->setUniform = NULL;

    table->createVertexBuffer = NULL;
    table->destroyVertexBuffer = NULL;

    table->createElementBuffer = NULL;
    table->destroyElementBuffer = NULL;

    table->createTextureBuffer = NULL;
    table->destroyTextureBuffer = NULL;

    table->createFrameBuffer = NULL;
    table->destroyFrameBuffer = NULL;

    table->bindBuffer = NULL;
    table->readBuffer = NULL;
    table->writeBuffer = NULL;
    table->bindProgram = NULL;

    return 1;
}
