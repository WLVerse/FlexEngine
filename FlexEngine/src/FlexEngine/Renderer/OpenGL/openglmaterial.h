// WLVERSE [https://wlverse.web.app]
// openglmaterial.h
// 
// Phong material class implementation for OpenGL renderer.
// Specular is not working yet.
// 
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include "AssetManager/assetkey.h"
#include "Renderer/OpenGL/opengltexture.h"

#include <vector>
#include <variant>

namespace FlexEngine
{
  namespace Asset
  {

    // Represents a material with a diffuse texture, a specular texture, and a shininess value
    // This follows the Phong lighting model instead of the PBR model
    class __FLX_API Material
    {
    public:
      // This variant allows for the Material class to store either AssetKeys or Textures
      // 
      // This is useful for handling textures that are embedded in a model because they
      // won't have an AssetKey associated with them and are not stored in the AssetManager.
      //using TextureVariant = std::variant<AssetKey, Texture>;
      using TextureVariant = AssetKey;

    public:
      TextureVariant diffuse;
      TextureVariant specular;
      float shininess;

    public:
      Material(
        TextureVariant _diffuse,
        TextureVariant _specular,
        float _shininess = 32.0f
      );

      #pragma region Getter Functions

      const Texture* GetDiffuse() const;

      // Returns a pair of the specular texture and the shininess value
      std::pair<const Texture*, float> GetSpecular() const;

      //std::vector<const Texture*> GetDiffuse() const;
      //std::vector<const Texture*> GetSpecular() const;

      #pragma endregion

      #pragma region Operator Overloads

      bool operator==(const Material& other) const;
      bool operator!=(const Material& other) const;

      #pragma endregion
    };

  }
}
