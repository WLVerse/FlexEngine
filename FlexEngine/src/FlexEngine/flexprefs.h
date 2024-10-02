/** WLVerse
 * \file flexprefs.h
 *
 * \brief
 *   FlexPrefs is a class that allows you to save and load player preferences in a JSON file.
 *   Usage: Include this header file in the required file.
 *          - Call FlexPrefs::Load() to load the FlexPrefs file. It will automatically create one if none is available.
 *          - Use the getters and setters to get and set values. Eg FlexPrefs::SetInt("score", 100);
 *          - Call FlexPrefs::Save() to update changes made by the getters/setters and save the FlexPrefs file.
 *   Note: Will generate warnings in the console.
 *
 * \authors
 *   [100%] Yew Chong (yewchong.k\@digipen.edu)
 *   - Incorporate flxdata, flxsound, flxprefab
 *
 * \par All content (c) 2024 DigiPen Institute of Technology Singapore. All rights reserved.
 */

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

    /*
      \brief Loads a FlexPrefs file if one exists, otherwise creates a empty one.
    */
    static void Load();

    /*
      \brief Saves the FlexPrefs file. Automatically creates a file if it doesn't exist.
      \param prettify Determines whether to output the json file with indentation and spacing. Default true because who doesn't want readable json?
    */
    static void Save(bool prettify = true);

    /*
      \brief Removes all keys and values from the preferences.
      Use with caution as this is not reversible.
    */
    static void DeleteAll();

    /*
      \brief Checks if the key exists in the JSON file
    */
    static bool HasKey(const std::string& key);

    /*
      \brief Deletes a key from the JSON file, will warn if the key doesn't exist.
    */
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
    // Creates a new FlexPrefs file with default values
    static void Internal_Create();
  };

}