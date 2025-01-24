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

    std::string shader = R"(/shaders/texture.flxshader)";
    std::string asset = R"(/images/flexengine/flexengine-256.png)"; // Texture or spritesheet
    int texture_index = -1; // For spritesheets, -1 means not a spritesheet
    Vector3 color = Vector3(1.0f, 0.0f, 1.0f);
    Vector3 color_to_add = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 color_to_multiply = Vector3(1.0f, 1.0f, 1.0f);
    Vector2 position = Vector2(0.0f, 0.0f);
    Vector2 scale = Vector2(1.0f, 1.0f);
    Vector2 window_size = Vector2(800.0f, 600.0f);
    Alignment alignment = Alignment_Center;
  };

  /*!***************************************************************************
  * \brief
  * Struct for 2D text rendering configuration and properties.
  *
  * The Renderer2DText struct encapsulates the necessary parameters for rendering
  * 2D text, including shader paths, font types, color settings, transformation
  * matrices, and alignment options for both X and Y axes.
  ***************************************************************************/
  struct __FLX_API Renderer2DText
  {
      enum __FLX_API AlignmentX
      {
          Alignment_Center = 0,      /*!< Center alignment */
          Alignment_Left = 1,        /*!< Left alignment */
          Alignment_Right = 2,       /*!< Right alignment */
      };

      enum __FLX_API AlignmentY
      {
          Alignment_Middle = 0,      /*!< Center alignment */
          Alignment_Top = 1,         /*!< Top alignment */
          Alignment_Bottom = 2,      /*!< Bottom alignment */
      };

      std::string m_shader = R"(/shaders/freetypetext.flxshader)";  /*!< Path to the shader for text rendering */
      std::string m_fonttype = R"()";                     /*!< Font type to use for rendering; empty means an error will occur */
      std::string m_words;                                /*!< The actual text to render */
      Vector3 m_color = Vector3::Zero;                    /*!< Color of the text */
      Vector2 m_window_size = Vector2(800.0f, 600.0f);
      Matrix4x4 m_transform = Matrix4x4::Identity;        /*!< Transformation matrix for positioning the text */
      std::pair<AlignmentX, AlignmentY> m_alignment;      /*!< Pair indicating X and Y alignment settings */
      float m_linespacing = 2.0f;
      float m_letterspacing = 2.0f;
      float m_maxwidthtextbox = 500.0f;                   /*!< Dimension width of text box */
      float m_maxheighttextbox = 500.0f;                  /*!< Dimension height of text box */
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
    static void DrawTexture2D(Camera const& cam, const Renderer2DProps& props = {});

    // Overloaded function to draw text as a texture.
    // Uses an internal unit square mesh to draw the texture.
    // Pass in a shader that supports the texture and color uniforms.
    static void DrawTexture2D(Camera const& cam, const Renderer2DText& text = {});

    // This function is designed to be extremely lightweight
    // and doesn't require the camera, props, or asset manager.
    // It uses the texture shader and a unit square mesh.
    static void DrawSimpleTexture2D(
      const Asset::Texture& texture = Asset::Texture::None,
      const Vector2& position = Vector2(0.0f, 0.0f),
      const Vector2& scale = Vector2(1600.0f, 900.0f),
      const Vector2& window_size = Vector2(1600.0f, 900.0f),
      Renderer2DProps::Alignment alignment = Renderer2DProps::Alignment_TopLeft
    );
  };

}