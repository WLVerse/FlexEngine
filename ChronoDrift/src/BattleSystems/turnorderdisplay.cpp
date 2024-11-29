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
		//queue.back().GetComponent<Character>()->current_speed;
		// Speed Queue max 
		//size_t max_length = 300.f * queue.size();

		auto scene = FlexECS::Scene::GetActiveScene();

		int i{ 0 };
		std::vector<FlexECS::Entity> display_slots = scene->Query<TurnOrderDisplay, IsActive, ZIndex, Position, Scale, Shader, Sprite>();
		std::vector<FlexECS::Entity> text_slots = scene->Query<TurnOrderDisplay, Text, IsActive, ZIndex, Position, Scale, Rotation, Transform>();
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
			if (i == 0) {
				(*ds).GetComponent<Scale>()->scale = Vector2(125.f, 125.f);
			}
			else {
				(*ds).GetComponent<Scale>()->scale = Vector2(100.f, 100.f);
			}
			FlexECS::Scene::StringIndex sprite_name = character.GetComponent<Character>()->character_name;
			if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(sprite_name) == "Grace") {
				(*ds).GetComponent<Position>()->position = { -535.f + (200.f * i), -285.f };
			}
			else (*ds).GetComponent<Position>()->position = { -525.f + (200.f * i), -285.f };

			// This is to display the character icon
			std::string texture_name = get_character_sprite[FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(sprite_name)];
			(*ds).GetComponent<Sprite>()->texture = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(texture_name);
			(*ds).GetComponent<Transform>()->is_dirty = true;
			++ds;
			// This is to set the background of the sprite
			
			(*ds).GetComponent<IsActive>()->is_active = true;
			(*ds).GetComponent<Scale>()->scale = Vector2(150.f, 150.f);
			(*ds).GetComponent<Position>()->position = { -530.f + (200.f * i), -285.f };

			// This should never happen ah
			if (i == 0) { // i == 0 is just to display this sprite for the first character
				if (character.GetComponent<Character>()->is_player) {
					(*ds).GetComponent<Scale>()->scale = Vector2(125.f, 125.f);
					(*ds).GetComponent<Sprite>()->texture = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(R"(\images\UI\UI_BattleScreen_Portrait_Big_Ally.png)");
				}
				else {
					(*ds).GetComponent<Sprite>()->texture = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(R"(\images\UI\UI_BattleScreen_Portrait_Big_Enemy.png)");
				}
			}
			else {
				if (character.GetComponent<Character>()->is_player) {
					(*ds).GetComponent<Sprite>()->texture = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(R"(\images\UI\UI_BattleScreen_Portrait_Small_Ally.png)");
				}
				else {
					(*ds).GetComponent<Sprite>()->texture = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(R"(\images\UI\UI_BattleScreen_Portrait_Small_Enemy.png)");
				}
			}
			(*ds).GetComponent<Transform>()->is_dirty = true;

			// Now imma display the text
			if (i == 0) text_slots[i].GetComponent<Position>()->position = { -530.f + (200.f * i), -190.f };
			else text_slots[i].GetComponent<Position>()->position = { -530.f + (200.f * i), -200.f };
			text_slots[i].GetComponent<IsActive>()->is_active = true;
			text_slots[i].GetComponent<Text>()->fonttype = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(R"(\fonts\Electrolize\Electrolize-Regular.ttf)");
			text_slots[i].GetComponent<Text>()->text = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(std::to_string(character.GetComponent<Character>()->current_speed));
			++i;
		}
	}	

}

