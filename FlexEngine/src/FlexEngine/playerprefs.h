#pragma once

#include "RapidJSON/document.h"

using namespace rapidjson;

namespace FlexEngine
{

  class PlayerPrefs
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

    static bool HasKey(std::string const& key);
    static void DeleteKey(std::string const& key);

    #pragma region Getters

    static float GetFloat(std::string const& key, float const& default_value = 0.f);
    static int GetInt(std::string const& key, int const& default_value = 0);
    static std::string GetString(std::string const& key, std::string const& default_value = "");
    static bool GetBool(std::string const& key, bool const& default_value = false);

    #pragma endregion

    #pragma region Setters

    static void SetFloat(std::string const& key, float const& value);
    static void SetInt(std::string const& key, int const& value);
    static void SetString(std::string const& key, std::string const& value);
    static void SetBool(std::string const& key, bool const& value);

    #pragma endregion

  private:

    // INTERNAL FUNCTION
    // Creates a new playerprefs file with default values
    static void Internal_Create();

  };

}