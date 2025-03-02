// WLVERSE [https://wlverse.web.app]
// openglshader.cpp
//
// Wraps the opengl shader. 
// Handles compiling and linking of the shader program. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#include "pch.h"

#include "openglshader.h"

#include <glad/glad.h>

namespace FlexEngine
{
  namespace Asset
  {

    Shader::Shader()
    {
    }

    Shader::~Shader()
    {
      Destroy();
    }

    bool Shader::IsValid() const
    {
      return m_shader_program != 0;
    }

    void Shader::Load(Path metadata)
    {
      m_path_to_metadata = metadata;

      // parse the file
      if (!Internal_Parse())
      {
        Log::Error("Shader file could not be parsed! Shader: " + m_path_to_metadata.string());
      }
    }

    void Shader::Destroy()
    {
      if (m_vertex_shader != 0) glDeleteShader(m_vertex_shader);
      if (m_fragment_shader != 0) glDeleteShader(m_fragment_shader);
      if (m_shader_program != 0) glDeleteProgram(m_shader_program);

      m_path_to_metadata = Path();
      m_path_to_vertex_shader = Path();
      m_path_to_fragment_shader = Path();
    }

    void Shader::Use() const
    {
      _FLX_SHADER_VALIDITY_CHECK;

      glUseProgram(m_shader_program);
    }

    #pragma region Set Uniforms

    void Shader::SetUniform_bool(const char* name, bool value)
    {
      Use(); // make sure the shader is being used
      glUniform1i(glGetUniformLocation(m_shader_program, name), (int)value);
    }

    void Shader::SetUniform_int(const char* name, int value)
    {
      Use();
      glUniform1i(glGetUniformLocation(m_shader_program, name), value);
    }

    void Shader::SetUniform_float(const char* name, float value)
    {
      Use();
      glUniform1f(glGetUniformLocation(m_shader_program, name), value);
    }

    void Shader::SetUniform_vec2(const char* name, const Vector2& vector)
    {
      Use();
      glUniform2f(glGetUniformLocation(m_shader_program, name), vector.x, vector.y);
    }

    void Shader::SetUniform_vec3(const char* name, const Vector3& vector)
    {
      Use();
      glUniform3f(glGetUniformLocation(m_shader_program, name), vector.x, vector.y, vector.z);
    }

    void Shader::SetUniform_mat4(const char* name, const Matrix4x4& matrix)
    {
      Use();
      glUniformMatrix4fv(glGetUniformLocation(m_shader_program, name), 1, GL_FALSE, matrix.data);
    }

    void Shader::SetUniform_int_array(const char* name, const int* array, int count)
    {
        Use();
        GLint location = glGetUniformLocation(m_shader_program, name);
        glUniform1iv(location, count, array);
    }

    void Shader::SetUniformGlyphMetrics(const char* name, const Asset::GlyphMetric* metrics, int count)
    {
        Use();
        for (int i = 0; i < count; ++i)
        {
            // Build the base name for the i-th element: e.g. "u_glyphs[0]"
            std::string baseName = std::string(name) + "[" + std::to_string(i) + "]";

            // Upload 'advance'
            GLint locAdvance = glGetUniformLocation(m_shader_program, (baseName + ".advance").c_str());
            glUniform1f(locAdvance, metrics[i].advance);

            // Upload 'size' (vec2)
            GLint locSize = glGetUniformLocation(m_shader_program, (baseName + ".size").c_str());
            glUniform2fv(locSize, 1, metrics[i].size);

            // Upload 'bearing' (vec2)
            GLint locBearing = glGetUniformLocation(m_shader_program, (baseName + ".bearing").c_str());
            glUniform2fv(locBearing, 1, metrics[i].bearing);

            // Upload 'uvOffset' (vec2)
            GLint locUVOffset = glGetUniformLocation(m_shader_program, (baseName + ".uvOffset").c_str());
            glUniform2fv(locUVOffset, 1, metrics[i].uvOffset);

            // Upload 'uvSize' (vec2)
            GLint locUVSize = glGetUniformLocation(m_shader_program, (baseName + ".uvSize").c_str());
            glUniform2fv(locUVSize, 1, metrics[i].uvSize);
        }
    }
    #pragma endregion

    unsigned int Shader::Get() const
    {
      // guard
      if (m_shader_program == 0)
      {
        Log::Warning(
          "Cannot get the shader program because it is not valid or hasn't been created yet! "
          "Shader: " + m_path_to_metadata.string()
        );
      }
      return m_shader_program;
    }

    #pragma region Internal Functions

    bool Shader::Internal_Parse()
    {
      // parse the file
      // the file is just a text file with the type and path of the shader
      File& file = File::Open(m_path_to_metadata);
      std::stringstream ss(file.Read());

      // parse format
      // <shader_type>: <path>
      // vertex: path/to/vertex.shader
      // fragment: path/to/fragment.shader
      std::string line;
      std::size_t line_number = 0; // for debugging
      while (std::getline(ss, line))
      {
        line_number++;

        // skip empty lines
        if (line.empty()) return false;

        // skip comments
        if (
          line[0] == '/' && line[1] == '/' ||
          line[0] == '#'
        ) return false;

        // remove all whitespace
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());

        // split the line into shader type and path
        std::size_t colon = line.find(':');
        std::string shader_type = line.substr(0, colon);
        std::string path = line.substr(colon + 1);
        Path path_to_shader;

        // warn for missing colon
        if (colon == std::string::npos)
        {
          Log::Error("The shader file has a missing colon. Shader: " + m_path_to_metadata.string() + " Line: " + std::to_string(line_number));
          return false;
        }

        // warn for bad shader type
        if (shader_type != "vertex" && shader_type != "fragment")
        {
          Log::Error("The shader file has an invalid shader type. Shader: " + m_path_to_metadata.string() + " Line: " + std::to_string(line_number));
          return false;
        }

        // error for invalid paths
        try
        {
          path_to_shader = Path(path);
        }
        catch (const std::invalid_argument& e)
        {
          Log::Error(std::string(e.what()) + " Shader: " + m_path_to_metadata.string() + " Line: " + std::to_string(line_number));
          return false;
        }

        // create the shader
        if (shader_type == "vertex")
        {
          m_path_to_vertex_shader = path_to_shader;
          Internal_CreateVertexShader(m_path_to_vertex_shader);
        }
        else if (shader_type == "fragment")
        {
          m_path_to_fragment_shader = path_to_shader;
          Internal_CreateFragmentShader(m_path_to_fragment_shader);
        }
      }

