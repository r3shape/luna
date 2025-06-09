#include <include/luna/core/platform.h>
#include <include/luna/core/renderer.h>
#include <include/luna/core/api/glapi.h>
#include <include/luna/core/api/gpuapi.h>

#define STB_IMAGE_IMPLEMENTATION
#include <include/STB/stb_image.h>

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


byte lunaInitGlApi(LunaGpuApi* table) {
    if (!table) {
        saneLog->log(SANE_LOG_ERROR, "[glapi] invalid table ptr :: lunaInitGpuBackend()");
        return SSDK_FALSE;
    }

    if (!lunaPlatform->createGLContext()) {
        saneLog->log(SANE_LOG_ERROR, "[glapi] failed to create opengl context");
        return SSDK_FALSE;
    } else saneLog->log(SANE_LOG_SUCCESS, "[glapi] created opengl context");

    // glapi
    LunaLibrary opengl32;
    if (!lunaPlatform->loadLibrary(NULL, "opengl32", &opengl32)) {
        saneLog->log(SANE_LOG_ERROR, "[glapi] failed to load opengl");
        return SSDK_FALSE;
    }  else saneLog->logFmt(SANE_LOG_SUCCESS, "[glapi] loaded opengl v%s", glGetString(GL_VERSION));

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

    SSDK_FORI(0, sizeof(functions) / sizeof(functions[0]), 1) {
        if (!lunaPlatform->loadLibrarySymbol(functions[i].name, functions[i].function, &opengl32)) {
            saneLog->logFmt(SANE_LOG_WARN, "[glapi] failed to load function: %s", functions[i].name);
        } else saneLog->logFmt(SANE_LOG_SUCCESS, "[glapi] loaded function: %s", functions[i].name);
    }
    
    lunaPlatform->unloadLibrary(&opengl32);

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

    return SSDK_TRUE;
}

byte lunaDeinitGlApi(LunaGpuApi* table) {
    if (!table) {
        saneLog->log(SANE_LOG_WARN, "[glapi] invalid table ptr :: lunaInitGpuBackend()");
        return SSDK_FALSE;
    }

    lunaPlatform->destroyGLContext();

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

    return SSDK_TRUE;
}
