// WLVERSE [https://wlverse.web.app]
// assetmanager.h
//
// The asset manager will look through the entire directory of assets, parse 
// everything and assign it to a umap. (Basically the File Manager and File List 
// already does this) 
// 
// A mesh component will simply hold the key (in this case the file path) and 
// perform a lookup everytime it needs it. 
// 
// The key is specifically the relative path to the asset from the default 
// directory. This is to ensure that the asset manager can easily find the asset. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include "Assets/battle.h"
#include "Assets/character.h"
#include "Assets/move.h"
#include "Assets/dialogue.h"
#include "Assets/cutscene.h"
#include "Renderer/OpenGL/openglmodel.h"
#include "Renderer/OpenGL/openglshader.h"
#include "Renderer/OpenGL/openglspritesheet.h"
#include "Renderer/OpenGL/opengltexture.h"
#include "Renderer/OpenGL/videodecoder.h"
#include "Utilities/path.h"
#include "assetkey.h"

#include "Renderer/OpenGL/openglfont.h"
#include "fmod/Sound.h"

#include <string>
#include <unordered_map>
#include <variant>

namespace FlexEngine
{

  // Variant of all asset types
  using AssetVariant = std::variant<
    Asset::Texture, Asset::Spritesheet, Asset::Shader, Asset::Model, Asset::Sound, Asset::Font, Asset::Battle,
    Asset::Character, Asset::Move, Asset::Dialogue, Asset::Cutscene, VideoDecoder>;

  // Helper macro to get an asset by its key.
  // Deprecation warning: This macro is deprecated and will be removed in the future.
  // Example usage: FLX_ASSET_GET(Asset::Texture, R"(/images/flexengine/flexengine-256.png)")
  #define FLX_ASSET_GET(TYPE, KEY) AssetManager::Get<TYPE>(KEY)

  class __FLX_API AssetManager
  {
    static Path default_directory;

  public:
    static std::unordered_map<AssetKey, AssetVariant> assets;

    // Add a custom texture asset
    // Saves it to a custom root path (/internal)
    static AssetKey AddTexture(const std::string& assetkey, const Asset::Texture& texture);

    // Load all assets in the directory
    static void Load();

    // Explicitly call this function to free all assets
    // Frees OpenGL textures and shaders
    static void Unload();

    static void LoadFileFromPath(Path path, Path _default_directory);

    #pragma region Getter

  public:
    // Get an asset variant by its key
    // This will return a reference to the asset or throw an error if the asset is not found
    template <typename T>
    static T& Get(AssetKey key)
    {
      auto asset = Internal_Get(key);
      if (asset == nullptr)
      {
        throw std::runtime_error("Asset key does not exist in the asset manager: " + key);
      }
      auto asset_ptr = std::get_if<T>(asset);
      if (asset_ptr == nullptr)
      {
        throw std::runtime_error("Asset key does not match the requested type: " + key);
      }
      return *asset_ptr;
    }

  private:
    // INTERNAL FUNCTION
    // Get an asset variant by its key
    // Returns nullptr if the asset is not found
    // 
    // Replaces all / or \ in the key with the platform specific separator
    // This is to ensure that the key is always the same regardless of the platform
    static AssetVariant* Internal_Get(AssetKey key);

    #pragma endregion

  public:
    // Get the default directory
    static Path DefaultDirectory();

#ifdef _DEBUG
    static void Dump();
#endif
  };

}