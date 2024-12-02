// WLVERSE [https://wlverse.web.app]
// openglshader.cpp
// 
// Shader class that links shader files with the same name.
// 
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "pch.h"

#include "openglshader.h"

#include <glad/glad.h>

namespace FlexEngine
{
  namespace Asset
  {

    Shader::Shader(const Path& path_to_vertex_shader, const Path& path_to_fragment_shader)
    {
      Create(path_to_vertex_shader, path_to_fragment_shader);
    }

    Shader::~Shader()
    {
      Destroy();
    }

    void Shader::Create(const Path& path_to_vertex_shader, const Path& path_to_fragment_shader)
    {
      FLX_FLOW_FUNCTION();

      Internal_CreateVertexShader(path_to_vertex_shader);
      Internal_CreateFragmentShader(path_to_fragment_shader);
      Internal_Link();

#ifdef _DEBUG
      // store paths for debugging
      m_path_to_vertex_shader = path_to_vertex_shader;
      m_path_to_fragment_shader = path_to_fragment_shader;
#endif
    }

    void Shader::Destroy()
    {
      if (m_vertex_shader != 0) glDeleteShader(m_vertex_shader);
      if (m_fragment_shader != 0) glDeleteShader(m_fragment_shader);
      if (m_shader_program != 0) glDeleteProgram(m_shader_program);

#ifdef _DEBUG
      m_path_to_vertex_shader = Path();
      m_path_to_fragment_shader = Path();
#endif
    }

    void Shader::Use() const
    {
      // guard
      if (m_shader_program == 0)
      {
        FLX_ASSERT(false, "Shader program has not been created yet!");
        return;
      }

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

    #pragma endregion

    unsigned int Shader::Get() const
    {
      // guard
      if (m_shader_program == 0)
      {
        Log::Warning("Shader program has not been created yet!");
      }
      return m_shader_program;
    }

    #pragma region Internal Functions

    void Shader::Internal_CreateVertexShader(const Path& path_to_vertex_shader)
    {
      FLX_FLOW_FUNCTION();

      // warning if vertex shader already exists
      // handled by overriding the old shader
      if (m_vertex_shader != 0)
      {
        Log::Warning("Vertex shader already exists, overriding it!");
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
        Log::Error(std::string("Vertex shader did not compile!\n") + infoLog + '\n');
      }
    }

    void Shader::Internal_CreateFragmentShader(const Path& path_to_fragment_shader)
    {
      FLX_FLOW_FUNCTION();

      // warning if fragment shader already exists
      // handled by overriding the old shader
      if (m_fragment_shader != 0)
      {
        Log::Warning("Fragment shader already exists, overriding it!");
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
        Log::Error(std::string("Fragment shader did not compile!\n") + infoLog + '\n');
      }
    }

    void Shader::Internal_Link()
    {
      FLX_FLOW_FUNCTION();

      // guard: check if shaders are compiled
      if (m_vertex_shader == 0 || m_fragment_shader == 0)
      {
        FLX_ASSERT(false, "Shader could not be linked because one of the shaders is missing!");
        return;
      }

      // create shader program
      m_shader_program = glCreateProgram();

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
        FLX_ASSERT(false, std::string("Shader linker error! ") + infoLog);
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