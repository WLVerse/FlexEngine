#include "pch.h"

#include "opengltexture.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace FlexEngine
{

  #pragma region Static Internal Functions

  static void Internal_LoadTextureForOpenGL(unsigned int* out_texture, unsigned char* texture_data, int width, int height)
  {
    // Create a OpenGL texture identifier
    glGenTextures(1, out_texture);
    glBindTexture(GL_TEXTURE_2D, *out_texture);

    // Setup filtering parameters for display
    //glGenerateMipmap(GL_TEXTURE_2D);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST for pixel art, find some way to toggle this for non-pixel art
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into opengl texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);
  }

  // Create a default texture
  // This is the legendary purple and black checkered texture
  static void Internal_CreateDefaultTexture(unsigned char** out_texture_data, int width, int height)
  {
    *out_texture_data = new unsigned char[width * height * 4];
    if (!(*out_texture_data))
    {
      Log::Error("Could not allocate memory for default texture data!");
      return;
    }

    for (int y = 0; y < height; ++y)
    {
      for (int x = 0; x < width; ++x)
      {
        unsigned char color = ( ((x / 32) + (y / 32)) % 2 == 0 ) ? 255 : 0;

        int index = (y * width + x) * 4;
        (*out_texture_data)[index + 0] = color; // Red
        (*out_texture_data)[index + 1] = 0;     // Green
        (*out_texture_data)[index + 2] = color; // Blue
        (*out_texture_data)[index + 3] = 255;   // Alpha
      }
    }
  }
  
  static void Internal_LoadDefaultTexture(unsigned char** out_texture_data, unsigned int* out_texture, int width, int height)
  {
    // create default texture
    Internal_CreateDefaultTexture(out_texture_data, width, height);

    // bind the default texture
    Internal_LoadTextureForOpenGL(out_texture, *out_texture_data, width, height);
  }
  
  static bool Internal_LoadTextureFromFile(const char* filename, unsigned char** out_texture_data, unsigned int* out_texture, int* out_width, int* out_height)
  {
    // The usual image format has the origin at the top-left corner.
    // OpenGL expects the origin at the bottom-left corner.
    // We handle this in the vertex shader by flipping the y-coordinate.
    // The reason is so that ImGui can display the texture correctly because
    // it does its own texture flipping.
    //stbi_set_flip_vertically_on_load(true);

    // Load from file
    unsigned char* image_data = stbi_load(filename, out_width, out_height, NULL, 4);
    if (!image_data)
    {
      Log::Warning("Could not load texture from file using stbi_load. File: " + std::string(filename));
      return false;
    }

    // Save the image data
    std::size_t texture_size = (*out_width) * (*out_height) * 4;
    *out_texture_data = new unsigned char[texture_size];
    if (!(*out_texture_data))
    {
      Log::Error("Could not allocate memory for texture data. File: " + std::string(filename));
      stbi_image_free(image_data);
      return false;
    }
    memcpy(*out_texture_data, image_data, texture_size);

    // Bind the texture
    Internal_LoadTextureForOpenGL(out_texture, *out_texture_data, *out_width, *out_height);

    stbi_image_free(image_data);

    return true;
  }

  #pragma warning(suppress: 4505) // unreferenced local function has been removed
  static bool Internal_LoadTextureFromMemory(const unsigned char* texture_data, int size, unsigned char** out_texture_data, unsigned int* out_texture, int* out_width, int* out_height)
  {
    // Decompress the texture
    int channels;
    unsigned char* image_data = stbi_load_from_memory(texture_data, size, out_width, out_height, &channels, 4);
    if (!image_data)
    {
      Log::Error("Could not decompress texture data!");
      return false;
    }

    // Save the image data
    std::size_t texture_size = (*out_width) * (*out_height) * 4;
    *out_texture_data = new unsigned char[texture_size];
    if (!(*out_texture_data))
    {
      Log::Error("Could not allocate memory for texture data!");
      stbi_image_free(image_data);
      return false;
    }
    memcpy(*out_texture_data, image_data, texture_size);

    // Bind the texture
    Internal_LoadTextureForOpenGL(out_texture, *out_texture_data, *out_width, *out_height);

    stbi_image_free(image_data);

    return true;
  }

  #pragma endregion
  
  namespace Asset
  {

    // static member initialization
    const Texture Texture::Null = Texture();
    const Texture Texture::None = Null;

    Texture Texture::Default()
    {
      Texture texture;
      texture.Load();
      return texture;
    }

    Texture::Texture(const Texture& other)
    {
      *this = other;
    }

    Texture& Texture::operator=(const Texture& other)
    {
      // deep copy
      m_texture = other.m_texture;
      m_width = other.m_width;
      m_height = other.m_height;
      std::size_t size = m_width * m_height * 4;
      if (size)
      {
        m_texture_data = new unsigned char[size];
        memcpy(m_texture_data, other.m_texture_data, size);
      }
      else
      {
        m_texture_data = nullptr;
      }

      return *this;
    }

    Texture::Texture(unsigned char* texture_data, int width, int height)
      : m_texture_data(nullptr), m_width(width), m_height(height)
    {
      // decompress the texture
      if (height == 0)
      {
        //bool success = Internal_LoadTextureFromMemory(texture_data, width, &m_texture_data, &m_texture, &m_width, &m_height);
        //// if no texture is loaded, bind the default texture
        //if (!success || !m_texture || !m_width || !m_height)
        //{
        //  Load();
        //}

        Log::Error("AssimpWrapper: Embedded textures are compressed, decompression is not supported.");
        Load();

        //Log::Debug("Loaded: " + std::to_string(m_texture));
      }
      // no decompression needed
      else
      {
        m_texture_data = new unsigned char[width * height * 4];
        if (!m_texture_data)
        {
          Log::Error("Could not allocate memory for texture data!");
          return;
        }
        memcpy(m_texture_data, texture_data, width * height * 4);
        
        Internal_LoadTextureForOpenGL(&m_texture, m_texture_data, m_width, m_height);
      }
    }

    Texture::~Texture()
    {
      Unload();
    }

    #pragma region Texture Management Functions

    void Texture::Load()
    {
      // always unload the texture before loading
      Unload();

      // set the texture to be the default texture
      m_width = m_height = 64;
      Internal_LoadDefaultTexture(&m_texture_data, &m_texture, m_width, m_height);
    }

    void Texture::Load(const Path& path_to_texture)
    {
      // always unload the texture before loading
      Unload();

      bool success = Internal_LoadTextureFromFile(path_to_texture.string().c_str(), &m_texture_data, &m_texture, &m_width, &m_height);
      // if no texture is loaded, bind the default texture
      if (!success || !m_texture || !m_width || !m_height)
      {
        Load();
      }
    }

    void Texture::Unload()
    {
      if (m_texture_data)
      {
        delete[] m_texture_data;
        m_texture_data = nullptr;
      }

      if (m_texture)
      {
        glDeleteTextures(1, &m_texture);
        m_texture = 0;
      }

      m_width = m_height = 0;
    }

    #pragma endregion

    #pragma region Binding functions for OpenGL

    void Texture::Bind(const Shader& shader, const char* name, unsigned int texture_unit) const
    {
      glActiveTexture(GL_TEXTURE0 + texture_unit);

      std::string texture_name = name;
      texture_name += std::to_string(texture_unit);
      glUniform1i(glGetUniformLocation(shader.Get(), texture_name.c_str()), texture_unit);

      glBindTexture(GL_TEXTURE_2D, m_texture);
    }

    void Texture::Unbind() const
    {
      glBindTexture(GL_TEXTURE_2D, 0);
    }

    #pragma endregion

  }

}
