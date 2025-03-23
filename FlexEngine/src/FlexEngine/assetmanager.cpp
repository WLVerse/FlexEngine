// WLVERSE [https://wlverse.web.app]
// assetmanager.cpp
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

#include "pch.h"

#include "assetmanager.h"

// #include "Utilities/assimp.h"

namespace FlexEngine
{

  // static member initialization
  Path AssetManager::default_directory = Path::current("assets");
  std::unordered_map<AssetKey, AssetVariant> AssetManager::assets;

  AssetKey AssetManager::AddTexture(const std::string& assetkey, const Asset::Texture& texture)
  {
    // create assetkey
    AssetKey key = R"(\internal)" + std::string((assetkey[0] == '\\') ? "" : "\\") + assetkey;

    // guard: asset already exists
    if (assets.count(key) != 0)
    {
      Log::Warning(
        "The internal texture already exists. "
        "Instead of creating a new one, we reuse the one that already exists. "
        "Asset key: " +
        std::string(key)
      );
      return key;
    }

    // add texture to assets
    assets[key] = texture;
    Log::Info("Added internal texture to the asset manager. Asset key: " + std::string(key));
    return key;
  }

  void AssetManager::Load()
  {
    FLX_FLOW_BEGINSCOPE();
    FLX_SCOPED_TIMER("AssetManager");

    // load all assets
    FileList list = FileList::GetAllFilesInDirectoryRecursively(default_directory);

    // guard
    if (list.size() == 0)
    {
      Log::Info(std::string("No assets found in directory: ") + default_directory.string());
      return;
    }

    // used to create an asset key
    // substr the path starting after the length of the default directory
    std::size_t default_directory_length = default_directory.string().length();

    // used to link shaders together
    // shaders with the same name will be linked together
    // looks for .vert, .frag, and .geom files
    // .glsl and .hlsl files are currently not supported
    // but in the future they will skip this linker because
    // they contain all the necessary information in one file
    // std::unordered_map<std::string, std::array<File*, 3>> shader_linker;

    Log::Flow("Loading assets...");
    // iterate through all files in the directory
    list.each(
      [&default_directory_length](File& file)
      {
        // determine what type of asset it is
        //
        // currently supported:
        // - textures
        // - spritesheets
        // - shaders
        // - models
        // - sounds
        // - fonts
        // - battles
        // - characters
        // - moves
        // - dialogues

        auto file_extension = file.path.extension();

        if (file_extension.string() == ".jpg" || file_extension.string() == ".jpeg" ||
            file_extension.string() == ".png")
        {
          // create an asset key
          AssetKey key = file.path.string().substr(default_directory_length);

          // load texture
          assets.emplace(key, Asset::Texture());
          Asset::Texture& texture = std::get<Asset::Texture>(assets[key]);
          texture.Load(file.path);
          //Log::Info("Loaded texture: " + key);
        }
        else if (file_extension.string() == ".flxshader")
        {
          // create an asset key
          AssetKey key = file.path.string().substr(default_directory_length);

          // load shader
          assets.emplace(key, Asset::Shader());
          Asset::Shader& shader = std::get<Asset::Shader>(assets[key]);
          shader.Load(file.path);
          //Log::Info("Loaded shader: " + key);
        }
        else if (file_extension.string() == ".mp3" || file_extension.string() == ".wav")
        {
          FLX_FLOW_BEGINSCOPE();
          AssetKey key = file.path.string().substr(default_directory_length);
          assets[key] = Asset::Sound{ key }; // create sound asserts on FMOD side and shouldn't need here
          //Log::Info("Loaded sound path: " + key);
          FLX_FLOW_ENDSCOPE();
        }
        else if (file_extension.string() == ".mp4")
        {
          AssetKey key = file.path.string().substr(default_directory_length);
          assets.emplace(key, VideoFrame());
          VideoFrame& video = std::get<VideoFrame>(assets[key]);
          video.Load(file.path);
        }
        else if (file_extension.string() == ".ttf")
        {
          FLX_FLOW_BEGINSCOPE();
          AssetKey key = file.path.string().substr(default_directory_length);
          assets[key] = Asset::Font{ key };
          FLX_FLOW_ENDSCOPE();
          //Log::Info("Loaded font path: " + key);
        }
        else if (file_extension.string() == ".flxspritesheet")
        {
          // create an asset key
          AssetKey key = file.path.string().substr(default_directory_length);

          // load spritesheet
          assets.emplace(key, Asset::Spritesheet(file));
          //Log::Info("Loaded spritesheet: " + key);
        }
        else if (file_extension.string() == ".flxbattle")
        {
          // create an asset key
          AssetKey key = file.path.string().substr(default_directory_length);
          // load battle
          assets.emplace(key, Asset::Battle(file));
          //Log::Info("Loaded battle: " + key);
        }
        else if (file_extension.string() == ".flxcharacter")
        {
          // create an asset key
          AssetKey key = file.path.string().substr(default_directory_length);
          // load character
          assets.emplace(key, Asset::Character(file));
          //Log::Info("Loaded character: " + key);
        }
        else if (file_extension.string() == ".flxmove")
        {
          // create an asset key
          AssetKey key = file.path.string().substr(default_directory_length);
          // load move
          assets.emplace(key, Asset::Move(file));
          //Log::Info("Loaded move: " + key);
        }
        else if (file_extension.string() == ".flxdialogue")
        {
            // create an asset key
            AssetKey key = file.path.string().substr(default_directory_length);
            // load dialogue
            assets.emplace(key, Asset::Dialogue(file));
            //Log::Info("Dialogue file: " + key);
        }
        else if (file_extension.string() == ".flxcutscene")
        {
            // create an asset key
            AssetKey key = file.path.string().substr(default_directory_length);
            // load move
            Asset::Cutscene test = Asset::Cutscene(file);
            assets.emplace(key, test);
            //Log::Info("Dialogue file: " + key);
        }
        else { Log::Warning("Unsupported file type: " + file.path.string()); }
      }
    );

    FLX_FLOW_ENDSCOPE();
  }

