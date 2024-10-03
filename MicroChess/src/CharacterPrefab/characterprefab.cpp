/* Start Header
*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      characterprefab.cpp
\author    [10%] Ho Jin Jie Donovan, h.jinjiedonovan, 2301233
\par       h.jinjiedonovan\@digipen.edu
\co-author [90%] Kuan Yew Chong
\par       yewchong.k\@digipen.edu
\date      03 October 2024
\brief     This file contains the following functions to load
           and save characters from and to file respectively,
           along with manpulating character data while the game
           is running:
           - SaveCharacters
           - ResetCharacters
           - CreateCharacter
           - EditCharacter
           - LoadCharacter

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "characterprefab.h"

void SaveCharacters() {
  auto scene = FlexECS::Scene::GetActiveScene();
  for (auto& entity : scene->View<ChronoShift::Character>()) {
    scene->SaveEntityAsPrefab(entity, scene->Internal_StringStorage_Get(*(entity.GetComponent<EntityName>())));
  }
  std::cout << "Saved Completed" << std::endl;
}

void ResetCharacters() {
  auto scene = FlexECS::Scene::GetActiveScene();
  for (auto& entity : scene->View<ChronoShift::Character>()) {
    auto character = entity.GetComponent<Character>();
    character->current_health = character->base_health;
    character->current_speed = character->base_speed;
  }
  std::cout << "Reset Complete" << std::endl;
}

void CreateCharacter(Character parameters) {
  auto scene = FlexECS::Scene::GetActiveScene();
  std::string character_name = scene->Internal_StringStorage_Get(parameters.character_name);
  FlexECS::Entity new_character = FlexECS::Scene::CreateEntity(character_name);
  new_character.AddComponent<Character>(parameters);
}

void EditCharacter(std::string character_name, Character parameters) {
  auto scene = FlexECS::Scene::GetActiveScene();
  for (auto& entity : scene->View<ChronoShift::Character>()) {
    std::string name_to_check = scene->Internal_StringStorage_Get(*(entity.GetComponent<EntityName>()));
    if (name_to_check == character_name) {
      entity.RemoveComponent<Character>();
      entity.AddComponent<Character>(parameters);
    }
  }
}

FlexECS::Entity& LoadCharacter(const std::string& prefabName)
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
          if (component_name == "Character")
          {
            Character data;
            type->Deserialize((void*)&data, member);
            new_entity.AddComponent(data);
          }
          else if (component_name == "Action")
          {
            Action data;
            type->Deserialize((void*)&data, member);
            new_entity.AddComponent(data);
          }
          else if (component_name == "Sprite")
          {
            Sprite data;
            type->Deserialize((void*)&data, member);
            new_entity.AddComponent(data);
          }
          else if (component_name == "IsActive")
          {
            IsActive data;
            type->Deserialize((void*)&data, member);
            new_entity.AddComponent(data);
          }
          else if (component_name == "Shader")
          {
            Shader data;
            type->Deserialize((void*)&data, member);
            new_entity.AddComponent(data);
          }
          else if (component_name == "Position")
          {
            Position data;
            type->Deserialize((void*)&data, member);
            new_entity.AddComponent(data);
          }
          else if (component_name == "Scale")
          {
            Scale data;
            type->Deserialize((void*)&data, member);
            new_entity.AddComponent(data);
          }
          else if (component_name == "Rotation")
          {
            Rotation data;
            type->Deserialize((void*)&data, member);
            new_entity.AddComponent(data);
          }
          else if (component_name == "ZIndex")
          {
            ZIndex data;
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