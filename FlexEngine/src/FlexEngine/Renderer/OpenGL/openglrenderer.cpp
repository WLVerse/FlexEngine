#include "openglrenderer.h"

#include "assetmanager.h" // FLX_ASSET_GET
#include "DataStructures/freequeue.h"
#include "Renderer/Camera/camera.h"

namespace FlexEngine
{

  // static member initialization
  uint32_t OpenGLRenderer::m_draw_calls = 0;
  uint32_t OpenGLRenderer::m_draw_calls_last_frame = 0;
  bool OpenGLRenderer::m_depth_test = false;
  bool OpenGLRenderer::m_blending = false;

  uint32_t OpenGLRenderer::GetDrawCalls()
  {
    return m_draw_calls;
  }

  uint32_t OpenGLRenderer::GetDrawCallsLastFrame()
  {
    return m_draw_calls_last_frame;
  }

  bool OpenGLRenderer::IsDepthTestEnabled()
  {
    return m_depth_test;
  }

  void OpenGLRenderer::EnableDepthTest()
  {
    m_depth_test = true;
    glEnable(GL_DEPTH_TEST);
  }

  void OpenGLRenderer::DisableDepthTest()
  {
    m_depth_test = false;
    glDisable(GL_DEPTH_TEST);
  }

  bool OpenGLRenderer::IsBlendingEnabled()
  {
    return m_blending;
  }

  void OpenGLRenderer::EnableBlending()
  {
    m_blending = true;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  void OpenGLRenderer::DisableBlending()
  {
    m_blending = false;
    glDisable(GL_BLEND);
  }

  void OpenGLRenderer::ClearFrameBuffer()
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void OpenGLRenderer::ClearColor(const Vector4& color)
  {
    glClearColor(color.x, color.y, color.z, color.w);
    m_draw_calls_last_frame = m_draw_calls;
    m_draw_calls = 0;
  }

  void OpenGLRenderer::Draw(GLsizei size)
  {
    glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, nullptr);
    m_draw_calls++;
  }

  void OpenGLRenderer::DrawTexture2D(const Renderer2DProps& props)
  {
    // unit square
    // Flipped UVs for OpenGL
    static const float vertices[] = {
      // Position        // TexCoords
      -0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // Bottom-left
       0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // Bottom-right
       0.5f,  0.5f, 0.0f,   0.0f, 1.0f, // Top-right
       0.5f,  0.5f, 0.0f,   0.0f, 1.0f, // Top-right
      -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // Top-left
      -0.5f, -0.5f, 0.0f,   1.0f, 0.0f  // Bottom-left
    };

    static GLuint vao = 0, vbo = 0;

    if (vao == 0)
    {
      glGenVertexArrays(1, &vao);
      glGenBuffers(1, &vbo);

      glBindVertexArray(vao);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

      glBindVertexArray(0);

      // free in freequeue
      FreeQueue::Push(
        [=]()
        {
          glDeleteBuffers(1, &vbo);
          glDeleteVertexArrays(1, &vao);
        }
      );
    }

    // guard
    if (vao == 0 || props.shader == "" || props.scale == Vector2::Zero) return;

    // bind all
    glBindVertexArray(vao);

    auto& asset_shader = FLX_ASSET_GET(Asset::Shader, props.shader);
    asset_shader.Use();

    if (props.texture != "")
    {
      asset_shader.SetUniform_bool("u_use_texture", true);
      auto& asset_texture = FLX_ASSET_GET(Asset::Texture, props.texture);
      asset_texture.Bind(asset_shader, "u_texture", 0);
    }
    else if (props.color != Vector3::Zero)
    {
      asset_shader.SetUniform_bool("u_use_texture", false);
      asset_shader.SetUniform_vec3("u_color", props.color);
    }
    else
    {
      Log::Fatal("No texture or color specified for texture shader.");
    }

    asset_shader.SetUniform_vec3("u_color_to_add", props.color_to_add);
    asset_shader.SetUniform_vec3("u_color_to_multiply", props.color_to_multiply);

    // alignment
    Vector2 position = Vector2(props.position.x, props.position.y);
    switch (props.alignment)
    {
    case Renderer2DProps::Alignment_TopLeft:
      position += props.scale * 0.5f;
      break;
    case Renderer2DProps::Alignment_Center:
    default:
      break;
    }
    
    // "Model scale" in this case refers to the scale of the object itself.
    Matrix4x4 model = Matrix4x4::Identity;
    asset_shader.SetUniform_mat4("u_model", model.Translate(Vector3(position.x, position.y, 0.0f)).Scale(Vector3(props.scale.x, props.scale.y, 1.0f)));

    // View matrix is always the same for 2D rendering outside of camera translation
    static const Matrix4x4 view_matrix = Matrix4x4::LookAt(Vector3::Zero, Vector3::Forward, Vector3::Up);

    // For 2D rendering, we use an orthographic projection matrix, but this one uses the window as the viewfinder
    /*Matrix4x4 projection_view = Matrix4x4::Orthographic(
      0.0f, props.window_size.x,
      props.window_size.y, 0.0f,
      -2.0f, 2.0f
    ) * view_matrix;*/
    asset_shader.SetUniform_mat4("u_projection_view", main_camera.GetProjViewMatrix());

    // draw
    glDrawArrays(GL_TRIANGLES, 0, 6);
    m_draw_calls++;

    glBindVertexArray(0);
  }

}