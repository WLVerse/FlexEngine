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
		std::vector<FlexECS::Entity> text_slots = scene->Query<TurnOrderDisplay, IsActive, ZIndex, Position, Scale, Rotation, Text, Transform>();
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
				// This is to set the scale and position of the character sprite of the first slot
				(*ds).GetComponent<Scale>()->scale = Vector2(250.f, 250.f);
				(*ds).GetComponent<Position>()->position = { -960.0f, -477.161f };
			}
			else {
				// This is to set the scale and position of the character sprite of the remaining slots
				(*ds).GetComponent<Scale>()->scale = Vector2(100.f, 100.f);
				(*ds).GetComponent<Position>()->position = { -910.0f + (170.f * i), -482.5f };
			}

			// This is to display the character icon
			std::string char_name = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(character.GetComponent<Character>()->character_name);
			std::string sprite_name = get_character_sprite[char_name];
			(*ds).GetComponent<Sprite>()->texture = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(sprite_name);
			(*ds).GetComponent<Transform>()->is_dirty = true;
			++ds;
			// This is to set the background of the sprite
			
			(*ds).GetComponent<IsActive>()->is_active = true;

			if (i == 0) { // i == 0 is just to display this sprite for the first character
				// This is to set the scale and position of the first slot
				(*ds).GetComponent<Scale>()->scale = Vector2(300.f, 300.f);
				(*ds).GetComponent<Position>()->position = { -960.0f, -477.161f };
				if (character.GetComponent<Character>()->is_player) {
					(*ds).GetComponent<Sprite>()->texture = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(R"(\images\UI\UI_BattleScreen_Portrait_Big_Ally.png)");
				}
				else {
					(*ds).GetComponent<Sprite>()->texture = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(R"(\images\UI\UI_BattleScreen_Portrait_Big_Enemy.png)");
				}
			}
			else {
				// This is to set the scale and position of the remaining slots
				(*ds).GetComponent<Scale>()->scale = Vector2(150.f, 150.f);
				(*ds).GetComponent<Position>()->position = { -910.0f + (170.f * i), -482.5f };
				if (character.GetComponent<Character>()->is_player) {
					(*ds).GetComponent<Sprite>()->texture = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(R"(\images\UI\UI_BattleScreen_Portrait_Small_Ally.png)");
				}
				else {
					(*ds).GetComponent<Sprite>()->texture = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(R"(\images\UI\UI_BattleScreen_Portrait_Small_Enemy.png)");
				}
			}
			(*ds).GetComponent<Transform>()->is_dirty = true;
			// Now imma display the text
			if (i == 0) text_slots[i].GetComponent<Position>()->position = { -960.0f, -322.161f };
			else text_slots[i].GetComponent<Position>()->position = { -910.0f + (170.f * i), -402.5f };
			text_slots[i].GetComponent<IsActive>()->is_active = true;
			text_slots[i].GetComponent<Text>()->fonttype = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(R"(\fonts\Electrolize\Electrolize-Regular.ttf)");
			text_slots[i].GetComponent<Text>()->text = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(char_name + ": " + std::to_string(character.GetComponent<Character>()->current_speed));

			++i;
		}
	}
}