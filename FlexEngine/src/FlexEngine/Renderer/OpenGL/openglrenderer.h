// WLVERSE [https://wlverse.web.app]
// openglrenderer.h
// 
// OpenGLRenderer class to manage OpenGL rendering.
// 
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include "FlexMath/vector4.h"
#include "opengltexture.h"

#include <glad/glad.h>

namespace FlexEngine
{
  class __FLX_API OpenGLRenderer
  {
    static uint32_t m_draw_calls;
    static uint32_t m_draw_calls_last_frame;
    static bool m_depth_test;
    static bool m_blending;
  public:

    static uint32_t GetDrawCalls();
    static uint32_t GetDrawCallsLastFrame();

    static bool IsDepthTestEnabled();
    static void EnableDepthTest();
    static void DisableDepthTest();

    static bool IsBlendingEnabled();
    static void EnableBlending();
    static void DisableBlending();

    static void ClearFrameBuffer();
    static void ClearColor(const Vector4& color);

    static void Draw(GLsizei size);

    // Standalone helper function to draw a texture.
    // Uses an internal unit square mesh to draw the texture.
    // Pass in a shader that supports the texture and color uniforms.
    //static void DrawTexture2D(const Renderer2DProps& props = {});
  };

}