// WLVERSE [https://wlverse.web.app] 
// opengldebugger.h
// 
// Custom calls for opengl debugging and various useful information
// 
// AUTHORS 
// [100%] Yew Chong (yewchong.k\@digipen.edu) 
// - Main Author 
// 
// Copyright (c) 2024 DigiPen, All rights reserved

#pragma once
#include <glad/glad.h>
#include "flexlogger.h"

#define PRINT_CURRENT_FRAMEBUFFER() { \
    GLuint currentFBO = 0; \
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&currentFBO); \
    Log::Info("Current framebuffer ID: " + std::to_string(currentFBO)); \
}

#define GET_OPENGL_ERROR() { \
    GLenum error = glGetError(); \
    if (error != GL_NO_ERROR) { \
        Log::Warning("OpenGL error: " + std::to_string(error)); \
    } \
}