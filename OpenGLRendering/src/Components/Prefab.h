/** WLVerse
 * \file prefab.h
 *
 * \brief This file contains the function to spawn an entity from a prefab file.
 *       This function only works within the context of the project, and the using namespace needs to be edited for it to work in other projects.
 *
 *       Usage: Include Prefab.h in the source file where you want to spawn entities from prefab files.
 *              EntityID e = SpawnEntityFromPrefab("test") where test is the name of the prefab
 *       
 *              From there, you can use the entityID handle to manipulate the entity as you wish.
 *
 *       Note: The prefab creation function is available from the Scene class and can be invoked as follows:
 *              FlexECS::Scene::GetActiveScene()->SaveEntityAsPrefab(object.Get() , "test");
 *
 * \authors
 *   [100%] Yew Chong (yewchong.k\@digipen.edu)
 *
 * \par All content (c) 2024 DigiPen Institute of Technology Singapore. All rights reserved.
 */
#pragma once
#include <FlexEngine.h> // Utility functions
#include "Components/Components.h" // Definition of all game level components.

using namespace OpenGLRendering; // This is solution specific...

/*!
  \brief Spawns an entity with configurations from a prefab file.
  \param prefabName Name of the prefab file.
*/
FlexECS::Entity& SpawnEntityFromPrefab(const std::string& prefabName)
{
  // Open the prefab file
  std::string file_name = prefabName + ".flxprefab";
  Path prefab_path = Path::current("assets\\prefabs\\" + file_name);
  File& prefab_file = File::Open(prefab_path);

  // Formatter to parse metadata first, then deserialize the prefab data
  FlxFmtFile formatter = FlexFormatter::Parse(prefab_file, FlxFmtFileType::Prefab);
  std::string contents = formatter.data;

  // Make it a valid JSON object in the form of an array
  contents.append("]");
  contents.insert(0, "[");

  // Passing it into rapidjson to make life easier
  Document document;
  document.Parse(contents.c_str());
  if (document.HasParseError())
  {
    Log::Error("Failed to parse prefab data.");
  }

  FlexECS::Entity new_entity;
  if (document.IsArray())
  {
    new_entity = FlexECS::Scene::GetActiveScene()->CreateEntity(prefabName); // Should be safe to create a new entity with this name now

    // Loop through each member
    for (auto& member : document.GetArray())
    {
      if (member.IsObject())
      {
        if (member["type"].IsString())
        {
          std::string component_name = member["type"].GetString();
          Reflection::TypeDescriptor* type = TYPE_DESCRIPTOR_LOOKUP[component_name];
          if (component_name == "Transform")
          {
            Transform data;
            type->Deserialize((void*)&data, member);
            new_entity.AddComponent(data);
          }
          else if (component_name == "IsActive")
          {
            IsActive data;
            type->Deserialize((void*)&data, member);
            new_entity.AddComponent(data);
          }
          else if (component_name == "LocalPosition")
          {
            LocalPosition data;
            type->Deserialize((void*)&data, member);
            new_entity.AddComponent(data);
          }
          else if (component_name == "GlobalPosition")
          {
            GlobalPosition data;
            type->Deserialize((void*)&data, member);
            new_entity.AddComponent(data);
          }
          else if (component_name == "Rotation")
          {
            Rotation data;
            type->Deserialize((void*)&data, member);
            new_entity.AddComponent(data);
          }
          else if (component_name == "Scale")
          {
            Scale data;
            type->Deserialize((void*)&data, member);
            new_entity.AddComponent(data);
          }
          else if (component_name == "Model")
          {
            Model data;
            type->Deserialize((void*)&data, member);
            new_entity.AddComponent(data);
          }
          else if (component_name == "Shader")
          {
            Shader data;
            type->Deserialize((void*)&data, member);
            new_entity.AddComponent(data);
          }
          else if (component_name != "uint64_t") // Ignore the binder to the old entity name
          {
            Log::Info("Component type not found: " + component_name);
          }
        }
      }
    }
  }

  // Close the file
  File::Close(prefab_path);

  return new_entity;
}