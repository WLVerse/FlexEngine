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
  void OpenGLRenderer::DrawTexture2D(const Renderer2DProps& props, const FlexECS::EntityID camID)
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
    asset_shader.SetUniform_mat4("u_model", model.Translate(Vector3(position.x, position.y, 0.0f))
      .RotateX(props.rotation.x).RotateY(props.rotation.y).RotateZ(props.rotation.z).Scale(Vector3(props.scale.x, props.scale.y, 1.0f)));

    // For 2D rendering, we use an orthographic projection matrix, but this one uses the window as the viewfinder
    asset_shader.SetUniform_mat4("u_projection_view", CameraManager::HasCamera(camID) ? CameraManager::GetCamera(camID)->GetProjViewMatrix(): CameraManager::GetEditorCamera()->GetProjViewMatrix());

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

  void OpenGLRenderer::DrawTexture2D(const Renderer2DText& text, const FlexECS::EntityID camID)
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
      asset_shader.SetUniform_mat4("projection", CameraManager::HasCamera(camID) ? CameraManager::GetCamera(camID)->GetProjViewMatrix() : CameraManager::GetEditorCamera()->GetProjViewMatrix());

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

      // Updated text rendering logic (Deprecated)
      #if 0
      {
          auto words = splitIntoWords(text.m_words);
          Vector2 pen_pos = getAlignmentOffset(text.m_alignment, ""); // Initial alignment offset
          float lineWidth = 0.0f, totalHeight = 0.0f, maxLineHeight = 0.0f;
          const Asset::Glyph& space = asset_font.GetGlyph(' ');
          std::string currentLine;
          std::string nextLine;

          for (size_t i = 0; i < words.size(); ++i)
          {
              const std::string& word = words[i];

              if (word == "\n") // Handle explicit line break
              {
                  // Render the current line
                  pen_pos.x = getAlignmentOffset(text.m_alignment, currentLine).x;
                  for (char c : currentLine) {
                      const Asset::Glyph& glyph = asset_font.GetGlyph(c);
                      renderGlyph(glyph, pen_pos);
                      pen_pos.x += glyph.advance + text.m_letterspacing;
                  }

                  // Move to the next line
                  pen_pos.y += maxLineHeight + text.m_linespacing;
                  lineWidth = 0.0f;
                  maxLineHeight = 0.0f;
                  currentLine.clear();
                  continue;
              }

              // Measure the word width
              float wordWidth = 0.0f;
              float wordHeight = 0.0f;
              for (char c : word) {
                  const Asset::Glyph& glyph = asset_font.GetGlyph(c);
                  wordWidth += glyph.advance + text.m_letterspacing;
                  wordHeight = std::max(wordHeight, glyph.size.y);
              }

              // Check if the word fits in the current line
              if (lineWidth + wordWidth > text.m_maxwidthtextbox)
              {
                  // Render the current line
                  pen_pos.x = getAlignmentOffset(text.m_alignment, currentLine).x;
                  for (char c : currentLine) {
                      const Asset::Glyph& glyph = asset_font.GetGlyph(c);
                      renderGlyph(glyph, pen_pos);
                      pen_pos.x += glyph.advance + text.m_letterspacing;
                  }

                  // Move to the next line
                  pen_pos.y += maxLineHeight + text.m_linespacing;
                  totalHeight += maxLineHeight + text.m_linespacing;
                  lineWidth = 0.0f;
                  maxLineHeight = 0.0f;

                  // Stop rendering if vertical overflow occurs
                  if (totalHeight + wordHeight > text.m_maxheighttextbox)
                  {
                      currentLine.clear();
                      break;
                  }

                  // Reset the current line and add the current word to the next line
                  currentLine = word;
                  lineWidth = wordWidth;
                  maxLineHeight = wordHeight;
              }
              else
              {
                  // Add the word to the current line
                  if (!currentLine.empty()) {
                      currentLine += " ";
                  }
                  currentLine += word;
                  lineWidth += wordWidth + space.advance + text.m_letterspacing;
                  maxLineHeight = std::max(maxLineHeight, wordHeight);
              }
          }

          // Render the last line
          if (!currentLine.empty())
          {
              pen_pos.x = getAlignmentOffset(text.m_alignment, currentLine).x;
              for (char c : currentLine) {
                  const Asset::Glyph& glyph = asset_font.GetGlyph(c);
                  renderGlyph(glyph, pen_pos);
                  pen_pos.x += glyph.advance + text.m_letterspacing;
              }
          }
      }
      #endif

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

}