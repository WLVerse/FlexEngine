#include "pch.h"

#include "openglshader.h"

#include <glad/glad.h>

namespace FlexEngine
{
  namespace Asset
  {

    Shader::Shader(File& _metadata)
      : metadata(_metadata)
    {
      // parse the file
      if (!Internal_Parse())
      {
        Log::Error("Shader file could not be parsed! Shader: " + metadata.path.string());
      }
    }

    Shader::~Shader()
    {
      Destroy();
    }

    bool Shader::IsValid() const
    {
      return m_shader_program != 0;
    }

    void Shader::Destroy()
    {
      if (m_vertex_shader != 0) glDeleteShader(m_vertex_shader);
      if (m_fragment_shader != 0) glDeleteShader(m_fragment_shader);
      if (m_shader_program != 0) glDeleteProgram(m_shader_program);

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

    #pragma endregion

    unsigned int Shader::Get() const
    {
      // guard
      if (m_shader_program == 0)
      {
        Log::Warning(
          "Cannot get the shader program because it is not valid or hasn't been created yet! "
          "Shader: " + metadata.path.string()
        );
      }
      return m_shader_program;
    }

    #pragma region Internal Functions

    bool Shader::Internal_Parse()
    {
      // parse the file
      // the file is just a text file with the type and path of the shader
      std::stringstream ss(metadata.Read());

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
          Log::Error("The shader file has a missing colon. Shader: " + metadata.path.string() + " Line: " + std::to_string(line_number));
          return false;
        }

        // warn for bad shader type
        if (shader_type != "vertex" && shader_type != "fragment")
        {
          Log::Error("The shader file has an invalid shader type. Shader: " + metadata.path.string() + " Line: " + std::to_string(line_number));
          return false;
        }

        // error for invalid paths
        try
        {
          path_to_shader = Path(path);
        }
        catch (const std::invalid_argument& e)
        {
          Log::Error(std::string(e.what()) + " Shader: " + metadata.path.string() + " Line: " + std::to_string(line_number));
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

      return true;
    }

    void Shader::Internal_CreateVertexShader(const Path& path_to_vertex_shader)
    {
      FLX_FLOW_FUNCTION();

      // warning if vertex shader already exists
      // handled by overriding the old shader
      if (m_vertex_shader != 0)
      {
        Log::Warning("Vertex shader already exists, overriding it! Shader: " + metadata.path.string());
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
          "Shader: " + metadata.path.string() + " Vertex shader: " + path_to_vertex_shader.string() + '\n' +
          infoLog + '\n'
        );
      }
    }

    void Shader::Internal_CreateFragmentShader(const Path& path_to_fragment_shader)
    {
      FLX_FLOW_FUNCTION();

      // warning if fragment shader already exists
      // handled by overriding the old shader
      if (m_fragment_shader != 0)
      {
        Log::Warning("Fragment shader already exists, overriding it! Shader: " + metadata.path.string());
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
          "Shader: " + metadata.path.string() + " Fragment shader: " + path_to_fragment_shader.string() + '\n' +
          infoLog + '\n'
        );
      }
    }

    void Shader::Internal_Link()
    {
      FLX_FLOW_FUNCTION();

      // guard: check if shaders are compiled
      if (m_vertex_shader == 0 || m_fragment_shader == 0)
      {
        Log::Error("Shader could not be linked because one of the shaders is missing! Shader: " + metadata.path.string());
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
        Log::Error(
          "Shader linker error! "
          "Shader: " + metadata.path.string() + '\n' +
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