      // link the shaders
      Internal_Link();

      return true;
    }

    void Shader::Internal_CreateVertexShader(const Path& path_to_vertex_shader)
    {
      FLX_FLOW_FUNCTION();

      // warning if vertex shader already exists
      // handled by overriding the old shader
      if (m_vertex_shader != 0)
      {
        Log::Warning("Vertex shader already exists, overriding it! Shader: " + m_path_to_metadata.string());
        glDeleteShader(m_vertex_shader);
      }

      // read vertex shader file
      File& vertex_shader_file = File::Open(path_to_vertex_shader);
      std::string vertex_shader_source = vertex_shader_file.Read();

      // create vertex shader
      m_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
      const char* vertex_shader_source_cstr = vertex_shader_source.c_str();
      glShaderSource(m_vertex_shader, 1, &vertex_shader_source_cstr, NULL);
      glCompileShader(m_vertex_shader);

      // check for vertex shader compile errors
      int success;
      char infoLog[512];
      glGetShaderiv(m_vertex_shader, GL_COMPILE_STATUS, &success);
      if (!success)
      {
        glGetShaderInfoLog(m_vertex_shader, 512, NULL, infoLog);
        Log::Error(
          "Vertex shader did not compile. "
          "Shader: " + m_path_to_metadata.string() + " Vertex shader: " + path_to_vertex_shader.string() + '\n' +
          infoLog + '\n'
        );
      }

      Log::Debug("Created vertex shader with id: " + std::to_string(m_vertex_shader));
    }

    void Shader::Internal_CreateFragmentShader(const Path& path_to_fragment_shader)
    {
      FLX_FLOW_FUNCTION();

      // warning if fragment shader already exists
      // handled by overriding the old shader
      if (m_fragment_shader != 0)
      {
        Log::Warning("Fragment shader already exists, overriding it! Shader: " + m_path_to_metadata.string());
        glDeleteShader(m_fragment_shader);
      }

      // read fragment shader file
      File& fragment_shader_file = File::Open(path_to_fragment_shader);
      std::string fragment_shader_source = fragment_shader_file.Read();

      // create fragment shader
      m_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
      const char* fragment_shader_source_cstr = fragment_shader_source.c_str();
      glShaderSource(m_fragment_shader, 1, &fragment_shader_source_cstr, NULL);
      glCompileShader(m_fragment_shader);

      // check for fragment shader compile errors
      int success;
      char infoLog[512];
      glGetShaderiv(m_fragment_shader, GL_COMPILE_STATUS, &success);
      if (!success)
      {
        glGetShaderInfoLog(m_fragment_shader, 512, NULL, infoLog);
        Log::Error(
          "Fragment shader did not compile. "
          "Shader: " + m_path_to_metadata.string() + " Fragment shader: " + path_to_fragment_shader.string() + '\n' +
          infoLog + '\n'
        );
      }

      Log::Debug("Created fragment shader with id: " + std::to_string(m_fragment_shader));
    }

    void Shader::Internal_Link()
    {
      FLX_FLOW_FUNCTION();

      // guard: check if shaders are compiled
      if (m_vertex_shader == 0 || m_fragment_shader == 0)
      {
        Log::Error("Shader could not be linked because one of the shaders is missing! Shader: " + m_path_to_metadata.string());
        return;
      }

      // create shader program
      m_shader_program = glCreateProgram();

      Log::Debug("Created shader program with id: " + std::to_string(m_shader_program));

      glAttachShader(m_shader_program, m_vertex_shader);
      glAttachShader(m_shader_program, m_fragment_shader);
      glLinkProgram(m_shader_program);

      // check for linking errors
      int success;
      char infoLog[512];
      glGetProgramiv(m_shader_program, GL_LINK_STATUS, &success);
      if (!success)
      {
        glDetachShader(m_shader_program, m_vertex_shader);
        glDetachShader(m_shader_program, m_fragment_shader);
        glGetProgramInfoLog(m_shader_program, 512, NULL, infoLog);
        Log::Error(
          "Shader linker error! "
          "Shader: " + m_path_to_metadata.string() + '\n' +
          infoLog + '\n'
        );
        return;
      }

      // delete shaders
      glDeleteShader(m_vertex_shader);
      glDeleteShader(m_fragment_shader);
    }

    #pragma endregion

#ifdef _DEBUG
    void Shader::Dump() const
    {
      // print the location of the shaders
      Log::Debug("Vertex Shader: " + std::to_string(m_path_to_vertex_shader));
      Log::Debug("Fragment Shader: " + std::to_string(m_path_to_fragment_shader));
    }
#endif

  }
}