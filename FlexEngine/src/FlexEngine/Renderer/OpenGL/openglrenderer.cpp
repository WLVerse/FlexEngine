/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// openglrenderer.h / .cpp
//
// This header declares the structures and functions used for 2D rendering
// with OpenGL. It defines properties for texture and text rendering, sprite
// batching, and includes the OpenGLRenderer class, which provides a suite of
// static functions for drawing 2D elements (textures, text, and batches) using
// an internal unit square mesh.
//
// Structures defined:
// - Renderer2DProps: Contains transformation, shader, asset, and color data
//   for rendering 2D textures.
// - Renderer2DText: Encapsulates text rendering parameters including shader,
//   font type, alignment, and transformation settings.
// - Renderer2DSpriteBatch: Holds batch data for rendering multiple sprite
//   instances with transformations and per-instance opacity.
//
// The OpenGLRenderer class provides static helper functions for common
// 2D rendering tasks and configuration options for depth testing and blending.
//
// AUTHORS
// [40%] Wen Loong (wenloong.l\@digipen.edu)
// [30%] Yew Chong (yewchong.k\@digipen.edu)
// [30%] Soh Wei Jie (weijie.soh\@digipen.edu)
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "openglrenderer.h"

#include "assetmanager.h" // FLX_ASSET_GET
#include "DataStructures/freequeue.h"
#include "FlexEngine/flexprefs.h"
#include "FlexEngine/FlexMath/quaternion.h"

#include "window.h"
namespace FlexEngine
{

  // static member initialization
  uint32_t OpenGLRenderer::m_draw_calls = 0;
  uint32_t OpenGLRenderer::m_draw_calls_last_frame = 0;
  uint32_t OpenGLRenderer::m_maxInstances = 3000; //Should be more than enough
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

  #pragma region Sprite Rendering
  // TODO: cache the vao and vbo
  // Draws to default camera
  void OpenGLRenderer::DrawTexture2D(Camera const& cam, const Renderer2DProps& props)
  {
    DrawTexture2D(props, cam);
  }

  void OpenGLRenderer::DrawTexture2D(const GLuint& texture, const Matrix4x4& transform, const Vector2& screenDimensions)
  {
      #pragma region VAO setup
      // unit square
      static const float vertices[] = {
          // Position           // TexCoords
          -0.5f, 0.5f, 0.0f,   0.0f, 1.0f, // Bottom-left
           0.5f, 0.5f, 0.0f,   1.0f, 1.0f, // Bottom-right
           0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // Top-right
           0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // Top-right
          -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, // Top-left
          -0.5f, 0.5f, 0.0f,   0.0f, 1.0f  // Bottom-left
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
      if (vao == 0) return; // hey, might need to check if scale is 0 because thats what the old code does idk

      // bind all
      glBindVertexArray(vao);
      #pragma endregion

      auto& asset_shader = AssetManager::Get<Asset::Shader>(R"(/shaders/texture.flxshader)");
      asset_shader.Use();

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture);

      asset_shader.SetUniform_bool("u_use_texture", true);
      asset_shader.SetUniform_int("u_texture", 0);
      asset_shader.SetUniform_vec3("u_color_to_add", Vector3(0.0f, 0.0f, 0.0f));
      asset_shader.SetUniform_vec3("u_color_to_multiply", Vector3(1.0f, 1.0f, 1.0f));
      asset_shader.SetUniform_float("u_alpha", 1.0f);

      asset_shader.SetUniform_mat4("u_model", transform);

      // For 2D rendering, we use an orthographic projection matrix, but this one uses the window as the viewfinder
      
      asset_shader.SetUniform_mat4("u_projection_view", Matrix4x4::Orthographic(-screenDimensions.x / 2, screenDimensions.x/2, -screenDimensions.y / 2, screenDimensions.y/2, -1, 1));

      // draw
      glDrawArrays(GL_TRIANGLES, 0, 6);
      m_draw_calls++;

      // error checking
      GLenum error = glGetError();
      if (error != GL_NO_ERROR)
      {
          Log::Fatal("OpenGL Error: " + std::to_string(error));
      }

      glBindVertexArray(0);
  }

 
  void OpenGLRenderer::DrawTexture2D(const Renderer2DProps& props, const Camera& cameraData)
  {
    // unit square
    static const float vertices[] = {
      // Position           // TexCoords
      -0.5f, -0.5f, 0.0f,   //0.0f, 1.0f, // Bottom-left
       0.5f, -0.5f, 0.0f,   //1.0f, 1.0f, // Bottom-right
       0.5f,  0.5f, 0.0f,   //1.0f, 0.0f, // Top-right
       0.5f,  0.5f, 0.0f,   //1.0f, 0.0f, // Top-right
      -0.5f,  0.5f, 0.0f,   //0.0f, 0.0f, // Top-left
      -0.5f, -0.5f, 0.0f,   //0.0f, 1.0f  // Bottom-left
    };

    static GLuint vao = 0, vbo = 0;

    if (vao == 0)
    {
      glGenVertexArrays(1, &vao);
      glGenBuffers(1, &vbo);

      glBindVertexArray(vao);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

      glad_glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
      //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
      //glEnableVertexAttribArray(1);
      //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

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
    if (vao == 0) return; // hey, might need to check if scale is 0 because thats what the old code does idk

    // tex coords
    bool is_spritesheet = props.texture_index >= 0;

    float tex_coords[12] = {
      0.0f, 1.0f, // Bottom-left
      1.0f, 1.0f, // Bottom-right
      1.0f, 0.0f, // Top-right
      1.0f, 0.0f, // Top-right
      0.0f, 0.0f, // Top-left
      0.0f, 1.0f  // Bottom-left
    };

    if (is_spritesheet)
    {
      auto& asset_spritesheet = FLX_ASSET_GET(Asset::Spritesheet, props.asset);
      auto uv = asset_spritesheet.GetUV(props.texture_index);
      tex_coords[0] = uv.x;
      tex_coords[1] = uv.w;
      tex_coords[2] = uv.z;
      tex_coords[3] = uv.w;
      tex_coords[4] = uv.z;
      tex_coords[5] = uv.y;
      tex_coords[6] = uv.z;
      tex_coords[7] = uv.y;
      tex_coords[8] = uv.x;
      tex_coords[9] = uv.y;
      tex_coords[10] = uv.x;
      tex_coords[11] = uv.w;
    }

    GLuint vbo_uv = 0;

    glGenBuffers(1, &vbo_uv);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_uv);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), tex_coords, GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    // free in freequeue
    FreeQueue::Push(
      [=]()
      {
        glDeleteBuffers(1, &vbo_uv);
      },
      "Free UV buffer"
    );


    // bind all
    glBindVertexArray(vao);

    auto& asset_shader = AssetManager::Get<Asset::Shader>(props.shader);
    asset_shader.Use();

    if (props.asset != "")
    {
      if (!is_spritesheet)
      {
        asset_shader.SetUniform_bool("u_use_texture", true);
        auto& asset_texture = FLX_ASSET_GET(Asset::Texture, props.asset);
        asset_texture.Bind(asset_shader, "u_texture", 0);
      }
      else
      {
        asset_shader.SetUniform_bool("u_use_texture", true);
        auto& asset_spritesheet = FLX_ASSET_GET(Asset::Spritesheet, props.asset);
        auto& asset_texture = FLX_ASSET_GET(Asset::Texture, asset_spritesheet.texture);
        asset_texture.Bind(asset_shader, "u_texture", 0);
      }
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
    asset_shader.SetUniform_float("u_alpha", props.alpha);


    // Original code for alignment, which I do not know what for
   /* Vector2 position = Vector2(props.position.x, props.position.y);
    switch (props.alignment)
    {
    case Renderer2DProps::Alignment_TopLeft:
      position += props.scale * 0.5f;
      break;
    case Renderer2DProps::Alignment_Center:
    default:
      break;
    }*/
    
    asset_shader.SetUniform_mat4("u_model", props.world_transform);

    // For 2D rendering, we use an orthographic projection matrix, but this one uses the window as the viewfinder
    asset_shader.SetUniform_mat4("u_projection_view", cameraData.GetProjViewMatrix());

    // draw
    glDrawArrays(GL_TRIANGLES, 0, 6);
    m_draw_calls++;

    // error checking
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
      Log::Fatal("OpenGL Error: " + std::to_string(error));
    }

