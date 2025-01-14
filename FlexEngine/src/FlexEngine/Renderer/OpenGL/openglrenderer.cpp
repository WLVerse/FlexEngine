#include "openglrenderer.h"

#include "assetmanager.h" // FLX_ASSET_GET
#include "DataStructures/freequeue.h"

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
  
  // TODO: cache the vao and vbo
  void OpenGLRenderer::DrawTexture2D(Camera const& cam, const Renderer2DProps& props)
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

      glEnableVertexAttribArray(0);
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
    if (vao == 0 || props.scale == Vector2::Zero) return;

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
      tex_coords[1] = uv.y;
      tex_coords[2] = uv.z;
      tex_coords[3] = uv.y;
      tex_coords[4] = uv.z;
      tex_coords[5] = uv.w;
      tex_coords[6] = uv.z;
      tex_coords[7] = uv.w;
      tex_coords[8] = uv.x;
      tex_coords[9] = uv.w;
      tex_coords[10] = uv.x;
      tex_coords[11] = uv.y;
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

    auto& asset_shader = FLX_ASSET_GET(Asset::Shader, props.shader);
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

    // For 2D rendering, we use an orthographic projection matrix, but this one uses the window as the viewfinder
    asset_shader.SetUniform_mat4("u_projection_view", cam.GetProjViewMatrix());

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

  void OpenGLRenderer::DrawTexture2D(Camera const& cam, const Renderer2DText& text)
  {
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
      //if (text.m_words.empty() ||
      //    (OpenGLFrameBuffer::CheckSameFrameBuffer(OpenGLFrameBuffer::m_gameFBO) && m_CamM_Instance->GetMainCamera() == INVALID_ENTITY_ID) ||
      //    (OpenGLFrameBuffer::CheckSameFrameBuffer(OpenGLFrameBuffer::m_editorFBO) && m_CamM_Instance->GetEditorCamera() == INVALID_ENTITY_ID))
      //    return;

      //if (followcam && m_CamM_Instance->GetMainCamera() == INVALID_ENTITY_ID)
      //    followcam = false;

      auto& asset_shader = FLX_ASSET_GET(Asset::Shader, text.m_shader);
      asset_shader.Use();

      asset_shader.SetUniform_vec3("u_color", text.m_color);
      asset_shader.SetUniform_mat4("u_model", text.m_transform);
      asset_shader.SetUniform_mat4("projection", cam.GetProjViewMatrix());

      glActiveTexture(GL_TEXTURE0);
      glBindVertexArray(vao);
      auto& asset_font = FLX_ASSET_GET(Asset::Font, text.m_fonttype);

      // Lambda to calculate the width of a line of text
      auto findLineWidth = [&](const std::string& line)
      {
          float total_length = 0.0f;
          for (char c : line) 
          {
              const Asset::Glyph& glyph = asset_font.GetGlyph(c);
              total_length += glyph.advance + (int)text.m_letterspacing;
              if (total_length > text.m_maxwidthtextbox) break;
          }
          return std::min(total_length, text.m_maxwidthtextbox);
      };
      // Calculate total text height
      auto findTotalTextHeight = [&]() -> float
      {
          float totalHeight = 0.0f, maxLineHeight = 0.0f;
          for (char c : text.m_words)
          {
              const Asset::Glyph& glyph = asset_font.GetGlyph(c);
              maxLineHeight = std::max(maxLineHeight, glyph.size.y);

              if (c == '\n' || maxLineHeight > text.m_maxwidthtextbox)
              {
                  totalHeight += maxLineHeight + text.m_linespacing;
                  maxLineHeight = 0.0f;
              }
          }
          return totalHeight + maxLineHeight; // Add last line height
      };

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
              {xpos, ypos + h, 0.0f, 0.0f}, 
              {xpos, ypos, 0.0f, 1.0f}, 
              {xpos + w, ypos, 1.0f, 1.0f},
              {xpos, ypos + h, 0.0f, 0.0f}, 
              {xpos + w, ypos, 1.0f, 1.0f}, 
              {xpos + w, ypos + h, 1.0f, 0.0f}
          };

          glBindBuffer(GL_ARRAY_BUFFER, vbo);
          glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quadVertices), quadVertices);
          glDrawArrays(GL_TRIANGLES, 0, 6);
          m_draw_calls++;
      };

      // Lambda to set horizontal alignment
      auto setAlignmentX = [&](Renderer2DText::AlignmentX alignment, const std::string& words) 
      {
          switch (alignment) 
          {
          case Renderer2DText::Alignment_Left: return 0.0f;
          case Renderer2DText::Alignment_Center: return -findLineWidth(words) / 2.0f;
          case Renderer2DText::Alignment_Right: return -findLineWidth(words);
          default: return 0.0f;
          }
      };

      // Lambda to set vertical alignment
      auto setAlignmentY = [&](Renderer2DText::AlignmentY alignment) 
      {
          switch (alignment) 
          {
          case Renderer2DText::Alignment_Top: return 0.0f;
          case Renderer2DText::Alignment_Middle: return findTotalTextHeight() / 2.0f; // Center align
          case Renderer2DText::Alignment_Bottom: return findTotalTextHeight(); // Start at the bottom
          default: return 0.0f;
          }
      };

      // Set initial pen position based on alignments
      Vector3 pen_pos = Vector3::Zero;
      float maxLineHeight = 0.0f, lineWidth = 0.0f;
      std::string currentLine;
      pen_pos.x = setAlignmentX(static_cast<Renderer2DText::AlignmentX>(text.m_alignment.first), text.m_words);
      pen_pos.y = setAlignmentY(static_cast<Renderer2DText::AlignmentY>(text.m_alignment.second));
      float totalHeight = 0.0f; // Calculate total height for vertical alignment
      for (char c : text.m_words) 
      {
          totalHeight += asset_font.GetGlyph(c).size.y + text.m_linespacing;
      }

      std::string remainingWords = text.m_words;
      int currentIndex = 0;
      for (char c : text.m_words) 
      {
          const Asset::Glyph& glyph = asset_font.GetGlyph(c);
          maxLineHeight = std::max(maxLineHeight, glyph.size.y);
          if (lineWidth + glyph.advance > text.m_maxwidthtextbox && c != ' ')
          {
              pen_pos.y += maxLineHeight + text.m_linespacing;
              lineWidth = 0.0f;
              remainingWords = remainingWords.substr(currentIndex);
              pen_pos.x = setAlignmentX(static_cast<Renderer2DText::AlignmentX>(text.m_alignment.first), remainingWords);
              currentIndex = 0;
          }
          renderGlyph(glyph, pen_pos);
          pen_pos.x += glyph.advance + text.m_letterspacing;
          lineWidth += glyph.advance + text.m_letterspacing;
          ++currentIndex;
      }
      glBindVertexArray(0);
      glBindTexture(GL_TEXTURE_2D, 0);
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

    static Asset::Shader texture_shader(Path::current("assets/shaders/texture.vert"), Path::current("assets/shaders/texture.frag"));
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

    // proj view matrix
    Matrix4x4 proj_view = Matrix4x4::Orthographic(0.0f, window_size.x, 0.0f, window_size.y, -1.0f, 1.0f);
    texture_shader.SetUniform_mat4("u_projection_view", proj_view);

    // draw
    glDrawArrays(GL_TRIANGLES, 0, 6);
    m_draw_calls++;

    glBindVertexArray(0);
  }

}