  void AssetManager::Unload()
  {
    FLX_FLOW_FUNCTION();

    for (auto& [key, asset] : assets)
    {
      std::visit(
        [](auto&& arg)
        {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, Asset::Texture>)
            arg.Unload();
          else if constexpr (std::is_same_v<T, Asset::Shader>)
            arg.Destroy();
        },
        asset
      );
    }
  }

  void AssetManager::LoadFileFromPath(Path path, Path _default_directory)
  {
    File file = File::Open(path);

    std::size_t default_directory_length = _default_directory.string().length();

    auto file_extension = file.path.extension();

    if (file_extension.string() == ".jpg" || file_extension.string() == ".jpeg" ||
        file_extension.string() == ".png")
    {
      // create an asset key
      AssetKey key = file.path.string().substr(default_directory_length);

      // load texture
      assets.emplace(key, Asset::Texture());
      Asset::Texture& texture = std::get<Asset::Texture>(assets[key]);
      texture.Load(file.path);
      Log::Info("Loaded texture: " + key);
    }
    else if (file_extension.string() == ".flxshader")
    {
      // create an asset key
      AssetKey key = file.path.string().substr(default_directory_length);

      // load shader
      assets.emplace(key, Asset::Shader());
      Asset::Shader& shader = std::get<Asset::Shader>(assets[key]);
      shader.Load(file.path);
      Log::Info("Loaded shader: " + key);
    }
    else if (file_extension.string() == ".mp3" || file_extension.string() == ".wav")
    {
      FLX_FLOW_BEGINSCOPE();
      AssetKey key = file.path.string().substr(default_directory_length);
      assets[key] = Asset::Sound{ key }; // create sound asserts on FMOD side and shouldn't need here
      Log::Info("Loaded sound path: " + key);
      FLX_FLOW_ENDSCOPE();
    }
    else if (file_extension.string() == ".ttf")
    {
      FLX_FLOW_BEGINSCOPE();
      AssetKey key = file.path.string().substr(default_directory_length);
      assets[key] = Asset::Font{ key };
      FLX_FLOW_ENDSCOPE();
      Log::Info("Loaded font path: " + key);
    }
    else if (file_extension.string() == ".flxspritesheet")
    {
      // create an asset key
      AssetKey key = file.path.string().substr(default_directory_length);

      // load spritesheet
      assets.emplace(key, Asset::Spritesheet(file));
      Log::Info("Loaded spritesheet: " + key);
    }
    else if (file_extension.string() == ".flxbattle")
    {
      // create an asset key
      AssetKey key = file.path.string().substr(default_directory_length);
      // load battle
      assets.emplace(key, Asset::Battle(file));
      Log::Info("Loaded battle: " + key);
    }
    else if (file_extension.string() == ".flxcharacter")
    {
      // create an asset key
      AssetKey key = file.path.string().substr(default_directory_length);
      // load character
      assets.emplace(key, Asset::Character(file));
      Log::Info("Loaded character: " + key);
    }
    else if (file_extension.string() == ".flxmove")
    {
      // create an asset key
      AssetKey key = file.path.string().substr(default_directory_length);
      // load move
      assets.emplace(key, Asset::Move(file));
      Log::Info("Loaded move: " + key);
    }
    else { Log::Warning("Unsupported file type: " + file.path.string()); }
  
  }

  AssetVariant* AssetManager::Internal_Get(AssetKey key)
  {
    std::replace(key.begin(), key.end(), '/', Path::separator);
    std::replace(key.begin(), key.end(), '\\', Path::separator);

    if (assets.count(key) == 0) return nullptr;
    return &assets[key];
  }

  Path AssetManager::DefaultDirectory()
  {
    return default_directory;
  }


#ifdef _DEBUG
  void AssetManager::Dump()
  {
    Log::Debug("AssetManager:");
    for (auto& [key, asset] : assets)
    {
      Log::Debug("Key: " + key);
      std::visit(
        [](auto&& arg)
        {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, Asset::Texture>)
          {
            Log::Debug("Type: Texture");
            Log::Debug("Size: [" + std::to_string(arg.GetWidth()) + ", " + std::to_string(arg.GetHeight()) + "]");
          }
          else if constexpr (std::is_same_v<T, Asset::Shader>)
          {
            Log::Debug("Type: Shader");
            arg.Dump();
          }
        },
        asset
      );
      Log::Debug(std::string(10, '-'));
    }
    Log::Debug("End of dump.");
  }
#endif
} // namespace FlexEngine