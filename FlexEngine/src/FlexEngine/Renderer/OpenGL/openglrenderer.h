#pragma once

#include "flx_api.h"

#include "FlexMath/vector4.h"
#include "opengltexture.h"

#include <glad/glad.h>

#include "Renderer/Camera/camera.h" // For rendering, need to include camera

namespace FlexEngine
{

  struct __FLX_API Renderer2DProps
  {
    enum __FLX_API Alignment
    {
      Alignment_Center = 0,
      Alignment_TopLeft = 1,
    };

    std::string shader = R"(/shaders/texture)";
    std::string texture = R"(/images/flexengine/flexengine-256.png)";
    Vector3 color = Vector3(1.0f, 0.0f, 1.0f);
    Vector3 color_to_add = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 color_to_multiply = Vector3(1.0f, 1.0f, 1.0f);
    Vector2 position = Vector2(0.0f, 0.0f);
    Vector2 scale = Vector2(1.0f, 1.0f);
    Vector2 window_size = Vector2(800.0f, 600.0f);
    Alignment alignment = Alignment_Center;
  };

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
    static void DrawTexture2D(FlexEngine::Camera const& cam, const Renderer2DProps& props = {});
  };

}