// WLVERSE [https://wlverse.web.app]
// physicslayer.cpp
//
// Battle layer for the editor.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "Layers.h"
//#include "battlestuff.h"

#if 0
namespace Game
{

    // intermediary oop data structure to store the battle data
    // we're doing this because our team is struggling with the ECS system

    Battle battle;

    void Start_Of_Turn();
    void Move_Select();
    void Move_Resolution();
    void End_Of_Turn();
    void Win_Battle();
    void Lose_Battle();

    void Internal_ParseBattle2(AssetKey assetkey)
    {
        //get positions of character icons and health bar.
        for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<CharacterSlot>())
        {
            auto& character_slot = *entity.GetComponent<CharacterSlot>();
            battle.sprite_slot_positions[character_slot.slot_number - 1] = entity.GetComponent<Position>()->position;
        }
        for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<HealthbarSlot>())
        {
            auto& healthbar_slot = *entity.GetComponent<HealthbarSlot>();
            battle.healthbar_slot_positions[healthbar_slot.slot_number - 1] = entity.GetComponent<Position>()->position;
        }

        // get the battle asset
        auto& asset = FLX_ASSET_GET(Asset::Battle, assetkey);

        // reserve the vectors
        // pushing back will invalidate the pointers in the arrays
        // TODO: this is a temporary solution
        // 
        //std::vector<Character> drifters = {};
        //std::vector<Character> enemies = {};

        // parse the battle data
        int index = 0;
        for (auto& slot : asset.GetDrifterSlots())
        {
            Character character;
            if (*slot == "None")
            {
                index++;
                continue;
            }
            auto& character_asset = FLX_ASSET_GET(Asset::Character, *slot);
            character.name = character_asset.character_name;
            character.health = character_asset.health;
            character.speed = character_asset.speed;
            character.character_id = character_asset.character_id;

            character.current_health = character.health;
            character.current_speed = character.speed;
            character.is_alive = true;

            if (character_asset.move_one != "None")
            {
                auto& move_one_asset = FLX_ASSET_GET(Asset::Move, character_asset.move_one);

                Move move_one;
                move_one.name = move_one_asset.name;
                move_one.speed = move_one_asset.speed;

                move_one.effect = move_one_asset.effect;
                move_one.value = move_one_asset.value;
                move_one.target = move_one_asset.target;

                move_one.description = move_one_asset.description;
                character.move_one = move_one;
            }

            if (character_asset.move_two != "None")
            {
                auto& move_two_asset = FLX_ASSET_GET(Asset::Move, character_asset.move_two);

                Move move_two;
                move_two.name = move_two_asset.name;
                move_two.speed = move_two_asset.speed;

                move_two.effect = move_two_asset.effect;
                move_two.value = move_two_asset.value;
                move_two.target = move_two_asset.target;

                move_two.description = move_two_asset.description;
                character.move_two = move_two;
            }

            if (character_asset.move_three != "None")
            {
                auto& move_three_asset = FLX_ASSET_GET(Asset::Move, character_asset.move_three);

                Move move_three;
                move_three.name = move_three_asset.name;
                move_three.speed = move_three_asset.speed;

                move_three.effect = move_three_asset.effect;
                move_three.value = move_three_asset.value;
                move_three.target = move_three_asset.target;

                move_three.description = move_three_asset.description;
                character.move_three = move_three;
            }

            character.current_slot = index;

            battle.drifters_and_enemies.push_back(&character);
            battle.speed_bar.push_back(&character);

            FlexECS::Entity character_sprite = FlexECS::Scene::GetActiveScene()->CreateEntity(character.name); // can always use GetEntityByName to find the entity
            character_sprite.AddComponent<Transform>({});
            character_sprite.AddComponent<Character>({});

            // find the slot position
            character_sprite.AddComponent<Position>({ battle.sprite_slot_positions[character.current_slot] });
            character_sprite.AddComponent<Rotation>({});
            character_sprite.AddComponent<Sprite>({  });

            switch (character.character_id)
            {
            case 1:
                character_sprite.AddComponent<Scale>({ Vector3(2, 2, 0) });
                character_sprite.AddComponent<Animator>(
                  { FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
                    FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Attack_Anim_Sheet.flxspritesheet)") }
                );
                break;
            case 2:
                character_sprite.AddComponent<Scale>({ Vector3(2, 2, 0) });
                character_sprite.AddComponent<Animator>(
                  { FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)"),
                    FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)") }
                );
                break;
            }
            character_sprite.AddComponent<ZIndex>({ 21 + index });

            FlexECS::Entity character_healthbar = FlexECS::Scene::GetActiveScene()->CreateEntity(character.name + " Healthbar"); // can always use GetEntityByName to find the entity
            character_healthbar.AddComponent<Healthbar>({});
            character_healthbar.AddComponent<Transform>({});
            character_healthbar.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot] });
            character_healthbar.AddComponent<Rotation>({});
            character_healthbar.AddComponent<Scale>({ Vector3(.1, .1, 0) });
            character_healthbar.GetComponent<Healthbar>()->original_position = character_healthbar.GetComponent<Position>()->position;
            character_healthbar.GetComponent<Healthbar>()->original_scale = character_healthbar.GetComponent<Scale>()->scale;
            character_healthbar.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_HealthBar_Green.png)") });
            character_healthbar.AddComponent<ZIndex>({ 21 + index });

            character_healthbar = FlexECS::Scene::GetActiveScene()->CreateEntity(character.name + " Stats"); // can always use GetEntityByName to find the entity
            character_healthbar.AddComponent<Transform>({});
            character_healthbar.AddComponent<Position>({ battle.sprite_slot_positions[character.current_slot] + Vector3(-30, -100, 0) });
            character_healthbar.AddComponent<Rotation>({});
            character_healthbar.AddComponent<Scale>({ Vector3(0.3f, 0.3f, 0) });
            character_healthbar.AddComponent<ZIndex>({ 21 + index });
            character_healthbar.AddComponent<Text>({
              FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
              FLX_STRING_NEW(R"(Lorem Ipsum)"),
              Vector3(1.0f, 1.0, 1.0f),
              { Renderer2DText::Alignment_Left, Renderer2DText::Alignment_Center },
              {                            600,                              320 }
            });

            FlexECS::Entity character_attack_buff = FlexECS::Scene::GetActiveScene()->CreateEntity(character.name + " Attack_Buff"); // can always use GetEntityByName to find the entity
            character_attack_buff.AddComponent<Transform>({});
            character_attack_buff.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot] + Vector3(-110, 0, 0) });
            character_attack_buff.AddComponent<Rotation>({});
            character_attack_buff.AddComponent<Scale>({ Vector3(.05, .05, 0) });
            character_attack_buff.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Attack_+1.png)") });
            character_attack_buff.AddComponent<ZIndex>({ 21 + index });

            FlexECS::Entity character_attack_debuff = FlexECS::Scene::GetActiveScene()->CreateEntity(character.name + " Attack_Debuff"); // can always use GetEntityByName to find the entity
            character_attack_debuff.AddComponent<Transform>({});
            character_attack_debuff.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot] + Vector3(-70, 0, 0) });
            character_attack_debuff.AddComponent<Rotation>({});
            character_attack_debuff.AddComponent<Scale>({ Vector3(.05, .05, 0) });
            character_attack_debuff.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Attack_-1.png)") });
            character_attack_debuff.AddComponent<ZIndex>({ 21 + index });

            FlexECS::Entity character_stun_debuff = FlexECS::Scene::GetActiveScene()->CreateEntity(character.name + " Stun_Debuff"); // can always use GetEntityByName to find the entity
            character_stun_debuff.AddComponent<Transform>({});
            character_stun_debuff.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot] + Vector3(-30, 0, 0) });
            character_stun_debuff.AddComponent<Rotation>({});
            character_stun_debuff.AddComponent<Scale>({ Vector3(.05, .05, 0) });
            character_stun_debuff.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Stun.png)") });
            character_stun_debuff.AddComponent<ZIndex>({ 21 + index });

            FlexECS::Entity character_shield_buff = FlexECS::Scene::GetActiveScene()->CreateEntity(character.name + " Shield_Buff"); // can always use GetEntityByName to find the entity
            character_shield_buff.AddComponent<Transform>({});
            character_shield_buff.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot] + Vector3(10, 0, 0) });
            character_shield_buff.AddComponent<Rotation>({});
            character_shield_buff.AddComponent<Scale>({ Vector3(.05, .05, 0) });
            character_shield_buff.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Def_+1.png)") });
            character_shield_buff.AddComponent<ZIndex>({ 21 + index });

            FlexECS::Entity character_protect_buff = FlexECS::Scene::GetActiveScene()->CreateEntity(character.name + " Protect_Buff"); // can always use GetEntityByName to find the entity
            character_protect_buff.AddComponent<Transform>({});
            character_protect_buff.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot] + Vector3(50, 0, 0) });
            character_protect_buff.AddComponent<Rotation>({});
            character_protect_buff.AddComponent<Scale>({ Vector3(.05, .05, 0) });
            character_protect_buff.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Heal_+1.png)") });
            character_protect_buff.AddComponent<ZIndex>({ 21 + index });

            index++;
        }

        index = 0;

        for (auto& slot : asset.GetEnemySlots())
        {
            Character character;
            if (*slot == "None")
            {
                index++;
                continue;
            }
            auto& character_asset = FLX_ASSET_GET(Asset::Character, *slot);
            character.name = character_asset.character_name;
            character.health = character_asset.health;
            character.speed = character_asset.speed;
            character.character_id = character_asset.character_id;

            character.current_health = character.health;
            character.current_speed = character.speed;
            character.is_alive = true;

            if (character_asset.move_one != "None")
            {
                auto& move_one_asset = FLX_ASSET_GET(Asset::Move, character_asset.move_one);

                Move move_one;
                move_one.name = move_one_asset.name;
                move_one.speed = move_one_asset.speed;

                move_one.effect = move_one_asset.effect;
                move_one.value = move_one_asset.value;
                move_one.target = move_one_asset.target;

                move_one.description = move_one_asset.description;
                character.move_one = move_one;
            }

            if (character_asset.move_two != "None")
            {
                auto& move_two_asset = FLX_ASSET_GET(Asset::Move, character_asset.move_two);

                Move move_two;
                move_two.name = move_two_asset.name;
                move_two.speed = move_two_asset.speed;

                move_two.effect = move_two_asset.effect;
                move_two.value = move_two_asset.value;
                move_two.target = move_two_asset.target;

                move_two.description = move_two_asset.description;
                character.move_two = move_two;
            }

            if (character_asset.move_three != "None")
            {
                auto& move_three_asset = FLX_ASSET_GET(Asset::Move, character_asset.move_three);

                Move move_three;
                move_three.name = move_three_asset.name;
                move_three.speed = move_three_asset.speed;

                move_three.effect = move_three_asset.effect;
                move_three.value = move_three_asset.value;
                move_three.target = move_three_asset.target;

                move_three.description = move_three_asset.description;
                character.move_three = move_three;
            }

            character.current_slot = index;

            battle.drifters_and_enemies.push_back(&character);
            battle.speed_bar.push_back(&character);

            FlexECS::Entity character_sprite = FlexECS::Scene::GetActiveScene()->CreateEntity(character.name); // can always use GetEntityByName to find the entity
            character_sprite.AddComponent<Transform>({});
            character_sprite.AddComponent<Character>({});
            character_sprite.AddComponent<Enemy>({});

            character_sprite.AddComponent<Position>({ battle.sprite_slot_positions[character.current_slot + 2] + Vector3(-18, 25, 0) }
            ); // offset by 2 for enemy slots, and offset the position
            character_sprite.AddComponent<Rotation>({});
            character_sprite.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Question Mark.png)") });

            switch (character.character_id)
            {
            case 3:
                character_sprite.AddComponent<Scale>({ Vector3(2, 2, 0) });
                character_sprite.AddComponent<Animator>(
                  { FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_01_Idle_Anim_Sheet.flxspritesheet)"),
                    FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_01_Idle_Anim_Sheet.flxspritesheet)") }
                );
                break;
            case 4:
                character_sprite.AddComponent<Scale>({ Vector3(2, 2, 0) });
                character_sprite.AddComponent<Animator>(
                  { FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_02_Idle_Anim_Sheet.flxspritesheet)"),
                    FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_02_Idle_Anim_Sheet.flxspritesheet)") }
                );
                break;
            case 5:
                character_sprite.AddComponent<Scale>({ Vector3(2, 2, 0) });
                character_sprite.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Idle_Anim_Sheet.flxspritesheet)"),
                                           FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Idle_Anim_Sheet.flxspritesheet)") }
                );
                break;
            }
            character_sprite.AddComponent<ZIndex>({ 21 + index });

            FlexECS::Entity character_healthbar = FlexECS::Scene::GetActiveScene()->CreateEntity(character.name + " Healthbar"); // can always use GetEntityByName to find the entity
            character_healthbar.AddComponent<Healthbar>({});
            character_healthbar.AddComponent<Transform>({});
            character_healthbar.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot] });
            character_healthbar.AddComponent<Rotation>({});
            character_healthbar.AddComponent<Scale>({ Vector3(.1, .1, 0) });
            character_healthbar.GetComponent<Healthbar>()->original_position = character_healthbar.GetComponent<Position>()->position;
            character_healthbar.GetComponent<Healthbar>()->original_scale = character_healthbar.GetComponent<Scale>()->scale;
            character_healthbar.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_HealthBar_Green.png)") });
            character_healthbar.AddComponent<ZIndex>({ 21 + index });

            character_healthbar = FlexECS::Scene::GetActiveScene()->CreateEntity(character.name + " Stats"); // can always use GetEntityByName to find the entity
            character_healthbar.AddComponent<Transform>({});
            character_healthbar.AddComponent<Position>({ battle.sprite_slot_positions[character.current_slot] + Vector3(-30, -100, 0) });
            character_healthbar.AddComponent<Rotation>({});
            character_healthbar.AddComponent<Scale>({ Vector3(0.3f, 0.3f, 0) });
            character_healthbar.AddComponent<ZIndex>({ 21 + index });
            character_healthbar.AddComponent<Text>({
              FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
              FLX_STRING_NEW(R"(Lorem Ipsum)"),
              Vector3(1.0f, 1.0, 1.0f),
              { Renderer2DText::Alignment_Left, Renderer2DText::Alignment_Center },
              {                            600,                              320 }
            });

            FlexECS::Entity character_attack_buff = FlexECS::Scene::GetActiveScene()->CreateEntity(character.name + " Attack_Buff"); // can always use GetEntityByName to find the entity
            character_attack_buff.AddComponent<Transform>({});
            character_attack_buff.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot] + Vector3(-110, 0, 0) });
            character_attack_buff.AddComponent<Rotation>({});
            character_attack_buff.AddComponent<Scale>({ Vector3(.05, .05, 0) });
            character_attack_buff.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Attack_+1.png)") });
            character_attack_buff.AddComponent<ZIndex>({ 21 + index });

            FlexECS::Entity character_attack_debuff = FlexECS::Scene::GetActiveScene()->CreateEntity(character.name + " Attack_Debuff"); // can always use GetEntityByName to find the entity
            character_attack_debuff.AddComponent<Transform>({});
            character_attack_debuff.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot] + Vector3(-70, 0, 0) });
            character_attack_debuff.AddComponent<Rotation>({});
            character_attack_debuff.AddComponent<Scale>({ Vector3(.05, .05, 0) });
            character_attack_debuff.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Attack_-1.png)") });
            character_attack_debuff.AddComponent<ZIndex>({ 21 + index });

            FlexECS::Entity character_stun_debuff = FlexECS::Scene::GetActiveScene()->CreateEntity(character.name + " Stun_Debuff"); // can always use GetEntityByName to find the entity
            character_stun_debuff.AddComponent<Transform>({});
            character_stun_debuff.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot] + Vector3(-30, 0, 0) });
            character_stun_debuff.AddComponent<Rotation>({});
            character_stun_debuff.AddComponent<Scale>({ Vector3(.05, .05, 0) });
            character_stun_debuff.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Stun.png)") });
            character_stun_debuff.AddComponent<ZIndex>({ 21 + index });

            FlexECS::Entity character_shield_buff = FlexECS::Scene::GetActiveScene()->CreateEntity(character.name + " Shield_Buff"); // can always use GetEntityByName to find the entity
            character_shield_buff.AddComponent<Transform>({});
            character_shield_buff.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot] + Vector3(10, 0, 0) });
            character_shield_buff.AddComponent<Rotation>({});
            character_shield_buff.AddComponent<Scale>({ Vector3(.05, .05, 0) });
            character_shield_buff.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Def_+1.png)") });
            character_shield_buff.AddComponent<ZIndex>({ 21 + index });

            FlexECS::Entity character_protect_buff = FlexECS::Scene::GetActiveScene()->CreateEntity(character.name + " Protect_Buff"); // can always use GetEntityByName to find the entity
            character_protect_buff.AddComponent<Transform>({});
            character_protect_buff.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot] + Vector3(50, 0, 0) });
            character_protect_buff.AddComponent<Rotation>({});
            character_protect_buff.AddComponent<Scale>({ Vector3(.05, .05, 0) });
            character_protect_buff.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Heal_+1.png)") });
            character_protect_buff.AddComponent<ZIndex>({ 21 + index });

            index++;
        }

        int player_num = 0;
        int enemy_num = 0;
        // can't have empty drifter or enemy vector
        for (auto character : battle.drifters_and_enemies)
        {
            if (character->character_id <= 2)
            {
                player_num++;
            }
            else
            {
                enemy_num++;
            }
        }
        FLX_ASSERT(player_num > 0, "Drifter slots cannot be empty.");
        FLX_ASSERT(enemy_num > 0, "Enemy slots cannot be empty.");

        battle.projected_character = FlexECS::Scene::GetActiveScene()->CreateEntity("projected_char"); // can always use GetEntityByName to find the entity
        battle.projected_character.AddComponent<Transform>({});
        battle.projected_character.AddComponent<Position>({});
        battle.projected_character.AddComponent<Rotation>({});
        battle.projected_character.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Question Mark.png)") });
        battle.projected_character.AddComponent<Scale>({ Vector3(0.5, 0.5, 0) });
        battle.projected_character.AddComponent<ZIndex>({ 21 + index});
        battle.projected_character.GetComponent<Transform>()->is_active = false;
    }

    #pragma endregion
    void Start_Of_Game()
    {
        //CameraManager::SetMainGameCameraID(FlexECS::Scene::GetEntityByName("Camera"));

        File& file = File::Open(Path::current("assets/saves/battlescene_v3.flxscene"));
        FlexECS::Scene::SetActiveScene(FlexECS::Scene::Load(file));

        #pragma region Load Battle Data

        // load the battle
        Internal_ParseBattle2(R"(/data/debug.flxbattle)");

        Start_Of_Turn();
        battle.start_of_turn = true;
        battle.move_select = false;
        battle.move_resolution = false;
        battle.end_of_turn = false;

        battle.change_phase = true;
    }

    void Start_Of_Turn()
    {
        //Character current_character
        if (battle.change_phase)
        {
            battle.change_phase = false;

            //speed bar update
            std::sort(
          battle.speed_bar.begin(), battle.speed_bar.end(),
          [](const Character* a, const Character* b)
            {
                return a->current_speed < b->current_speed;
            }
            );

            // resolve the speed bar (just minus the speed by the value of the first character)
            int first_speed = battle.speed_bar[0]->current_speed;
            if (first_speed > 0)
                for (auto character : battle.speed_bar) character->current_speed -= first_speed;

            // update the character id in the slot based on the speed bar order
            for (FlexECS::Entity& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<SpeedBarSlot>())
            {
                auto& speed_bar_slot = *entity.GetComponent<SpeedBarSlot>();

                if (speed_bar_slot.slot_number <= battle.speed_bar.size())
                    speed_bar_slot.character = battle.speed_bar[speed_bar_slot.slot_number - 1]->character_id;
                else
                    speed_bar_slot.character = 0;
            }

            // update the sprite based on what character is in the slot
            for (FlexECS::Entity& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Sprite, SpeedBarSlot>())
            {
                auto& sprite = *entity.GetComponent<Sprite>();
                auto& speed_bar_slot = *entity.GetComponent<SpeedBarSlot>();

                // sprite handles
                static FlexECS::Scene::StringIndex _empty =
                    FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_SpeedBar_AllyProfile_Empty.png)");
                static FlexECS::Scene::StringIndex _renko =
                    FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_SpeedBar_AllyProfile_Renko.png)");
                static FlexECS::Scene::StringIndex _grace =
                    FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_SpeedBar_AllyProfile_Grace.png)");
                static FlexECS::Scene::StringIndex _enemy1 =
                    FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_SpeedBar_EnemyProfile_En1.png)");
                static FlexECS::Scene::StringIndex _enemy2 =
                    FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_SpeedBar_EnemyProfile_En2.png)");
                static FlexECS::Scene::StringIndex _jack =
                    FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_SpeedBar_EnemyProfile_Jack.png)");

                // get the character in the slot
                switch (speed_bar_slot.character)
                {
                case 0:
                    sprite.sprite_handle = _empty;
                    break;
                case 1:
                    sprite.sprite_handle = _renko;
                    break;
                case 2:
                    sprite.sprite_handle = _grace;
                    break;
                case 3:
                    sprite.sprite_handle = _enemy1;
                    break;
                case 4:
                    sprite.sprite_handle = _enemy2;
                    break;
                case 5:
                    sprite.sprite_handle = _jack;
                    break;
                }
            }

            /*
            //speed bar delay
            battle.disable_input_timer += 1.0f;

            // disable input if the timer is active
            while (battle.disable_input_timer > 0.f)
            {
                battle.disable_input_timer -= Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
            }
            */
            battle.current_character = battle.speed_bar.front();
            battle.current_move = nullptr;
            battle.initial_target = nullptr;
            battle.move_num = 0;
            battle.target_num = 0;

            FLX_ASSERT(battle.current_character != nullptr, "Current character is null.");
            FLX_ASSERT(battle.current_character->is_alive, "Current character is dead.");

            battle.is_player_turn = (battle.current_character->character_id <= 2);

            if (battle.is_player_turn)
            {// Plays sound if swap from enemy phase to player phase
                std::string audio_to_play = "/audio/" + battle.current_character->name + " start.mp3";
                FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file = FLX_STRING_NEW(audio_to_play);
                FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
            }
        }

        // disable input if the timer is active
        while (battle.disable_input_timer > 0.f)
        {
            battle.disable_input_timer -= Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
        }

        //reset position
        if (battle.previous_character != nullptr) {
            Vector3 original_position = (battle.previous_character->character_id <= 2) ?
                battle.sprite_slot_positions[battle.previous_character->current_slot] :
                battle.sprite_slot_positions[battle.previous_character->current_slot + 2];
            FlexECS::Scene::GetActiveScene()->GetEntityByName(battle.previous_character->name).GetComponent<Position>()->position = original_position;
        }

        Move_Select();
        battle.start_of_turn = false;
        battle.move_select = true;
        battle.move_resolution = false;
        battle.end_of_turn = false;

        battle.change_phase = true;
    }

    void Move_Select()
    {
        if (battle.change_phase)
        {
            battle.change_phase = false;

            if (battle.current_character->stun_debuff_duration > 0)
            {
                battle.current_character->stun_debuff_duration -= 1;

                if (battle.current_character->attack_buff_duration > 0)
                    battle.current_character->attack_buff_duration -= 1;

                if (battle.current_character->attack_debuff_duration > 0)
                    battle.current_character->attack_debuff_duration -= 1;

                if (battle.current_character->shield_buff_duration > 0)
                    battle.current_character->shield_buff_duration -= 1;

                if (battle.current_character->protect_buff_duration > 0)
                    battle.current_character->protect_buff_duration -= 1;

                battle.current_character->current_speed += battle.current_character->speed + 10;

                End_Of_Turn();
                battle.start_of_turn = false;
                battle.move_select = false;
                battle.move_resolution = false;
                battle.end_of_turn = true;

                battle.change_phase = true;
                return;
            }
            else
            {

                if (battle.is_player_turn)
                {
                    for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, MoveUI>())
                        entity.GetComponent<Transform>()->is_active = true;

                    battle.move_num = 1;
                    battle.current_move = &battle.current_character->move_one;

                    for (auto character : battle.drifters_and_enemies)
                    {
                        if (battle.current_move->target[0] == "ALL_ALLIES" || battle.current_move->target[0] == "NEXT_ALLY" || battle.current_move->target[0] == "SINGLE_ALLY" || battle.current_move->target[0] == "SELF")
                        {
                            if (character->is_alive && character->character_id <= 2)
                            {
                                battle.initial_target = character;
                                battle.target_num = character->current_slot;
                                break;
                            }
                        }
                        else
                        {
                            if (character->is_alive && character->character_id > 2)
                            {
                                battle.initial_target = character;
                                battle.target_num = character->current_slot;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    battle.move_num = Range<int>(1, 3).Get();
                    switch (battle.move_num)
                    {
                    case 1:
                        battle.current_move = &battle.current_character->move_one;
                        break;
                    case 2:
                        battle.current_move = &battle.current_character->move_two;
                        break;
                    case 3:
                        battle.current_move = &battle.current_character->move_three;
                        break;
                    }

                    while (battle.initial_target == nullptr)
                    {
                        if (battle.current_move->target[0] == "ALL_ALLIES" || battle.current_move->target[0] == "NEXT_ALLY" || battle.current_move->target[0] == "SINGLE_ALLY" || battle.current_move->target[0] == "SELF")
                        {
                            battle.target_num = Range<int>(0, 4).Get();
                            for (auto character : battle.drifters_and_enemies)
                            {
                                if (character->is_alive && character->current_slot == battle.target_num && character->character_id > 2)
                                {
                                    battle.initial_target = character;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            battle.move_num = Range<int>(0, 1).Get();
                            for (auto character : battle.drifters_and_enemies)
                            {
                                if (character->is_alive && character->current_slot == battle.target_num && character->character_id <= 2)
                                {
                                    battle.initial_target = character;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        //enable move if player turn
        if (battle.is_player_turn)
        {
            if (Input::GetKeyDown(GLFW_KEY_W))
            {
                battle.move_num--;
                if (battle.move_num < 1)
                {
                    battle.move_num = 3;
                }

                switch (battle.move_num)
                {
                case 1:
                    battle.current_move = &battle.current_character->move_one;
                    break;
                case 2:
                    battle.current_move = &battle.current_character->move_two;
                    break;
                case 3:
                    battle.current_move = &battle.current_character->move_three;
                    break;
                }

                for (auto character : battle.drifters_and_enemies)
                {
                    if (battle.current_move->target[0] == "ALL_ALLIES" || battle.current_move->target[0] == "NEXT_ALLY" || battle.current_move->target[0] == "SINGLE_ALLY" || battle.current_move->target[0] == "SELF")
                    {
                        if (character->is_alive && character->character_id <= 2)
                        {
                            battle.initial_target = character;
                            battle.target_num = character->current_slot;
                            break;
                        }
                    }
                    else
                    {
                        if (character->is_alive && character->character_id > 2)
                        {
                            battle.initial_target = character;
                            battle.target_num = character->current_slot;
                            break;
                        }
                    }
                }
            }
            else if (Input::GetKeyDown(GLFW_KEY_S))
            {
                battle.move_num++;
                if (battle.move_num > 3)
                {
                    battle.move_num = 1;
                }

                switch (battle.move_num)
                {
                case 1:
                    battle.current_move = &battle.current_character->move_one;
                    break;
                case 2:
                    battle.current_move = &battle.current_character->move_two;
                    break;
                case 3:
                    battle.current_move = &battle.current_character->move_three;
                    break;
                }

                for (auto character : battle.drifters_and_enemies)
                {
                    if (battle.current_move->target[0] == "ALL_ALLIES" || battle.current_move->target[0] == "NEXT_ALLY" || battle.current_move->target[0] == "SINGLE_ALLY" || battle.current_move->target[0] == "SELF")
                    {
                        if (character->is_alive && character->character_id <= 2)
                        {
                            battle.initial_target = character;
                            battle.target_num = character->current_slot;
                            break;
                        }
                    }
                    else
                    {
                        if (character->is_alive && character->character_id > 2)
                        {
                            battle.initial_target = character;
                            battle.target_num = character->current_slot;
                            break;
                        }
                    }
                }
            }

            if (Input::GetKeyDown(GLFW_KEY_A))
            {
                battle.initial_target = nullptr;
                while (battle.initial_target == nullptr || !battle.initial_target->is_alive)
                {
                    battle.target_num--;
                    for (auto character : battle.drifters_and_enemies)
                    {
                        if (battle.current_move->target[0] == "ALL_ALLIES" || battle.current_move->target[0] == "NEXT_ALLY" || battle.current_move->target[0] == "SINGLE_ALLY" || battle.current_move->target[0] == "SELF")
                        {
                            if (battle.target_num < 0)
                            {
                                battle.target_num = 1;
                            }

                            if (character->is_alive && character->current_slot == battle.target_num && character->character_id <= 2)
                            {
                                battle.initial_target = character;
                                break;
                            }
                        }
                        else
                        {
                            if (battle.target_num < 0)
                            {
                                battle.target_num = 4;
                            }

                            if (character->is_alive && character->current_slot == battle.target_num && character->character_id > 2)
                            {
                                battle.initial_target = character;
                                break;
                            }
                        }
                    }
                }
            }
            else if (Input::GetKeyDown(GLFW_KEY_D))
            {
                battle.initial_target = nullptr;
                while (battle.initial_target == nullptr)
                {
                    battle.target_num++;
                    for (auto character : battle.drifters_and_enemies)
                    {
                        if (battle.current_move->target[0] == "ALL_ALLIES" || battle.current_move->target[0] == "NEXT_ALLY" || battle.current_move->target[0] == "SINGLE_ALLY" || battle.current_move->target[0] == "SELF")
                        {
                            if (battle.target_num > 1)
                            {
                                battle.target_num = 0;
                            }

                            if (character->is_alive && character->current_slot == battle.target_num && character->character_id <= 2)
                            {
                                battle.initial_target = character;
                                break;
                            }
                        }
                        else
                        {
                            if (battle.target_num > 4)
                            {
                                battle.target_num = 0;
                            }

                            if (character->is_alive && character->current_slot == battle.target_num && character->character_id > 2)
                            {
                                battle.initial_target = character;
                                break;
                            }
                        }
                    }
                }

                //button sprite
                FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 1").GetComponent<Sprite>()->sprite_handle) =
                    (battle.move_num == 1) ? ("/images/battle ui/Battle_UI_Skill_Selected.png")
                    : ("/images/battle ui/Battle_UI_Skill_Unselected.png");
                FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 2").GetComponent<Sprite>()->sprite_handle) =
                    (battle.move_num == 2) ? ("/images/battle ui/Battle_UI_Skill_Selected.png")
                    : ("/images/battle ui/Battle_UI_Skill_Unselected.png");
                FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 3 Text").GetComponent<Sprite>()->sprite_handle) =
                    (battle.move_num == 3) ? ("/images/battle ui/Battle_UI_Skill_Selected.png")
                    : ("/images/battle ui/Battle_UI_Skill_Unselected.png");

                //button text
                FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 1 Text").GetComponent<Text>()->text) =
                    battle.current_character->move_one.name;
                FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 2 Text").GetComponent<Text>()->text) =
                    battle.current_character->move_two.name;
                FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 3 Text").GetComponent<Text>()->text) =
                    battle.current_character->move_three.name;

                //description text
                switch (battle.move_num)
                {
                case 1:
                    FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move Description Text").GetComponent<Text>()->text) =
                        battle.current_character->move_one.description;
                    break;
                case 2:
                    FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move Description Text").GetComponent<Text>()->text) =
                        battle.current_character->move_two.description;
                    break;
                case 3:
                    FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move Description Text").GetComponent<Text>()->text) =
                        battle.current_character->move_three.description;
                    break;
                }

                //target UI
                for (FlexECS::Entity& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, CharacterSlot>())
                {
                    auto& transform = *entity.GetComponent<Transform>();
                    auto& character_slot = *entity.GetComponent<CharacterSlot>();

                    // +2 to offset the slot number to the enemy slots
                    // only target slots that have a character in them
                    if (battle.initial_target != nullptr && battle.current_move != nullptr)
                    {
                        if (battle.current_move->target[0] == "ALL_ALLIES" || battle.current_move->target[0] == "NEXT_ALLY" || battle.current_move->target[0] == "SINGLE_ALLY" || battle.current_move->target[0] == "SELF")
                        {
                            transform.is_active = (character_slot.slot_number == (battle.target_num + 1));
                        }
                        else transform.is_active = (character_slot.slot_number == (battle.target_num + 3));
                    }
                    else
                        transform.is_active = false;
                }

                //projected character UI
                int projected_speed = battle.current_character->speed + battle.current_move->speed;
                int slot_number = -1; //will always be bigger than first element (itself), account for +1 for slot 0.

                if (projected_speed > 0)
                {
                    battle.projected_character.GetComponent<Transform>()->is_active = true;
                    for (auto character : battle.speed_bar)
                    {
                        if (projected_speed < character->current_speed)
                        {
                            //if smaller than slot 1, -1 + 1 = 0 (always bigger than slot 0, aka itself, it will be displayed on slot 0 + offset to the right, between slot 0 and slot 1
                            break;
                        }
                        else
                        {
                            slot_number++;
                        }
                    }

                    bool checkFirst = true; //grabs current character's icon, slot 0
                    for (FlexECS::Entity& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Sprite, SpeedBarSlot>())
                    {
                        if (checkFirst)
                        {
                            battle.projected_character.GetComponent<Sprite>()->sprite_handle = entity.GetComponent<Sprite>()->sprite_handle;
                            checkFirst = false;
                        }
                        if (slot_number == 0)
                        {
                            battle.projected_character.GetComponent<Position>()->position = entity.GetComponent<Position>()->position + Vector3{ 65, -65, 0 };
                            break;
                        }
                        else
                        {
                            slot_number--; //counts backwards to the slot it's supposed to be
                        }
                    }
                }
            }
        }

        if (!battle.is_player_turn || battle.is_player_turn && Input::GetKeyDown(GLFW_KEY_SPACE) && battle.initial_target != nullptr)
        {
            if (battle.current_character->attack_buff_duration > 0)
                battle.current_character->attack_buff_duration -= 1;

            if (battle.current_character->attack_debuff_duration > 0)
                battle.current_character->attack_debuff_duration -= 1;

            if (battle.current_character->shield_buff_duration > 0)
                battle.current_character->shield_buff_duration -= 1;

            if (battle.current_character->protect_buff_duration > 0)
                battle.current_character->protect_buff_duration -= 1;

            Move_Resolution();
            battle.start_of_turn = false;
            battle.move_select = false;
            battle.move_resolution = true;
            battle.end_of_turn = false;

            battle.change_phase = true;
        }
    }

    void Move_Resolution()
    {
        //Character current_character
        if (battle.change_phase)
        {
            battle.change_phase = false;

            if (battle.is_player_turn)
            {
                for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, MoveUI>())
                    entity.GetComponent<Transform>()->is_active = false;
            }
            FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move Description Text").GetComponent<Text>()->text) = "";
            FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 1 Text").GetComponent<Text>()->text) = "";
            FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 2 Text").GetComponent<Text>()->text) = "";
            FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 3 Text").GetComponent<Text>()->text) = "";

            battle.projected_character.GetComponent<Transform>()->is_active = false;
        }

        if (battle.is_player_turn)
        {
            // apply the move
            std::vector<Character*> targets;
            for (int i = 0; i < battle.current_move->effect.size(); i++) {
                if (battle.current_move->target[i] == "ALL_ENEMIES")
                {
                    for (auto character : battle.drifters_and_enemies)
                    {
                        if (character->is_alive && character->character_id > 2)
                        {
                            targets.push_back(character);
                        }
                    }
                }
                else if (battle.current_move->target[i] == "ADJACENT_ENEMIES")
                {
                    for (auto character : battle.drifters_and_enemies)
                    {
                        if (character->is_alive && (character->current_slot == battle.target_num - 1 && character->character_id > 2 || character->current_slot == battle.target_num && character->character_id > 2 || character->current_slot == battle.target_num + 1 && character->character_id > 2))
                        {
                            targets.push_back(character);
                        }
                    }
                }
                else if (battle.current_move->target[i] == "NEXT_ENEMY")
                {
                    for (auto character : battle.drifters_and_enemies)
                    {
                        if (character->is_alive && character->character_id > 2)
                        {
                            targets.push_back(character);
                            break;
                        }
                    }
                }
                else if (battle.current_move->target[i] == "SINGLE_ENEMY")
                {
                    for (auto character : battle.drifters_and_enemies)
                    {
                        if (character->is_alive && character->current_slot == battle.target_num && character->character_id > 2)
                        {
                            targets.push_back(character);
                        }
                    }
                }
                else if (battle.current_move->target[i] == "ALL_ALLIES")
                {
                    for (auto character : battle.drifters_and_enemies)
                    {
                        if (character->is_alive && character->character_id <= 2)
                        {
                            targets.push_back(character);
                        }
                    }
                }
                else if (battle.current_move->target[i] == "NEXT_ALLY")
                {
                    for (auto character : battle.drifters_and_enemies)
                    {
                        if (character->is_alive && character->character_id <= 2 && character != battle.current_character)
                        {
                            targets.push_back(character);
                            break;
                        }
                    }
                }
                else if (battle.current_move->target[i] == "SINGLE_ALLY")
                {
                    if (battle.current_move->target[0] == "ALL_ALLIES" || battle.current_move->target[0] == "NEXT_ALLY" || battle.current_move->target[0] == "SINGLE_ALLY" || battle.current_move->target[0] == "SELF") //fizzles if not targeting ally
                    {
                        for (auto character : battle.drifters_and_enemies)
                        {
                            if (character->is_alive && character->current_slot == battle.target_num && character->character_id <= 2)
                            {
                                targets.push_back(character);
                            }
                        }
                    }
                    //else targets.push_back(*(battle.current_character)); //defaults to targeting self
                }
                else if (battle.current_move->target[i] == "ALL")
                {
                    for (auto character : battle.drifters_and_enemies)
                    {
                        if (character->is_alive)
                            targets.push_back(character);
                    }
                }
                else if (battle.current_move->target[i] == "SELF")
                {
                    targets.push_back(battle.current_character);
                }

                if (battle.current_move->effect[i] == "Damage")
                {
                    for (auto character : targets)
                    {
                        if (character->shield_buff_duration > 0)
                        {
                            continue;
                        }
                        else
                        {
                            int final_damage = battle.current_move->value[i];
                            if (battle.current_character->attack_buff_duration > 0)
                            {
                                final_damage += battle.current_move->value[i] / 2;
                            }
                            if (battle.current_character->attack_debuff_duration > 0)
                            {
                                final_damage -= battle.current_move->value[i] / 2;
                            }
                            character->current_health -= final_damage;
                            Log::Debug(std::to_string(character->current_health));
                        }
                    }
                }
                if (battle.current_move->effect[i] == "Heal")
                {
                    for (auto character : targets)
                    {
                        if (character->current_health + battle.current_move->value[i] > character->health)
                        {
                            character->current_health = character->health;
                        }
                        else character->current_health += battle.current_move->value[i];
                    }
                }
                else if (battle.current_move->effect[i] == "Speed_Up")
                {
                    for (auto character : targets)
                    {
                        if (character->current_speed - battle.current_move->value[i] < 0)
                        {
                            character->current_speed = 0;
                        }
                        else character->current_speed -= battle.current_move->value[i];
                    }
                }
                else if (battle.current_move->effect[i] == "Speed_Down")
                {
                    for (auto character : targets)
                    {
                        character->current_speed += battle.current_move->value[i];
                    }
                }
                else if (battle.current_move->effect[i] == "Attack_Up")
                {
                    for (auto character : targets)
                    {
                        character->attack_buff_duration += battle.current_move->value[i];
                    }
                }
                else if (battle.current_move->effect[i] == "Attack_Down")
                {
                    for (auto character : targets)
                    {
                        if (character->protect_buff_duration > 0) continue;
                        else character->attack_debuff_duration += battle.current_move->value[i];
                    }
                }
                else if (battle.current_move->effect[i] == "Stun")
                {
                    for (auto character : targets)
                    {
                        if (character->protect_buff_duration > 0) continue;
                        else character->stun_debuff_duration += battle.current_move->value[i];
                    }
                }
                else if (battle.current_move->effect[i] == "Shield")
                {
                    for (auto character : targets)
                    {
                        character->shield_buff_duration += battle.current_move->value[i];
                        Log::Warning(std::to_string(character->shield_buff_duration));
                    }
                }
                else if (battle.current_move->effect[i] == "Protect")
                {
                    for (auto character : targets)
                    {
                        character->protect_buff_duration += battle.current_move->value[i];
                    }
                }
                else if (battle.current_move->effect[i] == "Strip")
                {
                    for (auto character : targets)
                    {
                        character->attack_buff_duration = 0;
                        character->shield_buff_duration = 0;
                        character->protect_buff_duration = 0;
                    }
                }
                else if (battle.current_move->effect[i] == "Cleanse")
                {
                    for (auto character : targets)
                    {
                        character->attack_debuff_duration = 0;
                        character->stun_debuff_duration = 0;
                    }
                }
                targets.clear();
            }

            auto& current_character_animator = *FlexECS::Scene::GetActiveScene()->GetEntityByName(battle.current_character->name).GetComponent<Animator>();
            switch (battle.move_num)
            {
            case 1:
                switch (battle.current_character->character_id)
                {
                case 1:
                    current_character_animator.spritesheet_handle =
                        FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Attack_Anim_Sheet.flxspritesheet)");
                    FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                        FLX_STRING_NEW(R"(/audio/generic attack.mp3)");
                    FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                    break;
                case 2:
                    current_character_animator.spritesheet_handle =
                        FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Attack_Anim_Sheet.flxspritesheet)");
                    FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                        FLX_STRING_NEW(R"(/audio/generic attack.mp3)");
                    FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                    break;
                }
                break;
            case 2:
                switch (battle.current_character->character_id)
                {
                case 1:
                    current_character_animator.spritesheet_handle =
                        FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Attack_Anim_Sheet.flxspritesheet)");
                    FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                        FLX_STRING_NEW(R"(/audio/generic attack.mp3)");
                    FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                    break;
                case 2:
                    current_character_animator.spritesheet_handle =
                        FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Attack_Anim_Sheet.flxspritesheet)");
                    FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                        FLX_STRING_NEW(R"(/audio/generic attack.mp3)");
                    FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                    break;
                }
                break;
            case 3:
                switch (battle.current_character->character_id)
                {
                case 1:
                    current_character_animator.spritesheet_handle =
                        FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Ult_Anim_Sheet.flxspritesheet)");
                    FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                        FLX_STRING_NEW(R"(/audio/chrono gear activation (SCI-FI-POWER-UP_GEN-HDF-20770).wav)");
                    FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                    break;
                case 2:
                    current_character_animator.spritesheet_handle =
                        FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Ult_Anim_Sheet.flxspritesheet)");
                    FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                        FLX_STRING_NEW(R"(/audio/chrono gear activation (SCI-FI-POWER-UP_GEN-HDF-20770).wav)");
                    FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                    break;
                }
                break;
            }
            float animation_time =
                FLX_ASSET_GET(Asset::Spritesheet, FLX_STRING_GET(current_character_animator.spritesheet_handle))
                .total_frame_time;

            current_character_animator.should_play = true;
            current_character_animator.is_looping = false;
            current_character_animator.return_to_default = true;
            current_character_animator.frame_time = 0.f;
            current_character_animator.current_frame = 0;

            battle.disable_input_timer += animation_time + 1.f;

            while (battle.disable_input_timer > 0.f)
            {
                battle.disable_input_timer -= Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
            }
            battle.current_character->current_speed += battle.current_character->speed + battle.current_move->speed;
            // Temporarily move the character
            battle.previous_character = battle.current_character;
            FlexECS::Scene::GetActiveScene()->GetEntityByName(battle.current_character->name).GetComponent<Position>()->position = battle.sprite_slot_positions[battle.initial_target->current_slot + 2];
        }
        else
        {
            std::vector<Character*> targets;
            for (int i = 0; i < battle.current_move->effect.size(); i++) {
                if (battle.current_move->target[i] == "ALL_ENEMIES")
                {
                    for (auto character : battle.drifters_and_enemies)
                    {
                        if (character->is_alive && character->character_id <= 2)
                        {
                            targets.push_back(character);
                        }
                    }
                }
                else if (battle.current_move->target[i] == "ADJACENT_ENEMIES")
                {
                    for (auto character : battle.drifters_and_enemies)
                    {
                        if (character->is_alive && (character->current_slot == battle.target_num - 1 && character->character_id <= 2 || character->current_slot == battle.target_num && character->character_id <= 2 || character->current_slot == battle.target_num + 1 && character->character_id <= 2))
                        {
                            targets.push_back(character);
                        }
                    }
                }
                else if (battle.current_move->target[i] == "NEXT_ENEMY")
                {
                    for (auto character : battle.drifters_and_enemies)
                    {
                        if (character->is_alive && character->character_id <= 2)
                        {
                            targets.push_back(character);
                            break;
                        }
                    }
                }
                else if (battle.current_move->target[i] == "SINGLE_ENEMY")
                {
                    for (auto character : battle.drifters_and_enemies)
                    {
                        if (character->is_alive && character->current_slot == battle.target_num && character->character_id <= 2)
                        {
                            targets.push_back(character);
                        }
                    }
                }
                else if (battle.current_move->target[i] == "ALL_ALLIES")
                {
                    for (auto character : battle.drifters_and_enemies)
                    {
                        if (character->is_alive && character->character_id > 2)
                        {
                            targets.push_back(character);
                        }
                    }
                }
                else if (battle.current_move->target[i] == "NEXT_ALLY")
                {
                    for (auto character : battle.drifters_and_enemies)
                    {
                        if (character->is_alive && character->character_id > 2 && character != battle.current_character)
                        {
                            targets.push_back(character);
                            break;
                        }
                    }
                }
                else if (battle.current_move->target[i] == "SINGLE_ALLY")
                {
                    if (battle.current_move->target[0] == "ALL_ALLIES" || battle.current_move->target[0] == "NEXT_ALLY" || battle.current_move->target[0] == "SINGLE_ALLY" || battle.current_move->target[0] == "SELF") //fizzles if not targeting ally
                    {
                        for (auto character : battle.drifters_and_enemies)
                        {
                            if (character->is_alive && character->current_slot == battle.target_num && character->character_id > 2)
                            {
                                targets.push_back(character);
                            }
                        }
                    }
                    //else targets.push_back(*(battle.current_character)); //defaults to targeting self
                }
                else if (battle.current_move->target[i] == "ALL")
                {
                    for (auto character : battle.drifters_and_enemies)
                    {
                        if (character->is_alive)
                            targets.push_back(character);
                    }
                }
                else if (battle.current_move->target[i] == "SELF")
                {
                    targets.push_back(battle.current_character);
                }

                if (battle.current_move->effect[i] == "Damage")
                {
                    for (auto character : targets)
                    {
                        if (character->shield_buff_duration > 0)
                        {
                            continue;
                        }
                        else
                        {
                            int final_damage = battle.current_move->value[i];
                            if (battle.current_character->attack_buff_duration > 0)
                            {
                                final_damage += battle.current_move->value[i] / 2;
                            }
                            if (battle.current_character->attack_debuff_duration > 0)
                            {
                                final_damage -= battle.current_move->value[i] / 2;
                            }
                            character->current_health -= final_damage;
                        }
                    }
                }
                if (battle.current_move->effect[i] == "Heal")
                {
                    for (auto character : targets)
                    {
                        if (character->current_health + battle.current_move->value[i] > character->health)
                        {
                            character->current_health = character->health;
                        }
                        else character->current_health += battle.current_move->value[i];
                    }
                }
                else if (battle.current_move->effect[i] == "Speed_Up")
                {
                    for (auto character : targets)
                    {
                        if (character->current_speed - battle.current_move->value[i] < 0)
                        {
                            character->current_speed = 0;
                        }
                        else character->current_speed -= battle.current_move->value[i];
                    }
                }
                else if (battle.current_move->effect[i] == "Speed_Down")
                {
                    for (auto character : targets)
                    {
                        character->current_speed += battle.current_move->value[i];
                    }
                }
                else if (battle.current_move->effect[i] == "Attack_Up")
                {
                    for (auto character : targets)
                    {
                        character->attack_buff_duration += battle.current_move->value[i];
                    }
                }
                else if (battle.current_move->effect[i] == "Attack_Down")
                {
                    for (auto character : targets)
                    {
                        if (character->protect_buff_duration > 0) continue;
                        else character->attack_debuff_duration += battle.current_move->value[i];
                    }
                }
                else if (battle.current_move->effect[i] == "Stun")
                {
                    for (auto character : targets)
                    {
                        if (character->protect_buff_duration > 0) continue;
                        else character->stun_debuff_duration += battle.current_move->value[i];
                    }
                }
                else if (battle.current_move->effect[i] == "Shield")
                {
                    for (auto character : targets)
                    {
                        character->shield_buff_duration += battle.current_move->value[i];
                        Log::Warning(std::to_string(character->shield_buff_duration));
                    }
                }
                else if (battle.current_move->effect[i] == "Protect")
                {
                    for (auto character : targets)
                    {
                        character->protect_buff_duration += battle.current_move->value[i];
                    }
                }
                else if (battle.current_move->effect[i] == "Strip")
                {
                    for (auto character : targets)
                    {
                        character->attack_buff_duration = 0;
                        character->shield_buff_duration = 0;
                        character->protect_buff_duration = 0;
                    }
                }
                else if (battle.current_move->effect[i] == "Cleanse")
                {
                    for (auto character : targets)
                    {
                        character->attack_debuff_duration = 0;
                        character->stun_debuff_duration = 0;
                    }
                }
                targets.clear();
            }

            // play the animation
            auto& current_character_animator = *FlexECS::Scene::GetActiveScene()->GetEntityByName(battle.current_character->name).GetComponent<Animator>();
            switch (battle.current_character->character_id)
            {
            case 3:
                current_character_animator.spritesheet_handle =
                    FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_01_Attack_Anim_Sheet.flxspritesheet)");
                FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                    FLX_STRING_NEW(R"(/audio/robot shooting.mp3)");
                FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                break;
            case 4:
                current_character_animator.spritesheet_handle =
                    FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_02_Attack_Anim_Sheet.flxspritesheet)");
                FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                    FLX_STRING_NEW(R"(/audio/robot shooting.mp3)");
                FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                break;
            case 5:
                // current_character_animator.spritesheet_handle =
                //   FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Attack_Anim_Sheet.flxspritesheet)");
                // FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                // FLX_STRING_NEW(R"(/audio/jack attack (SCI-FI-IMPACT_GEN-HDF-20694).wav)");
                // FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                break;
            }
            float animation_time =
                FLX_ASSET_GET(Asset::Spritesheet, FLX_STRING_GET(current_character_animator.spritesheet_handle))
                .total_frame_time;

            current_character_animator.should_play = true;
            current_character_animator.is_looping = false;
            current_character_animator.return_to_default = true;
            current_character_animator.frame_time = 0.f;
            current_character_animator.current_frame = 0;

            battle.disable_input_timer += animation_time + 1.f;

            // disable input if the timer is active
            while (battle.disable_input_timer > 0.f)
            {
                battle.disable_input_timer -= Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
            }

            animation_time = 0.f;

            if (battle.current_move->target[0] == "ADJACENT ENEMIES" || battle.current_move->target[0] == "ALL ENEMIES")
            {
                for (auto character : battle.drifters_and_enemies)
                {
                    if (character->is_alive && character->character_id <= 2)
                    {
                        auto target_entity = FlexECS::Scene::GetActiveScene()->GetEntityByName(character->name);
                        auto& target_animator = *target_entity.GetComponent<Animator>();
                        switch (character->character_id)
                        {
                        case 1:
                            target_animator.spritesheet_handle =
                                FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Hurt_Anim_Sheet.flxspritesheet)");
                            break;
                        case 2:
                            target_animator.spritesheet_handle =
                                FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Hurt_Anim_Sheet.flxspritesheet)");
                            break;
                        }

                        // get the slowest animation time
                        if (FLX_ASSET_GET(Asset::Spritesheet, FLX_STRING_GET(target_animator.spritesheet_handle))
                            .total_frame_time > animation_time)
                        {
                            animation_time =
                                FLX_ASSET_GET(Asset::Spritesheet, FLX_STRING_GET(target_animator.spritesheet_handle))
                                .total_frame_time;
                        }

                        target_animator.should_play = true;
                        target_animator.is_looping = false;
                        target_animator.return_to_default = true;
                        target_animator.frame_time = 0.f;
                        target_animator.current_frame = 0;
                    }
                }
                battle.disable_input_timer += animation_time + 1.f;
            }
            else if (battle.current_move->target[0] == "SINGLE ENEMY" || battle.current_move->target[0] == "NEXT ENEMY")
            {
                auto target_entity = FlexECS::Scene::GetActiveScene()->GetEntityByName(battle.initial_target->name);
                auto& target_animator = *target_entity.GetComponent<Animator>();
                switch (battle.initial_target->character_id)
                {
                case 1:
                    target_animator.spritesheet_handle =
                        FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Hurt_Anim_Sheet.flxspritesheet)");
                    break;
                case 2:
                    target_animator.spritesheet_handle =
                        FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Hurt_Anim_Sheet.flxspritesheet)");
                    break;
                }
                animation_time =
                    FLX_ASSET_GET(Asset::Spritesheet, FLX_STRING_GET(target_animator.spritesheet_handle))
                    .total_frame_time;

                target_animator.should_play = true;
                target_animator.is_looping = false;
                target_animator.return_to_default = true;
                target_animator.frame_time = 0.f;
                target_animator.current_frame = 0;

                battle.disable_input_timer += animation_time + 1.f;
            }

            while (battle.disable_input_timer > 0.f)
            {
                battle.disable_input_timer -= Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
            }

            battle.current_character->current_speed += battle.current_character->speed + battle.current_move->speed;
            // Temporarily move the character
            battle.previous_character = battle.current_character;
            FlexECS::Scene::GetActiveScene()->GetEntityByName(battle.current_character->name).GetComponent<Position>()->position = battle.sprite_slot_positions[battle.initial_target->current_slot];
        }
        
    }

    void End_Of_Turn()
    {
        float animation_time = .0f;
        for (auto character : battle.drifters_and_enemies)
        {
            // check if any characters are dead
            if (character->is_alive && character->current_health <= 0)
            {
                // update state
                character->is_alive = false;
                character->current_health = 0;

                // remove from speed bar
                battle.speed_bar.erase(
                  std::remove(battle.speed_bar.begin(), battle.speed_bar.end(), character), battle.speed_bar.end()
                );

                // play death animation
                auto& target_animator = *FlexECS::Scene::GetActiveScene()->GetEntityByName(character->name).GetComponent<Animator>();
                switch (character->character_id)
                {
                case 1:
                    target_animator.spritesheet_handle =
                        FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Death_Anim_Sheet.flxspritesheet)");
                    break;
                case 2:
                    target_animator.spritesheet_handle =
                        FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Death_Anim_Sheet.flxspritesheet)");
                    break;
                case 3:
                    target_animator.spritesheet_handle =
                        FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_01_Death_Anim_Sheet.flxspritesheet)");
                    // FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                    //   FLX_STRING_NEW(R"(/audio/robot death.mp3)");
                    // FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                    break;
                case 4:
                    target_animator.spritesheet_handle =
                        FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_02_Death_Anim_Sheet.flxspritesheet)");
                    // FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                    //   FLX_STRING_NEW(R"(/audio/robot death.mp3)");
                    // FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                    break;
                case 5:
                    // goto jack death cutscene
                    break;
                }

                // get the slowest animation time
                if (FLX_ASSET_GET(Asset::Spritesheet, FLX_STRING_GET(target_animator.spritesheet_handle))
                    .total_frame_time > animation_time)
                {
                    animation_time =
                        FLX_ASSET_GET(Asset::Spritesheet, FLX_STRING_GET(target_animator.spritesheet_handle))
                        .total_frame_time;
                }

                target_animator.should_play = true;
                target_animator.is_looping = false;
                target_animator.return_to_default = true;
                target_animator.frame_time = 0.f;
                target_animator.current_frame = 0;
            }
        }
        battle.disable_input_timer += animation_time + 1.f;

        // disable input if the timer is active
        while (battle.disable_input_timer > 0.f)
        {
            battle.disable_input_timer -= Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
        }

        int player_count = 0;
        int enemy_count = 0;
        for (auto character : battle.drifters_and_enemies)
        {
            if (character->is_alive && character->character_id <= 2)
            {
                player_count++;
            }
            else
            {
                enemy_count++;
            }
        }

        if (!player_count)
        {
            Lose_Battle();
        }
        else if (!enemy_count)
        {
            Win_Battle();
        }
    }

    void Win_Battle()
    {
        battle.is_end = true;
        FLX_COMMAND_ADD_WINDOW_OVERLAY("Game", std::make_shared<WinLayer>());
    }

    void Lose_Battle()
    {
        battle.is_end = true;
        FLX_COMMAND_ADD_WINDOW_OVERLAY("Game", std::make_shared<LoseLayer>());
    }

    void TutorialLayer::OnAttach()
    {

    }

    void TutorialLayer::OnDetach()
    {
        // Make sure nothing carries over in the way of sound
        FMODWrapper::Core::ForceStop();
    }

    void TutorialLayer::Update()
    {
        std::string& current_fps = FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("FPS Display").GetComponent<Text>()->text);
        current_fps = "FPS: " + std::to_string(Application::GetCurrentWindow()->GetFramerateController().GetFPS());

        bool move_one_click = Application::MessagingSystem::Receive<bool>("MoveOne clicked");
        bool move_two_click = Application::MessagingSystem::Receive<bool>("MoveTwo clicked");
        bool move_three_click = Application::MessagingSystem::Receive<bool>("MoveThree clicked");

        bool target_one_click = Application::MessagingSystem::Receive<bool>("TargetOne clicked");
        bool target_two_click = Application::MessagingSystem::Receive<bool>("TargetTwo clicked");
        bool target_three_click = Application::MessagingSystem::Receive<bool>("TargetThree clicked");
        bool target_four_click = Application::MessagingSystem::Receive<bool>("TargetFour clicked");
        bool target_five_click = Application::MessagingSystem::Receive<bool>("TargetFive clicked");

        // check for escape key
        // this goes back to the main menu
        if (Input::GetKeyDown(GLFW_KEY_ESCAPE))
        {
            // set the main camera
            //CameraManager::SetMainGameCameraID(*CameraManager::GetMainGameCamera());

            // unload win layer
            auto win_layer = Application::GetCurrentWindow()->GetLayerStack().GetOverlay("Win Layer");
            if (win_layer != nullptr) FLX_COMMAND_REMOVE_WINDOW_OVERLAY("Game", win_layer);

            // unload lose layer
            auto lose_layer = Application::GetCurrentWindow()->GetLayerStack().GetOverlay("Lose Layer");
            if (lose_layer != nullptr) FLX_COMMAND_REMOVE_WINDOW_OVERLAY("Game", lose_layer);
        }

        // return if the battle is over
        if (battle.is_end) return;

        // insta win
        if (Input::GetKeyDown(GLFW_KEY_X))
            Win_Battle();

        #pragma region UI Display

        // update the healthbar display
        // loop through each healthbar and update the scale based on the current health
        // there is the healthbarslot, the actual healthbar entity, and the character entity that are all needed
        for (auto character : battle.drifters_and_enemies)
        {
            auto entity = FlexECS::Scene::GetActiveScene()->GetEntityByName(character->name + " Healthbar");

            // guard
            if (!entity && !entity.HasComponent<Scale>() && !entity.HasComponent<Healthbar>()) continue;

            auto* scale = entity.GetComponent<Scale>();
            auto* healthbar = entity.GetComponent<Healthbar>();
            auto* position = entity.GetComponent<Position>();

            // Calculate the health percentage and new scale.
            float health_percentage = static_cast<float>(character->current_health) / static_cast<float>(character->health);
            // Update Scale
            scale->scale.x = healthbar->original_scale.x * health_percentage;

            // Update Position
            position->position.x = healthbar->original_position.x - static_cast<float>(healthbar->pixelLength / 2) * (1.0 - health_percentage);

            entity = FlexECS::Scene::GetActiveScene()->GetEntityByName(character->name + " Stats");

            // guard
            if (!entity && !entity.HasComponent<Text>()) continue;

            // get the character's current health
            std::string stats = "HP: " + std::to_string(character->current_health) + " / " + std::to_string(character->health) + " , " + "SPD: " + std::to_string(character->current_speed);
            entity.GetComponent<Text>()->text = FLX_STRING_NEW(stats);


            entity = FlexECS::Scene::GetActiveScene()->GetEntityByName(character->name + " Attack_Buff");

            // guard
            if (!entity) continue;

            // get the character's current health
            if (character->attack_buff_duration > 0)
                entity.GetComponent<Transform>()->is_active = true;
            else entity.GetComponent<Transform>()->is_active = false;


            entity = FlexECS::Scene::GetActiveScene()->GetEntityByName(character->name + " Attack_Debuff");

            // guard
            if (!entity) continue;

            // get the character's current health
            if (character->attack_debuff_duration > 0)
                entity.GetComponent<Transform>()->is_active = true;
            else entity.GetComponent<Transform>()->is_active = false;


            entity = FlexECS::Scene::GetActiveScene()->GetEntityByName(character->name + " Stun_Debuff");

            // guard
            if (!entity) continue;

            // get the character's current health
            if (character->stun_debuff_duration > 0)
                entity.GetComponent<Transform>()->is_active = true;
            else entity.GetComponent<Transform>()->is_active = false;


            entity = FlexECS::Scene::GetActiveScene()->GetEntityByName(character->name + " Shield_Buff");

            // guard
            if (!entity) continue;

            // get the character's current health
            if (character->shield_buff_duration > 0)
                entity.GetComponent<Transform>()->is_active = true;
            else entity.GetComponent<Transform>()->is_active = false;


            entity = FlexECS::Scene::GetActiveScene()->GetEntityByName(character->name + " Protect_Buff");

            // guard
            if (!entity) continue;

            // get the character's current health
            if (character->protect_buff_duration > 0)
                entity.GetComponent<Transform>()->is_active = true;
            else entity.GetComponent<Transform>()->is_active = false;
        }


        #pragma endregion
    }

} // namespace Game
#endif