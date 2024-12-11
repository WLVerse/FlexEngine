/*
  \author Yew Chong
  \brief FlxData is a class that allows for easy reading and writing of data to a file. Mostly extends the same functionality as flexprefs.
         
         Usage: Asset::FlxData& f = FLX_ASSET_GET(Asset::FlxData, "/data/flxer.flxdata");
                if (f.GetBool("test", false) == false) Log::Warning("Failed to get bool");
                if (f.GetString("tester", "") != "Hello2 World!") Log::Warning("Failed to get string");
                if (f.GetInt("testint", 0) != 1243) Log::Warning("Failed to get int");
*/
#pragma once

#include "flx_api.h"
#include "RapidJSON/document.h"
#include <string>
using namespace rapidjson;

namespace FlexEngine
{
  namespace Asset
  {
    class __FLX_API FlxData
    {
    public:
      /*
        \brief Constructs an instance of FlxData loaded from key,otherwise creates one with the key.
      */
      FlxData(std::string const& key);

      void Load();
      void Save(bool prettify = true);

      #pragma region Getters

      float GetFloat(const std::string& key, float default_value = 0.f);
      int GetInt(const std::string& key, int default_value = 0);
      std::string GetString(const std::string& key, const std::string& default_value = "");
      bool GetBool(const std::string& key, bool default_value = false);

      #pragma endregion

      #pragma region Setters

      void SetFloat(const std::string& key, float value);
      void SetInt(const std::string& key, int value);
      void SetString(const std::string& key, const std::string& value);
      void SetBool(const std::string& key, bool value);

      #pragma endregion
      
      bool HasKey(const std::string& key) const;
      void DeleteKey(const std::string& key);

      inline bool IsLoaded() const { return m_loaded; }

    private:
      void Internal_Create(std::string const& s = "new_file.flxdata");
      Document document; // Document containing flxdata to read in
      bool m_loaded = false;
      std::string header = "flxdata";
      std::string SAVE_FILE_NAME = "default_filename";
    };
  }
}