    glBindVertexArray(0);

    // free
    FreeQueue::RemoveAndExecute("Free UV buffer");
  }

  /*!***************************************************************************
 * \brief
 * Draws a batch of 2D textures using the specified properties and batch data.
 *
 * \param props The rendering properties, including shaders, textures, and transformations.
 * \param data The batch instance data including transformation and color information.
 *****************************************************************************/
  void OpenGLRenderer::DrawBatchTexture2D(const Renderer2DProps& props, const Renderer2DSpriteBatch& data, const Camera& cameraData)
  {
      // unit square
      static const float vertices[] = {
          // Position           // TexCoords
          -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, // Bottom-left
           0.5f, -0.5f, 0.0f,   1.0f, 1.0f, // Bottom-right
           0.5f,  0.5f, 0.0f,   1.0f, 0.0f, // Top-right
           0.5f,  0.5f, 0.0f,   1.0f, 0.0f, // Top-right
          -0.5f,  0.5f, 0.0f,   0.0f, 0.0f, // Top-left
          -0.5f, -0.5f, 0.0f,   0.0f, 1.0f  // Bottom-left
      };

      static GLuint vao = 0, vbo = 0;
      static std::vector<GLuint> m_batchSSBOs;

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
      if (m_batchSSBOs.empty())
      {
          // Set up the SSBO for instance data
          GLuint t_tempSSBO;
          //Transformation
          glGenBuffers(1, &t_tempSSBO);
          m_batchSSBOs.emplace_back(t_tempSSBO);
          glBindBuffer(GL_SHADER_STORAGE_BUFFER, t_tempSSBO);
          glBufferData(GL_SHADER_STORAGE_BUFFER, m_maxInstances * sizeof(Matrix4x4), nullptr, GL_DYNAMIC_DRAW);
          glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, t_tempSSBO);
          //Uv - animation
          glGenBuffers(1, &t_tempSSBO);
          m_batchSSBOs.emplace_back(t_tempSSBO);
          glBindBuffer(GL_SHADER_STORAGE_BUFFER, t_tempSSBO);
          glBufferData(GL_SHADER_STORAGE_BUFFER, m_maxInstances * sizeof(Vector4), nullptr, GL_DYNAMIC_DRAW);
          glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, t_tempSSBO);
          //Opacity
          glGenBuffers(1, &t_tempSSBO);
          m_batchSSBOs.emplace_back(t_tempSSBO);
          glBindBuffer(GL_SHADER_STORAGE_BUFFER, t_tempSSBO);
          glBufferData(GL_SHADER_STORAGE_BUFFER, m_maxInstances * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
          glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, t_tempSSBO);

          //Color_to_add
          //glGenBuffers(1, &t_tempSSBO);
          //m_batchSSBOs.emplace_back(t_tempSSBO);
          //glBindBuffer(GL_SHADER_STORAGE_BUFFER, t_tempSSBO);
          //glBufferData(GL_SHADER_STORAGE_BUFFER, m_maxInstances * sizeof(Vector3), nullptr, GL_DYNAMIC_DRAW);
          //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, t_tempSSBO);
          //Color_to_multiply
          //glGenBuffers(1, &t_tempSSBO);
          //m_batchSSBOs.emplace_back(t_tempSSBO);
          //glBindBuffer(GL_SHADER_STORAGE_BUFFER, t_tempSSBO);
          //glBufferData(GL_SHADER_STORAGE_BUFFER, m_maxInstances * sizeof(Vector3), nullptr, GL_DYNAMIC_DRAW);
          //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, t_tempSSBO);


          FreeQueue::Push(
          [=]()
          {
              for (GLuint iter : m_batchSSBOs)
                  glDeleteBuffers(1, &iter);
          }
          );
      }

      // guard
      if (vao == 0) return; // hey, might need to check if scale is 0 because thats what the old code does idk

      // Guard
      if (data.m_transformationData.size() != data.m_opacity.size() ||
          data.m_opacity.size() != data.m_UVmap.size())
      {
          Log::Fatal("Instance batch data block is invalid (Check if all vectors are of same size)");
      }
      else if (data.m_transformationData.size() < 1)
      {
          Log::Debug("Instance batch data block is empty. Should not run render on this texture");
          return;
      }

      GLsizei dataSize = (GLsizei)data.m_transformationData.size();

      // Bind all
      glBindVertexArray(vao);

      // Apply Shader
      auto& asset_shader = AssetManager::Get<Asset::Shader>(data.m_shader);

      asset_shader.Use();

      bool is_spritesheet = data.m_UVmap[0] != Vector4(0,0,1,1);
      // Apply Texture
      if (props.asset != "")
      {
          if (!is_spritesheet)
          {
              asset_shader.SetUniform_bool("u_use_texture", true);
              auto& asset_texture = FLX_ASSET_GET(Asset::Texture, props.asset);
              asset_texture.Bind(asset_shader, "u_texture", 0);
          }
          else
          {
              asset_shader.SetUniform_bool("u_use_texture", true);
              auto& asset_spritesheet = FLX_ASSET_GET(Asset::Spritesheet, props.asset);
              auto& asset_texture = FLX_ASSET_GET(Asset::Texture, asset_spritesheet.texture);
              asset_texture.Bind(asset_shader, "u_texture", 0);
          }
      }
      else
      {
          asset_shader.SetUniform_bool("u_use_texture", false);
      }

      asset_shader.SetUniform_vec3("u_color_to_add", Vector3::Zero);
      asset_shader.SetUniform_vec3("u_color_to_multiply", Vector3::One);

      //Apply SSBOs
      glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_batchSSBOs[0]);
      glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, dataSize * sizeof(Matrix4x4), data.m_transformationData.data());
      glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_batchSSBOs[1]);
      glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, dataSize * sizeof(Vector4), data.m_UVmap.data());
      glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_batchSSBOs[2]);
      glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, dataSize * sizeof(float), data.m_opacity.data());
      glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      // For 2D rendering, we use an orthographic projection matrix, but this one uses the window as the viewfinder
      asset_shader.SetUniform_mat4("u_projection_view", cameraData.GetProjViewMatrix());
      // Draw
      glDrawArraysInstanced(GL_TRIANGLES, 0, 6, dataSize);
      m_draw_calls++;

      // error checking
      GLenum error = glGetError();
      if (error != GL_NO_ERROR)
      {
          Log::Fatal("OpenGL Error: " + std::to_string(error));
      }

      glBindVertexArray(0);
  }

  void OpenGLRenderer::DrawSimpleTexture2D(
    const Asset::Texture& texture,
    const Vector2& position,
    const Vector2& scale,
    const Vector2& window_size,
    Renderer2DProps::Alignment alignment
  )
  {
      // unit square
      static const float vertices[] = {
          // Position           // TexCoords
          -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, // Bottom-left
           0.5f, -0.5f, 0.0f,   1.0f, 1.0f, // Bottom-right
           0.5f,  0.5f, 0.0f,   1.0f, 0.0f, // Top-right
           0.5f,  0.5f, 0.0f,   1.0f, 0.0f, // Top-right
          -0.5f,  0.5f, 0.0f,   0.0f, 0.0f, // Top-left
          -0.5f, -0.5f, 0.0f,   0.0f, 1.0f  // Bottom-left
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
      if (vao == 0 || scale == Vector2::Zero) return;

      // bind all
      glBindVertexArray(vao);

      // jank optimization to run once
      static Asset::Shader texture_shader;
      static bool texture_shader_loaded = false;
      if (!texture_shader_loaded)
      {
          texture_shader.Load(Path::current("assets/shaders/texture.flxshader"));
          texture_shader_loaded = true;
      }
      texture_shader.Use();

      texture_shader.SetUniform_bool("u_use_texture", true);
      texture.Bind(texture_shader, "u_texture", 0);

      // set default values even though they are not used
      texture_shader.SetUniform_vec3("u_color", Vector3(1.0f, 1.0f, 1.0f));
      texture_shader.SetUniform_vec3("u_color_to_add", Vector3(0.0f, 0.0f, 0.0f));
      texture_shader.SetUniform_vec3("u_color_to_multiply", Vector3(1.0f, 1.0f, 1.0f)); // this is the important one

      // alignment
      Vector2 m_position = Vector2(position.x, position.y);
      switch (alignment)
      {
      case Renderer2DProps::Alignment_TopLeft:
          m_position += scale * 0.5f;
          break;
      case Renderer2DProps::Alignment_Center:
      default:
          break;
      }

      // model matrix
      Matrix4x4 model = Matrix4x4::Identity;
      texture_shader.SetUniform_mat4("u_model", model.Translate(Vector3(m_position.x, m_position.y, 0.0f)).Scale(Vector3(scale.x, scale.y, 1.0f)));

      // proj view matrix TODO
      Matrix4x4 proj_view = Matrix4x4::Orthographic(0.0f, window_size.x, 0.0f, window_size.y, -1.0f, 1.0f);
      texture_shader.SetUniform_mat4("u_projection_view", proj_view);

      // draw
      glDrawArrays(GL_TRIANGLES, 0, 6);
      m_draw_calls++;

      glBindVertexArray(0);
  }
  #pragma endregion

  #pragma region Text Rendering

  void OpenGLRenderer::DrawTexture2D(Camera const& cam, const Renderer2DText& text)
  {
      // Enable both to activate string render
      if (!FlexPrefs::GetBool("game.batching") || !FlexPrefs::GetBool("editor.batching"))
      {
          if (!CameraManager::has_main_camera) return;

          static GLuint vao = 0, vbo = 0;

          if (vao == 0)
          {
              // Configure VAO/VBO for text quads
              glGenVertexArrays(1, &vao);
              glGenBuffers(1, &vbo);
              glBindVertexArray(vao);
              glBindBuffer(GL_ARRAY_BUFFER, vbo);
              glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW); // 6 vertices per quad

              // Configure vertex attributes
              glEnableVertexAttribArray(0);
              glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
              glBindBuffer(GL_ARRAY_BUFFER, 0);
              glBindVertexArray(0);

              FreeQueue::Push(
                [=]()
              {
                  glDeleteVertexArrays(1, &vao);
                  glDeleteBuffers(1, &vbo);
              }
              );
          }

          // guard
          if (vao == 0 || text.m_shader == "") return;

          if (text.m_fonttype.empty())
          {
              Log::Info("Text Renderer: Unknown font type! Please check what you wrote!");
              return;
          }

          auto& asset_shader = FLX_ASSET_GET(Asset::Shader, "/shaders/freetypetext.flxshader");
          asset_shader.Use();

          asset_shader.SetUniform_vec3("u_color", text.m_color);
          asset_shader.SetUniform_mat4("u_model", text.m_transform);
          asset_shader.SetUniform_mat4("projection", cam.GetProjViewMatrix());

          glActiveTexture(GL_TEXTURE0);
          glBindVertexArray(vao);
          auto& asset_font = FLX_ASSET_GET(Asset::Font, text.m_fonttype);

          // Lambda to render a single glyph
          auto renderGlyph = [&](const Asset::Glyph& glyph, const Vector3& position)
          {
              glBindTexture(GL_TEXTURE_2D, glyph.textureID);

              float xpos = position.x + glyph.bearing.x;
              float ypos = position.y - (glyph.bearing.y - glyph.size.y);
              float w = glyph.size.x;
              float h = -glyph.size.y;

              float quadVertices[6][4] =
              {
                  {xpos, -ypos - h, 0.0f, 0.0f},
                  {xpos, -ypos, 0.0f, 1.0f},
                  {xpos + w, -ypos, 1.0f, 1.0f},
                  {xpos, -ypos - h, 0.0f, 0.0f},
                  {xpos + w, -ypos, 1.0f, 1.0f},
                  {xpos + w, -ypos - h, 1.0f, 0.0f}
              };

              glBindBuffer(GL_ARRAY_BUFFER, vbo);
              glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quadVertices), quadVertices);
              glDrawArrays(GL_TRIANGLES, 0, 6);
              m_draw_calls++;
          };

          // Function to calculate text box dimensions
          auto calculateTextBoxDimensions = [&](const std::string& words) -> std::pair<float, float>
          {
              float totalHeight = 0.0f, maxLineHeight = asset_font.GetGlyph('A').size.y;
              float maxWidth = 0.0f, currentLineWidth = 0.0f;

              for (char c : words)
              {
                  if (c == '\n')  // Handle explicit line breaks
                  {
                      maxWidth = std::max(maxWidth, currentLineWidth);
                      totalHeight += maxLineHeight;
                      currentLineWidth = 0.0f;
                      maxLineHeight = asset_font.GetGlyph('A').size.y; // Reset line height for new line
                      continue;
                  }

                  const Asset::Glyph& glyph = asset_font.GetGlyph(c);

                  // Update line width and max line height for the current line
                  currentLineWidth += glyph.advance + static_cast<int>(text.m_letterspacing);
                  maxLineHeight = std::max(maxLineHeight, glyph.size.y);

                  // Check if the current line exceeds the max width of the text box
                  if (currentLineWidth > text.m_textboxDimensions.x)
                  {
                      maxWidth = std::max(maxWidth, currentLineWidth - glyph.advance); // Exclude overflow
                      totalHeight += maxLineHeight;
                      currentLineWidth = static_cast<float>(glyph.advance) + text.m_letterspacing; // Start new line
                      maxLineHeight = glyph.size.y; // Reset line height for the new line

                      // Check if total height exceeds the maximum allowed height
                      if (totalHeight + maxLineHeight > text.m_textboxDimensions.y)
                      {
                          totalHeight -= maxLineHeight; // Remove the extra line's height
                          break;
                      }
                  }
              }

              // Account for the last line's dimensions
              if (currentLineWidth > 0)
              {
                  maxWidth = std::max(maxWidth, currentLineWidth);
                  totalHeight += maxLineHeight;
              }

              // Ensure dimensions do not exceed the max text box constraints
              return { std::min(maxWidth, text.m_textboxDimensions.x), std::min(totalHeight, text.m_textboxDimensions.y) };
          };

          // Function to set alignment
          auto getAlignmentOffset = [&](std::pair<Renderer2DText::AlignmentX, Renderer2DText::AlignmentY> alignment, const std::string& words) -> Vector2
          {
              auto [lineWidth, totalHeight] = calculateTextBoxDimensions(words);

              float alignXOffset = (alignment.first == Renderer2DText::Alignment_Center) ? -lineWidth / 2.0f :
                  (alignment.first == Renderer2DText::Alignment_Right) ? -lineWidth : 0.0f;

              float alignYOffset = (alignment.second == Renderer2DText::Alignment_Middle) ? -totalHeight / 2.0f :
                  (alignment.second == Renderer2DText::Alignment_Bottom) ? -totalHeight : 0.0f;

              return { alignXOffset, alignYOffset };
          };

          // Lambda to categorize and process words
          auto splitIntoWords = [&](const std::string& input) -> std::vector<std::string>
          {
              std::vector<std::string> words;
              std::string currentWord;
              for (char c : input)
              {
                  if (c == ' ' || c == '\n')
                  {
                      if (!currentWord.empty())
                      {
                          words.push_back(currentWord);
                          currentWord.clear();
                      }
                      if (c == '\n')
                          words.push_back("\n"); // Explicit newline
                  }
                  else
                      currentWord += c;
              }
              if (!currentWord.empty())
                  words.push_back(currentWord);

              return words;
          };

          // Refactored logic -> Primitive Text Scrolling
          auto words = splitIntoWords(text.m_words);
          Vector2 pen_pos = getAlignmentOffset(text.m_alignment, text.m_words); // Initial alignment offset
          float lineWidth = 0.0f, totalHeight = 0.0f, maxLineHeight = 0.0f;
          const Asset::Glyph& space = asset_font.GetGlyph(' ');
          std::string currentLine;

          auto renderLine = [&](const std::string& line)
          {
              pen_pos.x = getAlignmentOffset(text.m_alignment, line).x;
              for (char c : line)
              {
                  const Asset::Glyph& glyph = asset_font.GetGlyph(c);
                  renderGlyph(glyph, pen_pos);
                  pen_pos.x += glyph.advance + text.m_letterspacing;
              }
          };

          for (const auto& word : words)
          {
              if (word == "\n") // Handle explicit line break
              {
                  renderLine(currentLine);

                  // Move to the next line
                  pen_pos.y += maxLineHeight + text.m_linespacing;
                  lineWidth = 0.0f;
                  maxLineHeight = 0.0f;
                  currentLine.clear();
                  continue;
              }

              float wordWidth = 0.0f;
              float wordHeight = 0.0f;
              for (char c : word)
              {
                  const Asset::Glyph& glyph = asset_font.GetGlyph(c);
                  wordWidth += glyph.advance + text.m_letterspacing;
                  wordHeight = std::max(wordHeight, glyph.size.y);
              }

              // Check if the word fits in the current line
              if (lineWidth + wordWidth > text.m_textboxDimensions.x)
              {
                  renderLine(currentLine);

                  // Move to the next line
                  pen_pos.y += maxLineHeight + text.m_linespacing;
                  totalHeight += maxLineHeight + text.m_linespacing;
                  lineWidth = 0.0f;
                  maxLineHeight = 0.0f;

                  // Stop rendering if vertical overflow occurs
                  if (totalHeight + wordHeight > text.m_textboxDimensions.y)
                  {
                      currentLine.clear();
                      break;
                  }

                  // Start a new line
                  currentLine = word;
                  lineWidth = wordWidth;
                  maxLineHeight = wordHeight;
              }
              else // Add the word & space to the current line 
              {
                  if (!currentLine.empty()) currentLine += " ";
                  currentLine += word;
                  lineWidth += wordWidth + space.advance + text.m_letterspacing;
                  maxLineHeight = std::max(maxLineHeight, wordHeight);
              }
          }

          // Render the last line
          if (!currentLine.empty()) renderLine(currentLine);

          // Cleanup
          glBindVertexArray(0);
          glBindTexture(GL_TEXTURE_2D, 0);
      }
      else
      {

          //Current Known issues
         // 1. Words are being unnecessarily cut to new lines compared to old code in moves (Hour Of Reckoning) during combat -> pls extend your textbox size slightly
         // let me know if there are any more issues

         // Early-out if missing main camera, shader, or font.
          if (text.m_shader == "" ||
              text.m_fonttype.empty())
          {
              Log::Info("Text Renderer: Missing camera, shader or font!");
              return;
          }

          // --- Setup a static VAO/VBO for a single character quad (6 vertices) ---
          static GLuint vao = 0, vbo = 0;
          static bool initialized = false;
          if (!initialized)
          {
              // Define a unit quad in normalized space [0,1] for each character.
              float quadVertices[] = {
                  // positions (x,y)
                  0.0f, 0.0f,
                  0.0f, 1.0f,
                  1.0f, 1.0f,

                  0.0f, 0.0f,
                  1.0f, 1.0f,
                  1.0f, 0.0f
              };

              glGenVertexArrays(1, &vao);
              glGenBuffers(1, &vbo);
              glBindVertexArray(vao);
              glBindBuffer(GL_ARRAY_BUFFER, vbo);
              glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
              glEnableVertexAttribArray(0);
              glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
              glBindBuffer(GL_ARRAY_BUFFER, 0);
              glBindVertexArray(0);
              initialized = true;

              // Push a free callback to delete vao/vbo on shutdown.
              FreeQueue::Push([=]()
              {
                  glDeleteVertexArrays(1, &vao);
                  glDeleteBuffers(1, &vbo);
              });
          }

          // --- Use shader and set common uniforms ---
          auto& asset_shader = FLX_ASSET_GET(Asset::Shader, text.m_shader);
          asset_shader.Use();
          asset_shader.SetUniform_mat4("u_model", text.m_transform);
          asset_shader.SetUniform_mat4("u_projection", cam.GetProjViewMatrix());
          asset_shader.SetUniform_vec3("u_color", text.m_color);

          // --- Bind the atlas texture (which holds all glyphs) ---
          auto& asset_font = FLX_ASSET_GET(Asset::Font, text.m_fonttype);
          glActiveTexture(GL_TEXTURE0);
          glBindTexture(GL_TEXTURE_2D, asset_font.GetAtlasTexture());
          asset_shader.SetUniform_int("u_texture", 0);

          // --- Pass layout parameters ---
          asset_shader.SetUniform_float("u_letterSpacing", text.m_letterspacing);
          asset_shader.SetUniform_float("u_lineSpacing", text.m_linespacing);
          asset_shader.SetUniform_float("u_textboxWidth", text.m_textboxDimensions.x);
          asset_shader.SetUniform_float("u_textboxHeight", text.m_textboxDimensions.y);

          // Map text alignment enums to floats:
          // Left = 0.0, Center = 0.5, Right = 1.0 (for X)
          // Top = 0.0, Middle = 0.5, Bottom = 1.0 (for Y)
          float alignX = (text.m_alignment.first == Renderer2DText::Alignment_Center) ? 0.5f :
              (text.m_alignment.first == Renderer2DText::Alignment_Right) ? 1.0f : 0.0f;
          float alignY = (text.m_alignment.second == Renderer2DText::Alignment_Middle) ? 0.5f :
              (text.m_alignment.second == Renderer2DText::Alignment_Bottom) ? 1.0f : 0.0f;

          // --- Pass glyph metrics for ASCII characters (0-127) ---
          Asset::GlyphMetric glyphMetrics[128];
          for (int i = 0; i < 128; i++)
          {
              const Asset::Glyph& g = asset_font.GetGlyph(static_cast<char>(i));
              glyphMetrics[i].advance = static_cast<float>(g.advance);
              glyphMetrics[i].size[0] = g.size.x;
              glyphMetrics[i].size[1] = g.size.y;
              glyphMetrics[i].bearing[0] = g.bearing.x;
              glyphMetrics[i].bearing[1] = g.bearing.y;
              glyphMetrics[i].uvOffset[0] = g.uvOffset.x;
              glyphMetrics[i].uvOffset[1] = g.uvOffset.y;
              glyphMetrics[i].uvSize[0] = g.uvSize.x;
              glyphMetrics[i].uvSize[1] = g.uvSize.y;
          }
          asset_shader.SetUniformGlyphMetrics("u_glyphs", const_cast<const Asset::GlyphMetric*>(glyphMetrics), 128);

          // --- CPU-side text splitting ---
          // Use a lambda (without any custom struct) to split the text into lines.
          // Each element in the resulting vector is a pair: (line text, line width).
          auto splitLines = [&]() -> std::vector<std::pair<std::string, float>> {
              std::vector<std::pair<std::string, float>> lines;
              std::string currentLine;
              float currentWidth = 0.0f;
              int lastSpaceIndex = -1;
              float widthAtLastSpace = 0.0f;
              for (size_t i = 0; i < text.m_words.size(); i++)
              {
                  char c = text.m_words[i];
                  if (c == '\n')
                  {
                      // Push the current line as-is.
                      lines.push_back({ currentLine, currentWidth });
                      currentLine = "";
                      currentWidth = 0.0f;
                      lastSpaceIndex = -1;
                      widthAtLastSpace = 0.0f;
                  }
                  else
                  {
                      float advance = asset_font.GetGlyph(c).advance + text.m_letterspacing;
                      // If a space, record its position.
                      if (c == ' ')
                      {
                          lastSpaceIndex = (int)currentLine.size();
                          widthAtLastSpace = currentWidth;
                      }
                      // If adding this character exceeds the textbox width (and current line isnt empty):
                      if (!currentLine.empty() && (currentWidth + advance > text.m_textboxDimensions.x))
                      {
                          if (lastSpaceIndex != -1)
                          {
                              // Ensure lastSpaceIndex is within the bounds of currentLine.
                              int safeIndex = std::min(lastSpaceIndex, static_cast<int>(currentLine.size()));
                              // Break the line at the last space.
                              std::string lineToPush = currentLine.substr(0, safeIndex);
                              float lineWidth = widthAtLastSpace;
                              lines.push_back({ lineToPush, lineWidth });

                              // Start the new line with the word after the space.
                              std::string remaining = "";
                              if (safeIndex + 1 < (int)currentLine.size())
                                  remaining = currentLine.substr(safeIndex + 1);
                              currentLine = remaining;

                              // Recalculate currentWidth from the remaining word.
                              currentWidth = 0.0f;
                              for (char rc : remaining)
                              {
                                  currentWidth += asset_font.GetGlyph(rc).advance + text.m_letterspacing;
                              }

                              // Then add the current character.
                              currentLine.push_back(c);
                              currentWidth += advance;

                              // Reset space tracking.
                              lastSpaceIndex = -1;
                              widthAtLastSpace = 0.0f;
                          }
                          else
                          {
                              // No space was found; break immediately.
                              lines.push_back({ currentLine, currentWidth });
                              currentLine = "";
                              currentWidth = 0.0f;
                              currentLine.push_back(c);
                              currentWidth += advance;
                          }
                          // Reset space tracking.
                          lastSpaceIndex = -1;
                          widthAtLastSpace = 0.0f;
                      }
                      else
                      {
                          // Append the character.
                          currentLine.push_back(c);
                          currentWidth += advance;
                      }
                  }
              }
              if (!currentLine.empty())
                  lines.push_back({ currentLine, currentWidth });
              return lines;
          };

          auto lines = splitLines();

          // --- Compute vertical layout ---
          // Use the glyph for 'A' as the baseline line height.
          float lineHeight = asset_font.GetGlyph('A').size.y;
          int numLines = (int)lines.size();
          // Total text block height: line heights plus line spacing between lines.
          float totalTextHeight = numLines * lineHeight + (numLines - 1) * text.m_linespacing;
          // Compute vertical offset based on alignment:
          // For top (0.0) we use 0, for middle (0.5) we shift by half the total height,
          // for bottom (1.0) we shift by the total height.
          float verticalOffset = (alignY == 0.5f) ? totalTextHeight * 0.5f : (alignY == 1.0f ? totalTextHeight : 0.0f);

          // --- Render each line separately ---
          glBindVertexArray(vao);
          float currentBaseline = verticalOffset;
          const int maxTextLength = 256; // maximum supported per line
          for (size_t i = 0; i < lines.size(); i++)
          {
              const std::string& lineText = lines[i].first;
              float lineWidth = lines[i].second;
              // Compute horizontal alignment offset:
              float lineOffsetX = (alignX == 0.5f) ? -lineWidth * 0.5f : (alignX == 1.0f ? -lineWidth : 0.0f);
              // Set per-line offset uniform ("u_lineOffset" should be declared as vec2 in your shader).
              asset_shader.SetUniform_vec2("u_lineOffset", { lineOffsetX, currentBaseline });

              // Prepare the text array for this line.
              int lineLength = static_cast<int>(lineText.size());
              asset_shader.SetUniform_int("u_textLength", lineLength);
              int textArray[maxTextLength] = { 0 };
              for (int j = 0; j < lineLength && j < maxTextLength; j++)
              {
                  textArray[j] = static_cast<int>(lineText[j]);
              }
              asset_shader.SetUniform_int_array("u_text", textArray, maxTextLength);

              // Draw the line.
              glDrawArraysInstanced(GL_TRIANGLES, 0, 6, lineLength);
              m_draw_calls++;

              // Move the baseline down for the next line.
              currentBaseline -= (lineHeight + text.m_linespacing);
          }
          glBindVertexArray(0);
          glBindTexture(GL_TEXTURE_2D, 0);
      }
  }

  void OpenGLRenderer::DrawTexture2D(const Renderer2DText& text, const Camera& cameraData)
  {
      if (!CameraManager::has_main_camera) return;

      static GLuint vao = 0, vbo = 0;

      if (vao == 0)
      {
          // Configure VAO/VBO for text quads
          glGenVertexArrays(1, &vao);
          glGenBuffers(1, &vbo);
          glBindVertexArray(vao);
          glBindBuffer(GL_ARRAY_BUFFER, vbo);
          glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW); // 6 vertices per quad

          // Configure vertex attributes
          glEnableVertexAttribArray(0);
          glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
          glBindBuffer(GL_ARRAY_BUFFER, 0);
          glBindVertexArray(0);

          FreeQueue::Push(
            [=]()
          {
              glDeleteVertexArrays(1, &vao);
              glDeleteBuffers(1, &vbo);
          }
          );
      }

      // guard
      if (vao == 0 || text.m_shader == "") return;

      if (text.m_fonttype.empty())
      {
          Log::Info("Text Renderer: Unknown font type! Please check what you wrote!");
          return;
      }

      auto& asset_shader = FLX_ASSET_GET(Asset::Shader, "/shaders/freetypetext.flxshader");
      asset_shader.Use();

      asset_shader.SetUniform_vec3("u_color", text.m_color);
      asset_shader.SetUniform_mat4("u_model", text.m_transform);
      asset_shader.SetUniform_mat4("projection", cameraData.GetProjViewMatrix());

      glActiveTexture(GL_TEXTURE0);
      glBindVertexArray(vao);
      auto& asset_font = FLX_ASSET_GET(Asset::Font, text.m_fonttype);

      // Lambda to render a single glyph
      auto renderGlyph = [&](const Asset::Glyph& glyph, const Vector3& position)
      {
          glBindTexture(GL_TEXTURE_2D, glyph.textureID);

          float xpos = position.x + glyph.bearing.x;
          float ypos = position.y - (glyph.bearing.y - glyph.size.y);
          float w = glyph.size.x;
          float h = -glyph.size.y;

          float quadVertices[6][4] =
          {
              {xpos, -ypos - h, 0.0f, 0.0f},
              {xpos, -ypos, 0.0f, 1.0f},
              {xpos + w, -ypos, 1.0f, 1.0f},
              {xpos, -ypos - h, 0.0f, 0.0f},
              {xpos + w, -ypos, 1.0f, 1.0f},
              {xpos + w, -ypos - h, 1.0f, 0.0f}
          };

          glBindBuffer(GL_ARRAY_BUFFER, vbo);
          glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quadVertices), quadVertices);
          glDrawArrays(GL_TRIANGLES, 0, 6);
          m_draw_calls++;
      };

      // Function to calculate text box dimensions
      auto calculateTextBoxDimensions = [&](const std::string& words) -> std::pair<float, float>
      {
          float totalHeight = 0.0f, maxLineHeight = asset_font.GetGlyph('A').size.y;
          float maxWidth = 0.0f, currentLineWidth = 0.0f;

          for (char c : words)
          {
              if (c == '\n')  // Handle explicit line breaks
              {
                  maxWidth = std::max(maxWidth, currentLineWidth);
                  totalHeight += maxLineHeight;
                  currentLineWidth = 0.0f;
                  maxLineHeight = asset_font.GetGlyph('A').size.y; // Reset line height for new line
                  continue;
              }

              const Asset::Glyph& glyph = asset_font.GetGlyph(c);

              // Update line width and max line height for the current line
              currentLineWidth += glyph.advance + static_cast<int>(text.m_letterspacing);
              maxLineHeight = std::max(maxLineHeight, glyph.size.y);

              // Check if the current line exceeds the max width of the text box
              if (currentLineWidth > text.m_textboxDimensions.x)
              {
                  maxWidth = std::max(maxWidth, currentLineWidth - glyph.advance); // Exclude overflow
                  totalHeight += maxLineHeight;
                  currentLineWidth = static_cast<float>(glyph.advance) + text.m_letterspacing; // Start new line
                  maxLineHeight = glyph.size.y; // Reset line height for the new line

                  // Check if total height exceeds the maximum allowed height
                  if (totalHeight + maxLineHeight > text.m_textboxDimensions.y)
                  {
                      totalHeight -= maxLineHeight; // Remove the extra line's height
                      break;
                  }
              }
          }

          // Account for the last line's dimensions
          if (currentLineWidth > 0)
          {
              maxWidth = std::max(maxWidth, currentLineWidth);
              totalHeight += maxLineHeight;
          }

          // Ensure dimensions do not exceed the max text box constraints
          return { std::min(maxWidth, text.m_textboxDimensions.x), std::min(totalHeight, text.m_textboxDimensions.y) };
      };

      // Function to set alignment
      auto getAlignmentOffset = [&](std::pair<Renderer2DText::AlignmentX, Renderer2DText::AlignmentY> alignment, const std::string& words) -> Vector2
      {
          auto [lineWidth, totalHeight] = calculateTextBoxDimensions(words);

          float alignXOffset = (alignment.first == Renderer2DText::Alignment_Center) ? -lineWidth / 2.0f :
              (alignment.first == Renderer2DText::Alignment_Right) ? -lineWidth : 0.0f;

          float alignYOffset = (alignment.second == Renderer2DText::Alignment_Middle) ? -totalHeight / 2.0f :
              (alignment.second == Renderer2DText::Alignment_Bottom) ? -totalHeight : 0.0f;

          return { alignXOffset, alignYOffset };
      };

      // Lambda to categorize and process words
      auto splitIntoWords = [&](const std::string& input) -> std::vector<std::string>
      {
          std::vector<std::string> words;
          std::string currentWord;
          for (char c : input)
          {
              if (c == ' ' || c == '\n')
              {
                  if (!currentWord.empty())
                  {
                      words.push_back(currentWord);
                      currentWord.clear();
                  }
                  if (c == '\n')
                      words.push_back("\n"); // Explicit newline
              }
              else
                  currentWord += c;
          }
          if (!currentWord.empty())
              words.push_back(currentWord);

          return words;
      };

      // Refactored logic -> Primitive Text Scrolling
      #if 1
      {
          auto words = splitIntoWords(text.m_words);
          Vector2 pen_pos = getAlignmentOffset(text.m_alignment, text.m_words); // Initial alignment offset
          float lineWidth = 0.0f, totalHeight = 0.0f, maxLineHeight = 0.0f;
          const Asset::Glyph& space = asset_font.GetGlyph(' ');
          std::string currentLine;

          auto renderLine = [&](const std::string& line)
          {
              pen_pos.x = getAlignmentOffset(text.m_alignment, line).x;
              for (char c : line)
              {
                  const Asset::Glyph& glyph = asset_font.GetGlyph(c);
                  renderGlyph(glyph, pen_pos);
                  pen_pos.x += glyph.advance + text.m_letterspacing;
              }
          };

          for (const auto& word : words)
          {
              if (word == "\n") // Handle explicit line break
              {
                  renderLine(currentLine);

                  // Move to the next line
                  pen_pos.y += maxLineHeight + text.m_linespacing;
                  lineWidth = 0.0f;
                  maxLineHeight = 0.0f;
                  currentLine.clear();
                  continue;
              }

              float wordWidth = 0.0f;
              float wordHeight = 0.0f;
              for (char c : word)
              {
                  const Asset::Glyph& glyph = asset_font.GetGlyph(c);
                  wordWidth += glyph.advance + text.m_letterspacing;
                  wordHeight = std::max(wordHeight, glyph.size.y);
              }

              // Check if the word fits in the current line
              if (lineWidth + wordWidth > text.m_textboxDimensions.x)
              {
                  renderLine(currentLine);

                  // Move to the next line
                  pen_pos.y += maxLineHeight + text.m_linespacing;
                  totalHeight += maxLineHeight + text.m_linespacing;
                  lineWidth = 0.0f;
                  maxLineHeight = 0.0f;

                  // Stop rendering if vertical overflow occurs
                  if (totalHeight + wordHeight > text.m_textboxDimensions.y)
                  {
                      currentLine.clear();
                      break;
                  }

                  // Start a new line
                  currentLine = word;
                  lineWidth = wordWidth;
                  maxLineHeight = wordHeight;
              }
              else // Add the word & space to the current line 
              {
                  if (!currentLine.empty()) currentLine += " ";
                  currentLine += word;
                  lineWidth += wordWidth + space.advance + text.m_letterspacing;
                  maxLineHeight = std::max(maxLineHeight, wordHeight);
              }
          }

          // Render the last line
          if (!currentLine.empty()) renderLine(currentLine);
      }
      #endif

      // Cleanup
      glBindVertexArray(0);
      glBindTexture(GL_TEXTURE_2D, 0);
  }
  #pragma endregion

  #pragma region Post Processing

  /*!***************************************************************************
  * \brief
  * Applies a brightness threshold pass for the bloom effect.
  *
  * \param threshold The brightness threshold to apply.
  *****************************************************************************/
  void OpenGLRenderer::ApplyBrightnessPass(const GLuint& texture, float threshold)
  {
      #pragma region VAO setup
      // Full-screen quad covering clip space.
      static const float vertices[] = {
          // Position           // TexCoords
          -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, // Bottom-left
           1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // Bottom-right
           1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // Top-right
           1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // Top-right
          -1.0f,  1.0f, 0.0f,   0.0f, 1.0f, // Top-left
          -1.0f, -1.0f, 0.0f,   0.0f, 0.0f  // Bottom-left
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

      // Bind our VAO for drawing.
      glBindVertexArray(vao);
      #pragma endregion

      // Use the brightness pass shader.
      auto& asset_shader = FLX_ASSET_GET(Asset::Shader, "/shaders/Bloom_BrightnessPass.flxshader");
      asset_shader.Use();
      asset_shader.SetUniform_float("u_Threshold", threshold);

      // Bind the input texture to texture unit 0.
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture);
      asset_shader.SetUniform_int("u_texture", 0);

      // Draw the full-screen quad.
      glDrawArrays(GL_TRIANGLES, 0, 6);
      m_draw_calls++;

      // Error checking.
      GLenum error = glGetError();
      if (error != GL_NO_ERROR)
      {
          Log::Fatal("OpenGL Error: " + std::to_string(error));
      }

      glBindVertexArray(0);
  }

  ///*!***************************************************************************
  //* \brief
  //* Applies a Gaussian blur effect with specified passes, blur distance, and intensity.
  //*
  //* \param blurDrawPasses The number of passes to apply for the blur.
  //* \param blurDistance The distance factor for the blur effect.
  //* \param intensity The intensity of the blur.
  //*****************************************************************************/
  void OpenGLRenderer::ApplyGaussianBlur(const GLuint& texture, float blurDistance, int blurIntensity, bool isHorizontal)
  {
      #pragma region VAO setup
      // Full-screen quad covering clip space.
      static const float vertices[] = {
          // Position           // TexCoords
          -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, // Bottom-left
           1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // Bottom-right
           1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // Top-right
           1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // Top-right
          -1.0f,  1.0f, 0.0f,   0.0f, 1.0f, // Top-left
          -1.0f, -1.0f, 0.0f,   0.0f, 0.0f  // Bottom-left
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

      // Bind our VAO for drawing.
      glBindVertexArray(vao);
      #pragma endregion

      auto& asset_shader = FLX_ASSET_GET(Asset::Shader, "/shaders/Gaussian_Blur.flxshader");
     
      asset_shader.SetUniform_int("horizontal", isHorizontal);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture);
      asset_shader.SetUniform_int("u_texture", 0);
      asset_shader.SetUniform_float("blurDistance", blurDistance);
      asset_shader.SetUniform_int("intensity", blurIntensity);
      
      glDrawArrays(GL_TRIANGLES, 0, 6);
      m_draw_calls++;
  }

  ///*!***************************************************************************
  //* \brief
  //* Applies the final bloom composition with a specified opacity level.
  //*
  //* \param opacity The opacity level for the bloom composition.
  //*****************************************************************************/
  void OpenGLRenderer::ApplyBloomFinalComposition(const GLuint& texture, const GLuint& blurtextureHorizontal, const GLuint& blurtextureVertical, float opacity, float spread)
  {
      #pragma region VAO setup
      // Full-screen quad covering clip space.
      static const float vertices[] = {
          // Position           // TexCoords
          -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, // Bottom-left
           1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // Bottom-right
           1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // Top-right
           1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // Top-right
          -1.0f,  1.0f, 0.0f,   0.0f, 1.0f, // Top-left
          -1.0f, -1.0f, 0.0f,   0.0f, 0.0f  // Bottom-left
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

      // Bind our VAO for drawing.
      glBindVertexArray(vao);
      #pragma endregion

      auto& asset_shader = FLX_ASSET_GET(Asset::Shader, "/shaders/Bloom_final_composite.flxshader");
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture); // Original scene texture
      asset_shader.SetUniform_int("screenTex", 0);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, blurtextureVertical); // Blur Vertical
      asset_shader.SetUniform_int("bloomVTex", 1);
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, blurtextureHorizontal); // Blur Horizontal
      asset_shader.SetUniform_int("bloomHTex", 2);
      asset_shader.SetUniform_float("opacity", opacity);
      asset_shader.SetUniform_float("bloomRadius", spread);

      glDrawArrays(GL_TRIANGLES, 0, 6);
      m_draw_calls++;
  }

  void OpenGLRenderer::ApplyBlurFinalComposition(const GLuint& blurtextureHorizontal, const GLuint& blurtextureVertical)
  {
      #pragma region VAO setup
      // Full-screen quad covering clip space.
      static const float vertices[] = {
          // Position           // TexCoords
          -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, // Bottom-left
           1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // Bottom-right
           1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // Top-right
           1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // Top-right
          -1.0f,  1.0f, 0.0f,   0.0f, 1.0f, // Top-left
          -1.0f, -1.0f, 0.0f,   0.0f, 0.0f  // Bottom-left
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

      // Bind our VAO for drawing.
      glBindVertexArray(vao);
      #pragma endregion

      auto& asset_shader = FLX_ASSET_GET(Asset::Shader, "/shaders/Blur_final_composite.flxshader");
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, blurtextureHorizontal); // Original scene texture
      asset_shader.SetUniform_int("blurHTex", 0);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, blurtextureVertical); // Blur Vertical
      asset_shader.SetUniform_int("blurVTex", 1);

      glDrawArrays(GL_TRIANGLES, 0, 6);
      m_draw_calls++;
  }


  void OpenGLRenderer::ApplyChromaticAberration(const GLuint& inputTex, float chromaIntensity, const Vector2& redOffset, const Vector2& greenOffset, const Vector2& blueOffset, const Vector2& EdgeRadius, const Vector2& EdgeSoftness)
  {
      #pragma region VAO Setup
      // Full-screen quad covering clip space.
      static const float vertices[] = {
          // Positions           // TexCoords
          -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, // Bottom-left
           1.0f, -1.0f, 0.0f,     1.0f, 0.0f, // Bottom-right
           1.0f,  1.0f, 0.0f,     1.0f, 1.0f, // Top-right
           1.0f,  1.0f, 0.0f,     1.0f, 1.0f, // Top-right
          -1.0f,  1.0f, 0.0f,     0.0f, 1.0f, // Top-left
          -1.0f, -1.0f, 0.0f,     0.0f, 0.0f  // Bottom-left
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

          // Push cleanup into your free queue.
          FreeQueue::Push([=]()
          {
              glDeleteBuffers(1, &vbo);
              glDeleteVertexArrays(1, &vao);
          });
      }
      glBindVertexArray(vao);
      #pragma endregion

      // Retrieve the chromatic aberration shader asset.
      auto& asset_shader = FLX_ASSET_GET(Asset::Shader, "/shaders/Chromatic_aberration.flxshader");

      // Bind the input texture to texture unit 0.
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, inputTex);
      asset_shader.SetUniform_int("u_InputTex", 0);

      // Set the chromatic aberration parameters.
      asset_shader.SetUniform_float("u_ChromaIntensity", chromaIntensity);
      asset_shader.SetUniform_vec2("u_RedOffset", redOffset);
      asset_shader.SetUniform_vec2("u_GreenOffset", greenOffset);
      asset_shader.SetUniform_vec2("u_BlueOffset", blueOffset);

      asset_shader.SetUniform_vec2("u_EdgeRadius", EdgeRadius);
      asset_shader.SetUniform_vec2("u_EdgeSoftness", EdgeSoftness);

      // Draw the full-screen quad.
      glDrawArrays(GL_TRIANGLES, 0, 6);
      m_draw_calls++;
  }

  void OpenGLRenderer::ApplyColorGrading(const GLuint& inputTex, float brightness, float contrast, float saturation)
  {
      #pragma region VAO Setup
      // Full-screen quad covering clip space.
      static const float vertices[] = {
          // Positions           // TexCoords
          -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, // Bottom-left
           1.0f, -1.0f, 0.0f,     1.0f, 0.0f, // Bottom-right
           1.0f,  1.0f, 0.0f,     1.0f, 1.0f, // Top-right
           1.0f,  1.0f, 0.0f,     1.0f, 1.0f, // Top-right
          -1.0f,  1.0f, 0.0f,     0.0f, 1.0f, // Top-left
          -1.0f, -1.0f, 0.0f,     0.0f, 0.0f  // Bottom-left
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

          // Push cleanup into your free queue.
          FreeQueue::Push([=]() {
              glDeleteBuffers(1, &vbo);
              glDeleteVertexArrays(1, &vao);
          });
      }
      glBindVertexArray(vao);
      #pragma endregion

      // Retrieve the color grading shader asset.
      auto& asset_shader = FLX_ASSET_GET(Asset::Shader, "/shaders/Color_grading.flxshader");

      // Bind the input texture to texture unit 0.
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, inputTex);
      asset_shader.SetUniform_int("u_InputTex", 0);

      // Set the color grading parameters.
      asset_shader.SetUniform_float("u_Brightness", brightness);
      asset_shader.SetUniform_float("u_Contrast", contrast);
      asset_shader.SetUniform_float("u_Saturation", saturation);

      // Draw the full-screen quad.
      glDrawArrays(GL_TRIANGLES, 0, 6);
      m_draw_calls++;
  }

  void OpenGLRenderer::ApplyVignette(const GLuint& inputTex, float vignetteIntensity, const Vector2& vignetteRadius, const Vector2& vignetteSoftness)
  {
      #pragma region VAO Setup
      // Full-screen quad covering clip space.
      static const float vertices[] = {
          // Positions           // TexCoords
          -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, // Bottom-left
           1.0f, -1.0f, 0.0f,     1.0f, 0.0f, // Bottom-right
           1.0f,  1.0f, 0.0f,     1.0f, 1.0f, // Top-right
           1.0f,  1.0f, 0.0f,     1.0f, 1.0f, // Top-right
          -1.0f,  1.0f, 0.0f,     0.0f, 1.0f, // Top-left
          -1.0f, -1.0f, 0.0f,     0.0f, 0.0f  // Bottom-left
      };

      static GLuint vao = 0, vbo = 0;
      if (vao == 0)
      {
          glGenVertexArrays(1, &vao);
          glGenBuffers(1, &vbo);

          glBindVertexArray(vao);
          glBindBuffer(GL_ARRAY_BUFFER, vbo);
          glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

          glEnableVertexAttribArray(0); // Position attribute
          glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
          glEnableVertexAttribArray(1); // TexCoord attribute
          glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
          glBindVertexArray(0);

          // Push cleanup code to your free queue.
          FreeQueue::Push([=]() {
              glDeleteBuffers(1, &vbo);
              glDeleteVertexArrays(1, &vao);
          });
      }
      glBindVertexArray(vao);
      #pragma endregion

      // Retrieve the vignette shader asset.
      auto& asset_shader = FLX_ASSET_GET(Asset::Shader, "/shaders/Vignette.flxshader");

      // Bind the input texture (e.g., the current screen texture) to texture unit 0.
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, inputTex); 
      asset_shader.SetUniform_int("u_InputTex", 0);

      // Set the vignette parameters.
      asset_shader.SetUniform_float("u_VignetteIntensity", vignetteIntensity);
      asset_shader.SetUniform_vec2("u_VignetteRadius", vignetteRadius);
      asset_shader.SetUniform_vec2("u_VignetteSoftness", vignetteSoftness);

      // Draw the full-screen quad.
      glDrawArrays(GL_TRIANGLES, 0, 6);
      m_draw_calls++;
  }

  void OpenGLRenderer::ApplyFilmGrain(const GLuint& inputTex,float filmGrainIntensity,float filmGrainSize,bool filmGrainAnimate)
  {
      #pragma region VAO Setup
      // Full-screen quad covering clip space.
      static const float vertices[] = {
          // Positions           // TexCoords
          -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, // Bottom-left
           1.0f, -1.0f, 0.0f,     1.0f, 0.0f, // Bottom-right
           1.0f,  1.0f, 0.0f,     1.0f, 1.0f, // Top-right
           1.0f,  1.0f, 0.0f,     1.0f, 1.0f, // Top-right
          -1.0f,  1.0f, 0.0f,     0.0f, 1.0f, // Top-left
          -1.0f, -1.0f, 0.0f,     0.0f, 0.0f  // Bottom-left
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

          // Push cleanup into your free queue.
          FreeQueue::Push([=]()
          {
              glDeleteBuffers(1, &vbo);
              glDeleteVertexArrays(1, &vao);
          });
      }
      glBindVertexArray(vao);
      #pragma endregion

      // Retrieve the film grain shader asset.
      auto& asset_shader = FLX_ASSET_GET(Asset::Shader, "/shaders/FilmGrain.flxshader");

      // Bind the input texture to texture unit 0.
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, inputTex);
      asset_shader.SetUniform_int("u_InputTex", 0);

      // Set film grain parameters.
      asset_shader.SetUniform_float("u_FilmGrainIntensity", filmGrainIntensity);
      asset_shader.SetUniform_float("u_FilmGrainSize", filmGrainSize);
      asset_shader.SetUniform_int("u_FilmGrainAnimate", filmGrainAnimate ? 1 : 0);

      // If animating, pass a time uniform (replace with your engine's time function)
      if (filmGrainAnimate)
      {
          float time = static_cast<float>(glfwGetTime());
          asset_shader.SetUniform_float("u_Time", time);
      }

      // Draw the full-screen quad.
      glDrawArrays(GL_TRIANGLES, 0, 6);
      m_draw_calls++;
  }


  void OpenGLRenderer::ApplyPixelate(const GLuint& inputTex, float pixelWidth, float pixelHeight)
  {
      #pragma region VAO Setup
      // Full-screen quad covering clip space.
      static const float vertices[] = {
          // Positions           // TexCoords
          -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, // Bottom-left
           1.0f, -1.0f, 0.0f,     1.0f, 0.0f, // Bottom-right
           1.0f,  1.0f, 0.0f,     1.0f, 1.0f, // Top-right
           1.0f,  1.0f, 0.0f,     1.0f, 1.0f, // Top-right
          -1.0f,  1.0f, 0.0f,     0.0f, 1.0f, // Top-left
          -1.0f, -1.0f, 0.0f,     0.0f, 0.0f  // Bottom-left
      };

      static GLuint vao = 0, vbo = 0;
      if (vao == 0)
      {
          glGenVertexArrays(1, &vao);
          glGenBuffers(1, &vbo);

          glBindVertexArray(vao);
          glBindBuffer(GL_ARRAY_BUFFER, vbo);
          glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

          glEnableVertexAttribArray(0); // Position
          glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
          glEnableVertexAttribArray(1); // TexCoords
          glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
          glBindVertexArray(0);

          // Schedule cleanup of the buffers.
          FreeQueue::Push([=]()
          {
              glDeleteBuffers(1, &vbo);
              glDeleteVertexArrays(1, &vao);
          });
      }
      glBindVertexArray(vao);
      #pragma endregion

      // Retrieve the pixelate shader asset.
      auto& asset_shader = FLX_ASSET_GET(Asset::Shader, "/shaders/Pixelate.flxshader");

      // Bind the input texture to texture unit 0.
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, inputTex);
      asset_shader.SetUniform_int("u_InputTex", 0);

      // Set the pixelation parameters.
      asset_shader.SetUniform_float("u_PixelWidth", pixelWidth);
      asset_shader.SetUniform_float("u_PixelHeight", pixelHeight);

      // Draw the full-screen quad.
      glDrawArrays(GL_TRIANGLES, 0, 6);
      m_draw_calls++;
  }

  #pragma endregion
}