/* Start Header
*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      turnorderdisplay.cpp
\author    [100%] Ho Jin Jie Donovan, h.jinjiedonovan, 2301233
\par       h.jinjiedonovan\@digipen.edu
\date      03 October 2024
\brief     This file contains the definition of the function
					 that will display the turn order of both the players
					 and enemies on the left side of the screen

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "turnorderdisplay.h"
#include "Components/battlecomponents.h"
#include "Components/rendering.h"

namespace ChronoDrift 
{

	//The Sprite2d renderer will automatically draw it, so we just set the positions, colors, etc.
	void DisplayTurnOrder(std::vector<FlexECS::Entity>& queue)
	{
		auto scene = FlexECS::Scene::GetActiveScene();

		int i{ 0 };
		int count{ 0 };
		std::vector<FlexECS::Entity> display_slots = scene->Query<TurnOrderDisplay, IsActive, ZIndex, Position, Scale, Shader, Sprite>();
		for (auto ds = display_slots.begin(); ds != display_slots.end(); ds++) {
			if (i >= queue.size())
			{
				(*ds).GetComponent<IsActive>()->is_active = false;
				continue;
			}
			std::vector<FlexECS::Entity>::const_iterator it = queue.begin();
			std::advance(it, i);
			auto character = *it;

			(*ds).GetComponent<IsActive>()->is_active = true;
			(*ds).GetComponent<Position>()->position = { -450.f + (300.f * i), -275.f };
			//entity.GetComponent<Sprite>()->color = character.GetComponent<Sprite>()->color;
			//entity.GetComponent<Sprite>()->color_to_add = character.GetComponent<Sprite>()->color_to_add;
			//entity.GetComponent<Sprite>()->color_to_multiply = character.GetComponent<Sprite>()->color_to_multiply;

			// This should never happen ah
			FlexECS::Scene::StringIndex sprite_name = character.GetComponent<Character>()->character_name;
			std::string texture_name = get_character_sprite[FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(sprite_name)];
			(*ds).GetComponent<Sprite>()->texture = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(texture_name);
			++ds;
			// This is to set the background of the sprite
			
			(*ds).GetComponent<IsActive>()->is_active = true;
			(*ds).GetComponent<Scale>()->scale = Vector2(200.f, 200.f);
			(*ds).GetComponent<Position>()->position = { -450.f + (300.f * i), -275.f};

			// This should never happen ah
			if (character.GetComponent<Character>()->is_player) {
				(*ds).GetComponent<Sprite>()->texture = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(R"(\images\UI\UI_BattleScreen_Portrait_Small_Ally.png)");
			}
			else {
				(*ds).GetComponent<Sprite>()->texture = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(R"(\images\UI\UI_BattleScreen_Portrait_Small_Enemy.png)");
			}
			++i;
		}
	//	for (auto entity : )
	//	{
	//		if (i >= queue.size())
	//		{
	//			entity.GetComponent<IsActive>()->is_active = false;
	//			continue;
	//		}
	//		std::vector<FlexECS::Entity>::const_iterator it = queue.begin();
	//		std::advance(it, i);
	//		auto character = *it;

	//		entity.GetComponent<IsActive>()->is_active = true;
	//		entity.GetComponent<Position>()->position = { -450.f + (300.f * count), -275.f };
	//		//entity.GetComponent<Sprite>()->color = character.GetComponent<Sprite>()->color;
	//		//entity.GetComponent<Sprite>()->color_to_add = character.GetComponent<Sprite>()->color_to_add;
	//		//entity.GetComponent<Sprite>()->color_to_multiply = character.GetComponent<Sprite>()->color_to_multiply;
	//		
	//		// This should never happen ah
	//		FlexECS::Scene::StringIndex sprite_name = character.GetComponent<Character>()->character_name;
	//		std::string texture_name = get_character_sprite[FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(sprite_name)];
	//		entity.GetComponent<Sprite>()->texture = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(texture_name);
	//		++i;
	//		// This is to set the background of the sprite
	//		
	//		//entity.GetComponent<IsActive>()->is_active = true;
	//		//entity.GetComponent<Position>()->position = { -450.f + (300.f * count), -275.f};

	//		//// This should never happen ah
	//		//if (character.GetComponent<Character>()->is_player) {
	//		//	entity.GetComponent<Sprite>()->texture = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(R"(\images\UI\UI_BattleScreen_Portrait_Small_Ally.png)");
	//		//}
	//		//else {
	//		//	entity.GetComponent<Sprite>()->texture = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(R"(\images\UI\UI_BattleScreen_Portrait_Small_Enemy.png)");
	//		//}
	//		//++i;
	//		++count;
	//	}
	}	

}

