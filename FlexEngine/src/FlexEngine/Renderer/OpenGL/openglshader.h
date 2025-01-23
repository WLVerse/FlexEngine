#pragma once

#include "flx_api.h"

#include "Utilities/path.h"
#include "FlexMath/matrix4x4.h"

namespace FlexEngine
{
  namespace Asset
  {

    // Helper class for building and compiling shaders
    // TODO: Rework to have a .flxshader file that stores the shader paths
    // TODO: use the flxfmt formatter
    // TODO: save any changes to file
    // TODO: generate a new metadata file in editor
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