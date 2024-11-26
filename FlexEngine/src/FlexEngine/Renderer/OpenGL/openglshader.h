// WLVERSE [https://wlverse.web.app]
// openglshader.h
// 
// Shader class that links shader files with the same name.
// 
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include "Wrapper/path.h"
#include "FlexMath/matrix4x4.h"

namespace FlexEngine
{
  namespace Asset
  {

    // Helper class for building and compiling shaders using glad
    class __FLX_API Shader
    {
      unsigned int m_shader_program = 0;
      unsigned int m_vertex_shader = 0;
      unsigned int m_fragment_shader = 0;

    public:
      Shader() = default;
      Shader(const Path& path_to_vertex_shader, const Path& path_to_fragment_shader);
      ~Shader();

      // Links the vertex and fragment shaders into a shader program
      void Create(const Path& path_to_vertex_shader, const Path& path_to_fragment_shader);
      void Destroy();

      // Use the shader program with glUseProgram
      void Use() const;

      #pragma region Set Uniforms

      void SetUniform_bool(const char* name, bool value);
      void SetUniform_int(const char* name, int value);
      void SetUniform_float(const char* name, float value);
      void SetUniform_vec2(const char* name, const Vector2& vector);
      void SetUniform_vec3(const char* name, const Vector3& vector);
      void SetUniform_mat4(const char* name, const Matrix4x4& matrix);

      #pragma endregion

      // Returns the shader program
      unsigned int Get() const;

      #pragma region Internal Functions

    private:
      void Internal_CreateVertexShader(const Path& path_to_vertex_shader);
      void Internal_CreateFragmentShader(const Path& path_to_fragment_shader);
      void Internal_Link();

      #pragma endregion

      #pragma region Enums

    public:
      enum Type
      {
        Vertex = 0,
        Fragment,
        Geometry
      };

      #pragma endregion

#ifdef _DEBUG
    private:
      Path m_path_to_vertex_shader;
      Path m_path_to_fragment_shader;
    public:
      void Dump() const;
#endif
    };

  }
}