#pragma once

#include "flx_api.h"

#include "RapidJSON/document.h"
using namespace rapidjson;

namespace FlexEngine
{

  class __FLX_API FlexPrefs
  {

    static Document m_document;

  public:

    // Automatically creates a new save if it doesn't exist.
    static void Load();

    // Prettify parameter determines whether to output the json file with indentation and spacing.
    // Automatically creates a file if it doesn't exist.
    static void Save(bool prettify = false);

    // Removes all keys and values from the preferences.
    // Use with caution.
    static void DeleteAll();

    static bool HasKey(const std::string& key);
    static void DeleteKey(const std::string& key);

    #pragma region Getters

    static float GetFloat(const std::string& key, float default_value = 0.f);
    static int GetInt(const std::string& key, int default_value = 0);
    static std::string GetString(const std::string& key, const std::string& default_value = "");
    static bool GetBool(const std::string& key, bool default_value = false);

    #pragma endregion

    #pragma region Setters

    static void SetFloat(const std::string& key, float value);
    static void SetInt(const std::string& key, int value);
    static void SetString(const std::string& key, const std::string& value);
    static void SetBool(const std::string& key, bool value);

    #pragma endregion

  private:

    // INTERNAL FUNCTION
    // Creates a new playerprefs file with default values
    static void Internal_Create();

  };

}