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

void ResetCharacters() {
  auto scene = FlexECS::Manager::GetInstance().GetActiveScene();
  for (auto& entity : scene->Query<ChronoDrift::Character>()) {
    auto character = entity.GetComponent<Character>();
    character->current_health = character->base_health;
    character->current_speed = character->base_speed;
  }
  for (auto& entity : scene->Query<Shock>()) {
    entity.RemoveComponent<Shock>();
  }
  for (auto& entity : scene->Query<Burn>()) {
    entity.RemoveComponent<Burn>();
  }
  for (auto& entity : scene->Query<Shear>()) {
    entity.RemoveComponent<Shear>();
  }
  for (auto& entity : scene->Query<Immunity>()) {
    entity.RemoveComponent<Immunity>();
  }
  for (auto& entity : scene->Query<Recovery>()) {
    entity.RemoveComponent<Recovery>();
  }
  for (auto& entity : scene->Query<Stun>()) {
    entity.RemoveComponent<Stun>();
  }
  std::cout << "Reset Complete" << std::endl;
}

void CreateCharacter(Character parameters) {
  auto scene = FlexECS::Manager::GetInstance().GetActiveScene();
  std::string character_name = scene->Internal_StringStorage_Get(parameters.character_name);
  FlexECS::Entity new_character = FlexECS::Manager::GetInstance().CreateEntity(character_name);
  new_character.AddComponent<Character>(parameters);
}

void EditCharacter(std::string character_name, Character parameters) {
  auto scene = FlexECS::Manager::GetInstance().GetActiveScene();
  for (auto& entity : scene->Query<ChronoDrift::Character>()) {
    std::string name_to_check = scene->Internal_StringStorage_Get(*(entity.GetComponent<EntityName>()));
    if (name_to_check == character_name) {
      entity.RemoveComponent<Character>();
      entity.AddComponent<Character>(parameters);
    }
  }
}