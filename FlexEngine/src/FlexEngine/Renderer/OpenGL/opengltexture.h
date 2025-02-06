// WLVERSE [https://wlverse.web.app]
// opengltexture.h
//
// Texture class to load and unload textures. 
// Wraps around OpenGL texture functions and uses stb_image.h to load images. 
// Can be used to load textures from files or create textures from memory. 
// Can be used to bind and unbind textures for OpenGL. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include "Utilities/file.h"
#include "Renderer/OpenGL/openglshader.h"

#include <string>

// Helper macro to display a texture for ImGui using the ImGui::Image function
// Example usage: ImGui::Image(IMGUI_IMAGE(texture_variable));
#define IMGUI_IMAGE(TEXTURE) TEXTURE.GetTextureImGui(), ImVec2(TEXTURE.GetWidthF(), TEXTURE.GetHeightF())

namespace FlexEngine
{
  namespace Asset
  {

    // Texture class to load and unload textures.
    // Wraps around OpenGL texture functions and uses stb_image.h to load images.
    // Can be used to load textures from files or create textures from memory.
    // Can be used to bind and unbind textures for OpenGL.
    class __FLX_API Texture
    {
      // The texture data is stored as an array of unsigned chars.
      // In OpenGL, the texture data is stored as RGBA.
      unsigned char* m_texture_data = nullptr;
      unsigned int m_texture = 0;
      int m_width = 0;
      int m_height = 0;

    public:

      Texture() = default;

      // deep copy constructor
      Texture(const Texture& other);
      Texture& operator=(const Texture& other);

      // For creating textures from memory
      // Useful for creating textures from procedural generation or loading
      // embedded textures.
      Texture(unsigned char* texture_data, int width, int height = 0);

      ~Texture();

      // Common texture names.
      // They are all represented by the default purple-black checkerboard texture.

      static const Texture Null;
      static const Texture None;
      static Texture Default();

      #pragma region Texture Management Functions

      // Load the default texture
      void Load();

      // Load a texture from a path
      void Load(const Path& path_to_texture);

      void Unload();

      #pragma endregion

      #pragma region Binding functions for OpenGL

      void Bind(const Shader& shader, const char* name = "u_texture_diffuse", unsigned int texture_unit = 0) const;
      void Unbind() const;

      #pragma endregion

      #pragma region Operator Overloads

      operator unsigned int() const { return m_texture; }
      operator unsigned char*() const { return m_texture_data; }
      operator bool() const { return m_texture != 0; }

      friend bool operator==(const Texture& lhs, const Texture& rhs) { return lhs.GetTexture() == rhs.GetTexture(); }
      friend bool operator!=(const Texture& lhs, const Texture& rhs) { return lhs.GetTexture() != rhs.GetTexture(); }

      #pragma endregion

      // getter functions

      unsigned int    GetTexture()      const { return m_texture; }
      unsigned char*  GetTextureData()  const { return m_texture_data; }
      int             GetWidth()        const { return m_width; }
      int             GetHeight()       const { return m_height; }
      float           GetWidthF()       const { return static_cast<float>(m_width); }
      float           GetHeightF()      const { return static_cast<float>(m_height); }
      // Helper function for ImGui
      void*           GetTextureImGui() const { return (void*)(intptr_t)m_texture; }

    };

  }
}