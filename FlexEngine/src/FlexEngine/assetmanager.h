// WLVERSE [https://wlverse.web.app]
// assetmanager.h
// 
// The asset manager will look through the entire directory of assets,
// parse everything and assign it to a umap.
// (Basically the File Manager and File List already does this)
// 
// A mesh component will simply hold the key (in this case the file path)
// and perform a lookup everytime it needs it.
// 
// The key is specifically the relative path to the asset from the
// default directory. This is to ensure that the asset manager can
// easily find the asset.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include "assetkey.h"
#include "Utilities/path.h"
#include "Renderer/OpenGL/opengltexture.h"
#include "Renderer/OpenGL/openglshader.h"
#include "Renderer/OpenGL/openglmodel.h"

#include "Renderer/OpenGL/openglfont.h"
#include "fmod/Sound.h"

#include <string>
#include <unordered_map>
#include <variant>

namespace FlexEngine
{

  // Variant of all asset types
  using AssetVariant = std::variant<Asset::Texture, Asset::Shader, Asset::Model, Asset::Sound, Asset::Font>;

  // Helper macro to get an asset by its key.
  // Example usage: FLX_ASSET_GET(Asset::Texture, R"(/images/flexengine/flexengine-256.png)")
  #define FLX_ASSET_GET(TYPE, KEY) std::get<TYPE>(*AssetManager::Get(KEY))

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

    // Frees OpenGL textures and shaders
    static void Unload();

    // Get an asset variant by its key
    static AssetVariant* Get(AssetKey key);

    // Get the default directory
    static Path DefaultDirectory();

#ifdef _DEBUG
    static void Dump();
#endif
  };

}