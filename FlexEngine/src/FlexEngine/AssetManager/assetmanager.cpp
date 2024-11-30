#include "pch.h"

#include "assetmanager.h"

#include "Wrapper/assimp.h"
#include "FMOD/FMODWrapper.h"

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
      Log::Warning(std::string("AssetManager: Asset already exists: ") + key);
      return key;
    }

    // add texture to assets
    assets[key] = texture;
    Log::Debug(std::string("AssetManager: Added texture: ") + key);
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
    std::unordered_map<std::string, std::array<File*, 3>> shader_linker;

    Log::Flow("Loading assets...");
    // iterate through all files in the directory
    list.each(
      [&default_directory_length, &shader_linker](File& file)
      {
        // determine what type of asset it is
        // 
        // currently supported:
        // - textures
        // - shaders
        // - models
        // - audio
        // - flxscene data
        // - font
        
        auto file_extension = file.path.extension();

        if (FLX_EXTENSIONS_CHECK_SAFETY("image", file_extension.string()))
        {
          // create an asset key
          AssetKey key = file.path.string().substr(default_directory_length);
          
          // load texture
          assets[key] = Asset::Texture::Null;
          Asset::Texture& texture = std::get<Asset::Texture>(assets[key]);
          texture.Load(file.path);
          Log::Debug("Loaded texture: " + key);
        }
        else if (FLX_EXTENSIONS_CHECK_SAFETY("shader", file_extension.string()))
        {
          // ignore .glsl and .hlsl files
          // they are not currently supported
          if (file_extension.string() == ".glsl" || file_extension.string() == ".hlsl")
          {
            Log::Warning(std::string("Unsupported shader type: ") + file_extension.string());
            return;
          }

          std::string file_name = file.path.stem().string();

          Asset::Shader::Type shader_type;
               if (file_extension.string() == ".vert") shader_type = Asset::Shader::Type::Vertex;
          else if (file_extension.string() == ".frag") shader_type = Asset::Shader::Type::Fragment;
          else if (file_extension.string() == ".geom") shader_type = Asset::Shader::Type::Geometry;
          else
          {
            Log::Warning(std::string("Unsupported shader type: ") + file_extension.string());
            return;
          }

          // add it to the linker
          // the shader linker will link shaders with the same name after all shaders are loaded
          if (shader_linker.count(file_name) == 0)
          {
            // create a new entry in the linker
            shader_linker[file_name] = {nullptr, nullptr, nullptr};
          }

          // add the file to the linker
          shader_linker[file_name][shader_type] = &file;
        }
        else if (FLX_EXTENSIONS_CHECK_SAFETY("model", file_extension.string()))
        {
          // create an asset key
          AssetKey key = file.path.string().substr(default_directory_length);

          // load and save the model
          Log::Flow("Loading model: " + key);
          FLX_FLOW_BEGINSCOPE();
          FLX_SCOPED_TIMER("Loaded model: " + key);
          Asset::Model loaded_model = AssimpWrapper::LoadModel(file.path);

          if (loaded_model)
          {
            //Log::Debug("Loaded model: " + key);
            assets[key] = loaded_model;
          }
          FLX_FLOW_ENDSCOPE();
        }
        else if (FLX_EXTENSIONS_CHECK_SAFETY("audio", file_extension.string()))
        {
          AssetKey key = file.path.string().substr(default_directory_length);
          FLX_FLOW_BEGINSCOPE();
          assets[key] = Asset::Sound{ key }; // create sound asserts on FMOD side and shouldn't need here
          FLX_FLOW_ENDSCOPE();
        }
        else if (FLX_EXTENSIONS_CHECK_SAFETY("flx", file_extension.string()))
        {
          if (file_extension.string() == ".flxprefab")
          {
            // Read in flexprefab not needed
          }
          else if (file_extension.string() == ".flxdata")
          {
            // Write flxdata support
            AssetKey key = file.path.string().substr(default_directory_length);
            FLX_FLOW_BEGINSCOPE();
            assets[key] = Asset::FlxData{ key };
            FLX_FLOW_ENDSCOPE();
          }
        }
        else if (FLX_EXTENSIONS_CHECK_SAFETY("font", file_extension.string()))
        {
            // create an asset key
            AssetKey key = file.path.string().substr(default_directory_length);
            
            // load font
            FLX_FLOW_BEGINSCOPE();
            assets[key] = Asset::Font{key};
            FLX_FLOW_ENDSCOPE();
            Log::Info("Loaded font path: " + key);
        }
      }
    );

    // perform shader linking
    // currently geometry shaders are not supported
    Log::Flow("Linking shaders...");
    for (auto& [key, files] : shader_linker)
    {
      // guard: check if all shaders are present
      if (files[Asset::Shader::Type::Vertex] == nullptr || files[Asset::Shader::Type::Fragment] == nullptr)
      {
        Log::Error(std::string("Shader ") + key + " is missing a vertex or fragment shader!");
        continue;
      }

      // create an asset key
      std::string key_string = files[0]->path.string().substr(default_directory_length);
      key_string = key_string.substr(0, key_string.find_last_of('.'));
      AssetKey assetkey = key_string;

      // create a new shader
      assets[assetkey] = Asset::Shader();
      Asset::Shader& shader = std::get<Asset::Shader>(assets[assetkey]);
      shader.Create(files[Asset::Shader::Type::Vertex]->path, files[Asset::Shader::Type::Fragment]->path);
      Log::Debug("Loaded shader: " + assetkey);
    }

    FLX_FLOW_ENDSCOPE();
  }

  void AssetManager::Unload()
  {
    FLX_FLOW_FUNCTION();

    FMODWrapper::Core::ForceStop(); // As weird as this looks, I need this to be here so I can free sounds safely. - YC

    for (auto& [key, asset] : assets)
    {
      std::visit(
        [](auto&& arg)
        {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, Asset::Texture>)
          {
            arg.Unload();
          }
          else if constexpr (std::is_same_v<T, Asset::Shader>)
          {
            arg.Destroy();
          }
          else if constexpr (std::is_same_v<T, Asset::Font>)
          {
            arg.Unload();
          }
          else if constexpr (std::is_same_v<T, Asset::Sound>)
          {
            arg.Unload();
          }
        },
        asset
      );
    }

    assets.clear();
  }


  AssetVariant* AssetManager::Get(AssetKey key)
  {
    // replace all / or \ in the key with the platform specific separator
    // this is to ensure that the key is always the same
    // regardless of the platform
    std::replace(key.begin(), key.end(), '/', Path::separator);
    std::replace(key.begin(), key.end(), '\\', Path::separator);

    if (assets.count(key) == 0)
    {
      Log::Error(std::string("Asset not found: ") + key);
      return nullptr;
    }
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
          else if constexpr (std::is_same_v<T, Asset::Model>)
          {
            Log::Debug("Type: Model");
            Log::Debug("Meshes: " + std::to_string(arg.meshes.size()));
          }
        },
        asset
      );
      Log::Debug(std::string(10, '-'));
    }
    Log::Debug("End of dump.");
  }
#endif

}
