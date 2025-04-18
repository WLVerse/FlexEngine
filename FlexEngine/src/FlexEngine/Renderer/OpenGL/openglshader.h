// WLVERSE [https://wlverse.web.app]
// openglshader.h
//
// Wraps the opengl shader. 
// Handles compiling and linking of the shader program. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include "Utilities/file.h"
#include "FlexMath/matrix4x4.h"
#include "openglfont.h"
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
    public:
      // Load the shader from a metadata file
      Shader();

      // Must be implemented to free the shader
      // RAII
      ~Shader();

      #pragma region Validity

      // Internal helper macro to check if the shader is valid
      #define _FLX_SHADER_VALIDITY_CHECK \
        if (!IsValid()) \
        { \
          Log::Error( \
            "Tried to use a shader that isn't valid! " \
            "Shader: " + m_path_to_vertex_shader.string() + " " + m_path_to_fragment_shader.string() \
          ); \
          return; \
        }

      // Returns true if the shader program is valid
      // Simply checks if it is not 0 since it should be an actual number if it is valid
      bool IsValid() const;

      #pragma endregion

    private:
      unsigned int m_shader_program = 0;
      unsigned int m_vertex_shader = 0;
      unsigned int m_fragment_shader = 0;
      //unsigned int m_geometry_shader = 0;

      Path m_path_to_metadata;

      Path m_path_to_vertex_shader;
      Path m_path_to_fragment_shader;
      //Path m_path_to_geometry_shader;

    public:

      // Load the shader from the metadata file
      void Load(Path metadata);

      // Links the vertex and fragment shaders into a shader program
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
      void SetUniform_int_array(const char* name, const int* array, int count);
      void SetUniformGlyphMetrics(const char* name, const Asset::GlyphMetric* metrics, int count);

      #pragma endregion

      // Returns the shader program
      unsigned int Get() const;

      #pragma region Internal Functions

    private:
      // INTERNAL FUNCTION
      // Parse the metadata file.
      // This function calls the other internal functions to
      // create and link the shaders.
      bool Internal_Parse();

      void Internal_CreateVertexShader(const Path& path_to_vertex_shader);
      void Internal_CreateFragmentShader(const Path& path_to_fragment_shader);
      //void Internal_CreateGeometryShader(const Path& path_to_geometry_shader);
      void Internal_Link();

      #pragma endregion

      #pragma region Enums

    public:
      enum Type
      {
        Vertex = 0,
        Fragment,
        //Geometry
      };

      #pragma endregion

#ifdef _DEBUG
    public:
      void Dump() const;
#endif
    };

  }
}