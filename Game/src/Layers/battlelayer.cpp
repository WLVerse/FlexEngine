// WLVERSE [https://wlverse.web.app]
// battlelayer.cpp
//
// _Battle layer for the game.
//
// AUTHORS
// [70%] Ng Jia Cheng (n.jiacheng\@digipen.edu)
//   - Main Author
// Copyright (c) 2025 DigiPen, All rights reserved.

#include "Layers.h"
#include "battlestuff.h"
namespace Game
{
    extern std::string file_name;

    struct _Move
    {
        std::string name = "";
        int speed = 0;

        //For reference, effects are: Damage Heal Speed_Up Speed_Down Attack_Up Attack_Down Stun Shield Protect Strip Cleanse
        std::vector<std::string> effect;
        std::vector<int> value;
        std::vector<std::string> target;

        std::string description = "";
    };

    struct _Character
    {
        std::string name = "";

        int character_id = 0; // 1-5, determines the animations and sprites which are hardcoded
        int health = 0;
        int speed = 0;

        _Move move_one = {};
        _Move move_two = {};
        _Move move_three = {};

        int attack_debuff_duration = 0;
        int attack_buff_duration = 0;
        int stun_debuff_duration = 0;
        int shield_buff_duration = 0;
        int protect_buff_duration = 0;

        int current_health = 0;
        int current_speed = 0;
        int current_slot = 0;          // 0-4, 0-1 for drifters, 0-4 for enemies

        int previous_health = 0;
        int speed_change = 0;
        bool is_alive = true;
    };

    struct _Battle
    {
        //vector will store information of all characters. upon death, change status of is_alive to false and do not remove from vector. used to manage all combat related stuff.
        std::vector<_Character> drifters_and_enemies = {};
        //vector will store pointer to the information of all characters. upon death, remove from vector. used to manage speed bar icons.
        std::vector<_Character*> speed_bar = {};

        //icon to show where the character will end up on the speed bar after using a move
        FlexECS::Entity projected_character;
        FlexECS::Entity projected_character_text;
        FlexECS::Entity curr_char_highlight;

        FlexECS::Entity projected_move;
        FlexECS::Entity projected_move_text;

        //vector 3 positions to move things around
        std::array<Vector3, 7> sprite_slot_positions = {};
        //std::array<Vector3, 7> healthbar_slot_positions = {};
        std::array<FlexECS::Entity, 7> speed_slot_icons = {};
        std::array<Vector3, 7> speed_slot_position = {};

        // used during move selection and resolution
        _Character* current_character = nullptr;
        _Move* current_move = nullptr;
        _Character* initial_target = nullptr; //used to resolve whether the move will target allies or enemies by default and other stuff
        int move_num = 0; //1-3, which move is being hovered
        int target_num = 0; // 0-4, pointing out which enemy slot is the target
        int curr_char_pos_after_taking_turn = 0;

        // game state
        bool is_player_turn = false;
        float disable_input_timer = 0.f;
        float anim_timer = 0.f;

        bool is_paused = false;
        //bool is_end = false;
        bool is_win = false;
        bool is_lose = false;

        bool god_mode = false;

        int battle_num = 0;
        int tutorial_info = 0;
        bool is_tutorial = true;
        bool is_tutorial_running = false;
        FlexECS::Entity tutorial_text;

        bool start_of_turn = false;
        bool play_battle_start = true;
        bool move_select = false;
        bool move_resolution = false;
        bool speedbar_animating = false;
        bool end_of_turn = false;
        bool enable_combat_camera = false;
        bool force_disable_combat_camera = false;
        bool jerk_towards_defender = false;
        bool change_phase = false;

        // Pause Buttons
        std::string active_pause_button;
        std::string active_volume_button;

        // Camera Original Ortho dimensions
        float m_originalWidth;
        float m_originalHeight;
    };

    struct _SpriteHandles
    {
      FlexECS::Scene::StringIndex empty;
      FlexECS::Scene::StringIndex renko;
      FlexECS::Scene::StringIndex grace;
      FlexECS::Scene::StringIndex enemy1;
      FlexECS::Scene::StringIndex enemy2;
      FlexECS::Scene::StringIndex jack;
    };

    struct _VFXPresets
    {
      std::string vfx_player_attack  = "/images/vfx/Impact_FX_Player_64.flxspritesheet";
      std::string vfx_enemy_attack1  = "/images/vfx/Impact_FX_En1_48.flxspritesheet";
      std::string vfx_enemy_attack2  = "/images/vfx/Impact_FX_En2_48.flxspritesheet";

      std::string vfx_grace_ult      = "/images/vfx/VFX_Grace_Ult_Anim_Sheet.flxspritesheet";
      std::string vfx_jack_ult       = "/images/vfx/VFX_Jack_Ult_Sheet.flxspritesheet";
    } VFXPresets;

    _Battle battle;
    _SpriteHandles sprite_handles;
    void ClearBattleStruct();
    void Start_Of_Turn();
    void Move_Select();
    void Move_Resolution();
    void End_Of_Turn();
    void Win_Battle();
    void Lose_Battle();
    void Update_Character_Status();
    void Update_Damage_And_Targetting_Previews();

    void ClearBattleStruct()
    {
      //Battle struct is global, so the variables persist through layer swapping
      battle.drifters_and_enemies.clear();
      battle.speed_bar.clear();

      //icon to show where the character will end up on the speed bar after using a move
      battle.projected_character = FlexECS::Entity::Null;
      battle.projected_character_text = FlexECS::Entity::Null;

      FlexECS::Entity projected_move;
      FlexECS::Entity projected_move_text;

      //vector 3 positions to move things around
      battle.sprite_slot_positions = {};
      //battle.healthbar_slot_positions = {};
      battle.speed_slot_icons = {};
      battle.speed_slot_position = {};

      battle.current_character = nullptr;
      battle.current_move = nullptr;
      battle.initial_target = nullptr; //used to resolve whether the move will target allies or enemies by default and other stuff
      battle.move_num = 0; //1-3, which move is being hovered
      battle.target_num = 0; // 0-4, pointing out which enemy slot is the target
      battle.curr_char_pos_after_taking_turn = 0;

      battle.is_player_turn = false;
      battle.disable_input_timer = 0.f;
      battle.anim_timer = 0.f;

      battle.is_paused = false;
      battle.is_win = false;
      battle.is_lose = false;

      battle.god_mode = false;

      battle.battle_num = 0;
      battle.tutorial_info = 0;
      battle.is_tutorial = true;
      battle.is_tutorial_running = false;
      battle.tutorial_text;

      battle.start_of_turn = false;
      battle.play_battle_start = true;
      battle.move_select = false;
      battle.move_resolution = false;
      battle.speedbar_animating = false;
      battle.end_of_turn = false;

      battle.change_phase = false;
    }
        
    static void ReplaceUnderscoresWithSpaces(std::string& str) 
    {
        // Find each underscore and replace it with a space
        size_t pos = 0;
        while ((pos = str.find('_', pos)) != std::string::npos) {
            str[pos] = ' ';  // Replace the underscore with a space
            pos++;  // Move to the next character after the replaced one
        }
    }

    void Internal_ParseBattle(AssetKey assetkey)
    {
        //get positions of character icons and health bar.
        for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<CharacterSlot>())
        {
            auto& character_slot = *entity.GetComponent<CharacterSlot>();
            battle.sprite_slot_positions[character_slot.slot_number - 1] = entity.GetComponent<Position>()->position;
        }
        /*for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<HealthbarSlot>())
        {
            auto& healthbar_slot = *entity.GetComponent<HealthbarSlot>();
            battle.healthbar_slot_positions[healthbar_slot.slot_number - 1] = entity.GetComponent<Position>()->position;
        }*/

        // get the battle asset
        auto& asset = FLX_ASSET_GET(Asset::Battle, assetkey);

        // Retrieve the current camera entity by name (assuming it's named "Camera")
        FlexECS::Entity currentCamera = FlexECS::Scene::GetActiveScene()->GetEntityByName("Camera");

        // Create a new entity for the UI Camera.
        FlexECS::Entity uiCamera = FlexECS::Scene::CreateEntity("UI Camera");
        uiCamera.AddComponent<Position>(*currentCamera.GetComponent<Position>());
        uiCamera.AddComponent<Scale>(*currentCamera.GetComponent<Scale>());
        uiCamera.AddComponent<Rotation>(*currentCamera.GetComponent<Rotation>());
        uiCamera.AddComponent<Transform>(*currentCamera.GetComponent<Transform>());
        uiCamera.AddComponent<Camera>(*currentCamera.GetComponent<Camera>());

        battle.battle_num = asset.battle_num;
        battle.is_tutorial = !asset.battle_num;
        // parse the battle data
        int index = 0;
        int temp_index = 0;
        for (auto& slot : asset.GetDrifterSlots())
        {
            _Character character;
            if (*slot == "None")
            {
                index++;

                FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index)).GetComponent<Transform>()->is_active = false;
                FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index) + " Name").GetComponent<Transform>()->is_active = false;
                FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index) + " Healthbar").GetComponent<Transform>()->is_active = false;
                FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index) + " Current Healthbar").GetComponent<Transform>()->is_active = false;
                FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index) + " Buff 1").GetComponent<Transform>()->is_active = false;
                FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index) + " Buff 2").GetComponent<Transform>()->is_active = false;
                FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index) + " Buff 3").GetComponent<Transform>()->is_active = false;
                FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index) + " Buff 4").GetComponent<Transform>()->is_active = false;
                FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index) + " Buff 5").GetComponent<Transform>()->is_active = false;

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

                _Move move_one;
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

                _Move move_two;
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

                _Move move_three;
                move_three.name = move_three_asset.name;
                move_three.speed = move_three_asset.speed;

                move_three.effect = move_three_asset.effect;
                move_three.value = move_three_asset.value;
                move_three.target = move_three_asset.target;

                move_three.description = move_three_asset.description;
                character.move_three = move_three;
            }

            character.current_slot = index;

            battle.drifters_and_enemies.push_back(character);
            /*FlexECS::Entity character_sprite = FlexECS::Scene::CreateEntity(character.name); // can always use GetEntityByName to find the entity
            character_sprite.AddComponent<Transform>({});
            //character_sprite.AddComponent<Character>({});

            // find the slot position
            character_sprite.AddComponent<Position>({ battle.sprite_slot_positions[character.current_slot] + Vector3(20, 120, 0) });
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

            FlexECS::Entity character_healthbar = FlexECS::Scene::CreateEntity(character.name + " Healthbar"); // can always use GetEntityByName to find the entity
            character_healthbar.AddComponent<Healthbar>({});
            character_healthbar.AddComponent<Transform>({});
            character_healthbar.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot] });
            character_healthbar.AddComponent<Rotation>({});
            character_healthbar.AddComponent<Scale>({ Vector3(.1f, .1f, 0) });
            character_healthbar.GetComponent<Healthbar>()->original_position = character_healthbar.GetComponent<Position>()->position;
            character_healthbar.GetComponent<Healthbar>()->original_scale = character_healthbar.GetComponent<Scale>()->scale;
            character_healthbar.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_HealthBar_Green.png)") });
            character_healthbar.AddComponent<ZIndex>({ 21 + index });

            character_healthbar = FlexECS::Scene::CreateEntity(character.name + " Stats"); // can always use GetEntityByName to find the entity
            character_healthbar.AddComponent<Transform>({});
            character_healthbar.AddComponent<Position>({ battle.sprite_slot_positions[character.current_slot] + Vector3(-30, -100, 0) });
            character_healthbar.AddComponent<Rotation>({});
            character_healthbar.AddComponent<Scale>({ Vector3(0.5f, 0.5f, 0) });
            character_healthbar.AddComponent<ZIndex>({ 21 + index });
            character_healthbar.AddComponent<Text>({
              FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
              FLX_STRING_NEW(R"(Lorem Ipsum)"),
              Vector3(1.0f, 1.0, 1.0f),
              { Renderer2DText::Alignment_Left, Renderer2DText::Alignment_Center },
              {                            600,                              320 }
            });

            FlexECS::Entity character_attack_buff = FlexECS::Scene::CreateEntity(character.name + " Attack_Buff"); // can always use GetEntityByName to find the entity
            character_attack_buff.AddComponent<Transform>({});
            character_attack_buff.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot] + Vector3(-110, -20, 0) });
            character_attack_buff.AddComponent<Rotation>({});
            character_attack_buff.AddComponent<Scale>({ Vector3(.05f, .05f, 0) });
            character_attack_buff.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Attack_+1.png)") });
            character_attack_buff.AddComponent<ZIndex>({ 21 + index });

            FlexECS::Entity character_attack_debuff = FlexECS::Scene::CreateEntity(character.name + " Attack_Debuff"); // can always use GetEntityByName to find the entity
            character_attack_debuff.AddComponent<Transform>({});
            character_attack_debuff.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot] + Vector3(-70, -20, 0) });
            character_attack_debuff.AddComponent<Rotation>({});
            character_attack_debuff.AddComponent<Scale>({ Vector3(.05f, .05f, 0) });
            character_attack_debuff.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Attack_-1.png)") });
            character_attack_debuff.AddComponent<ZIndex>({ 21 + index });

            FlexECS::Entity character_stun_debuff = FlexECS::Scene::CreateEntity(character.name + " Stun_Debuff"); // can always use GetEntityByName to find the entity
            character_stun_debuff.AddComponent<Transform>({});
            character_stun_debuff.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot] + Vector3(-30, -20, 0) });
            character_stun_debuff.AddComponent<Rotation>({});
            character_stun_debuff.AddComponent<Scale>({ Vector3(.05f, .05f, 0) });
            character_stun_debuff.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Stun.png)") });
            character_stun_debuff.AddComponent<ZIndex>({ 21 + index });

            FlexECS::Entity character_shield_buff = FlexECS::Scene::CreateEntity(character.name + " Shield_Buff"); // can always use GetEntityByName to find the entity
            character_shield_buff.AddComponent<Transform>({});
            character_shield_buff.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot] + Vector3(10,  -20, 0) });
            character_shield_buff.AddComponent<Rotation>({});
            character_shield_buff.AddComponent<Scale>({ Vector3(.05f, .05f, 0) });
            character_shield_buff.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Def_+1.png)") });
            character_shield_buff.AddComponent<ZIndex>({ 21 + index });

            FlexECS::Entity character_protect_buff = FlexECS::Scene::CreateEntity(character.name + " Protect_Buff"); // can always use GetEntityByName to find the entity
            character_protect_buff.AddComponent<Transform>({});
            character_protect_buff.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot] + Vector3(50, -20, 0) });
            character_protect_buff.AddComponent<Rotation>({});
            character_protect_buff.AddComponent<Scale>({ Vector3(.05f, .05f, 0) });
            character_protect_buff.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Heal_+1.png)") });
            character_protect_buff.AddComponent<ZIndex>({ 21 + index });
            */
            index++;
            temp_index++;


            FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index)).GetComponent<Transform>()->is_active = true;
            switch (character.character_id)
            {
            case 1:
                FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index)).GetComponent<Animator>()->default_spritesheet_handle =
                    FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Attack_Anim_Sheet.flxspritesheet)");
                FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index)).GetComponent<Animator>()->spritesheet_handle =
                    FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Attack_Anim_Sheet.flxspritesheet)");
                break;
            case 2:
                FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index)).GetComponent<Animator>()->default_spritesheet_handle =
                    FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)");
                FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index)).GetComponent<Animator>()->spritesheet_handle =
                    FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)");
                break;
            }

            FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index) + " Name").GetComponent<Transform>()->is_active = true;
            FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index) + " Name").GetComponent<Text>()->text = FLX_STRING_NEW(character.name);

            FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index) + " Healthbar").GetComponent<Transform>()->is_active = true;
            FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index) + " Current Healthbar").GetComponent<Transform>()->is_active = true;
            FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index) + " Current Healthbar").GetComponent<Healthbar>()->original_scale =
                FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index) + " Current Healthbar").GetComponent<Scale>()->scale;
            FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index) + " Current Healthbar").GetComponent<Healthbar>()->original_position = 
                FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index) + " Current Healthbar").GetComponent<Position>()->position;

            FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index) + " Buff 1").GetComponent<Transform>()->is_active = false;
            FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index) + " Buff 2").GetComponent<Transform>()->is_active = false;
            FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index) + " Buff 3").GetComponent<Transform>()->is_active = false;
            FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index) + " Buff 4").GetComponent<Transform>()->is_active = false;
            FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(index) + " Buff 5").GetComponent<Transform>()->is_active = false;

            FlexECS::Scene::GetEntityByName("Speed slot " + std::to_string(temp_index)).GetComponent<Transform>()->is_active = true;
            if (temp_index != 1)
            {
                FlexECS::Scene::GetEntityByName("Speedbar Accent " + std::to_string(temp_index - 1)).GetComponent<Transform>()->is_active = true;
            }
        }

        index = 0;
        for (auto& slot : asset.GetEnemySlots())
        {
            _Character character;
            if (*slot == "None")
            {
                index++;

                FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index)).GetComponent<Transform>()->is_active = false;
                FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index) + " Name").GetComponent<Transform>()->is_active = false;
                FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index) + " Healthbar").GetComponent<Transform>()->is_active = false;
                FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index) + " Current Healthbar").GetComponent<Transform>()->is_active = false;

                FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index) + " Buff 1").GetComponent<Transform>()->is_active = false;
                FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index) + " Buff 2").GetComponent<Transform>()->is_active = false;
                FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index) + " Buff 3").GetComponent<Transform>()->is_active = false;
                FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index) + " Buff 4").GetComponent<Transform>()->is_active = false;
                FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index) + " Buff 5").GetComponent<Transform>()->is_active = false;

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

                _Move move_one;
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

                _Move move_two;
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

                _Move move_three;
                move_three.name = move_three_asset.name;
                move_three.speed = move_three_asset.speed;

                move_three.effect = move_three_asset.effect;
                move_three.value = move_three_asset.value;
                move_three.target = move_three_asset.target;

                move_three.description = move_three_asset.description;
                character.move_three = move_three;
            }

            character.current_slot = index;

            battle.drifters_and_enemies.push_back(character);
            /*
            FlexECS::Entity character_sprite = FlexECS::Scene::CreateEntity(character.name); // can always use GetEntityByName to find the entity
            character_sprite.AddComponent<Transform>({});
            //character_sprite.AddComponent<Character>({});
            //character_sprite.AddComponent<Enemy>({});

            character_sprite.AddComponent<Position>({ battle.sprite_slot_positions[character.current_slot + 2] + Vector3(-18, 15, 0) }
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

            FlexECS::Entity character_healthbar = FlexECS::Scene::CreateEntity(character.name + " Healthbar"); // can always use GetEntityByName to find the entity
            character_healthbar.AddComponent<Healthbar>({});
            character_healthbar.AddComponent<Transform>({});
            character_healthbar.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot + 2] });
            character_healthbar.AddComponent<Rotation>({});
            character_healthbar.AddComponent<Scale>({ Vector3(.1f, .1f, 0) });
            character_healthbar.GetComponent<Healthbar>()->original_position = character_healthbar.GetComponent<Position>()->position;
            character_healthbar.GetComponent<Healthbar>()->original_scale = character_healthbar.GetComponent<Scale>()->scale;
            character_healthbar.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_HealthBar_Green.png)") });
            character_healthbar.AddComponent<ZIndex>({ 21 + index });

            FlexECS::Entity damage_preview = FlexECS::Scene::CreateEntity(character.name + " DamagePreview"); // can always use GetEntityByName to find the entity
            damage_preview.AddComponent<Healthbar>({});
            damage_preview.AddComponent<Transform>({});
            damage_preview.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot + 2] });
            damage_preview.AddComponent<Rotation>({});
            damage_preview.AddComponent<Scale>({ Vector3(.1f, .1f, 0) });
            damage_preview.GetComponent<Healthbar>()->original_position = damage_preview.GetComponent<Position>()->position;
            damage_preview.GetComponent<Healthbar>()->original_scale = damage_preview.GetComponent<Scale>()->scale;
            damage_preview.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_HealthBar_Rose.png)") });
            damage_preview.AddComponent<ZIndex>({ 21 + index + 1 });

            character_healthbar = FlexECS::Scene::CreateEntity(character.name + " Stats"); // can always use GetEntityByName to find the entity
            character_healthbar.AddComponent<Transform>({});
            character_healthbar.AddComponent<Position>({ battle.sprite_slot_positions[character.current_slot + 2] + Vector3(-70, -100, 0) });
            character_healthbar.AddComponent<Rotation>({});
            character_healthbar.AddComponent<Scale>({ Vector3(0.5f, 0.5f, 0) });
            character_healthbar.AddComponent<ZIndex>({ 21 + index });
            character_healthbar.AddComponent<Text>({
              FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
              FLX_STRING_NEW(R"(Lorem Ipsum)"),
              Vector3(1.0f, 1.0, 1.0f),
              { Renderer2DText::Alignment_Left, Renderer2DText::Alignment_Center },
              {                            600,                              320 }
            });

            FlexECS::Entity character_attack_buff = FlexECS::Scene::CreateEntity(character.name + " Attack_Buff"); // can always use GetEntityByName to find the entity
            character_attack_buff.AddComponent<Transform>({});
            character_attack_buff.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot + 2] + Vector3(-110, -20, 0) });
            character_attack_buff.AddComponent<Rotation>({});
            character_attack_buff.AddComponent<Scale>({ Vector3(.05f, .05f, 0) });
            character_attack_buff.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Attack_+1.png)") });
            character_attack_buff.AddComponent<ZIndex>({ 21 + index });

            FlexECS::Entity character_attack_debuff = FlexECS::Scene::CreateEntity(character.name + " Attack_Debuff"); // can always use GetEntityByName to find the entity
            character_attack_debuff.AddComponent<Transform>({});
            character_attack_debuff.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot + 2] + Vector3(-70, -20, 0) });
            character_attack_debuff.AddComponent<Rotation>({});
            character_attack_debuff.AddComponent<Scale>({ Vector3(.05f, .05f, 0) });
            character_attack_debuff.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Attack_-1.png)") });
            character_attack_debuff.AddComponent<ZIndex>({ 21 + index });

            FlexECS::Entity character_stun_debuff = FlexECS::Scene::CreateEntity(character.name + " Stun_Debuff"); // can always use GetEntityByName to find the entity
            character_stun_debuff.AddComponent<Transform>({});
            character_stun_debuff.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot + 2] + Vector3(-30, -20, 0) });
            character_stun_debuff.AddComponent<Rotation>({});
            character_stun_debuff.AddComponent<Scale>({ Vector3(.05f, .05f, 0) });
            character_stun_debuff.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Stun.png)") });
            character_stun_debuff.AddComponent<ZIndex>({ 21 + index });

            FlexECS::Entity character_shield_buff = FlexECS::Scene::CreateEntity(character.name + " Shield_Buff"); // can always use GetEntityByName to find the entity
            character_shield_buff.AddComponent<Transform>({});
            character_shield_buff.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot + 2] + Vector3(10, -20, 0) });
            character_shield_buff.AddComponent<Rotation>({});
            character_shield_buff.AddComponent<Scale>({ Vector3(.05f, .05f, 0) });
            character_shield_buff.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Def_+1.png)") });
            character_shield_buff.AddComponent<ZIndex>({ 21 + index });

            FlexECS::Entity character_protect_buff = FlexECS::Scene::CreateEntity(character.name + " Protect_Buff"); // can always use GetEntityByName to find the entity
            character_protect_buff.AddComponent<Transform>({});
            character_protect_buff.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot + 2] + Vector3(50, -20, 0) });
            character_protect_buff.AddComponent<Rotation>({});
            character_protect_buff.AddComponent<Scale>({ Vector3(.05f, .05f, 0) });
            character_protect_buff.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Heal_+1.png)") });
            character_protect_buff.AddComponent<ZIndex>({ 21 + index });
            */
            index++;
            temp_index++;
            FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index)).GetComponent<Transform>()->is_active = true;
            switch (character.character_id)
            {
            case 3:
                FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index)).GetComponent<Animator>()->default_spritesheet_handle =
                    FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_01_Idle_Anim_Sheet.flxspritesheet)");
                FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index)).GetComponent<Animator>()->spritesheet_handle =
                    FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_01_Idle_Anim_Sheet.flxspritesheet)");
                break;
            case 4:
                FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index)).GetComponent<Animator>()->default_spritesheet_handle =
                    FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_02_Idle_Anim_Sheet.flxspritesheet)");
                FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index)).GetComponent<Animator>()->spritesheet_handle =
                    FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_02_Idle_Anim_Sheet.flxspritesheet)");
                break;
            case 5:
                FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index)).GetComponent<Animator>()->default_spritesheet_handle =
                    FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Idle_Anim_Sheet.flxspritesheet)");
                FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index)).GetComponent<Animator>()->spritesheet_handle =
                    FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Idle_Anim_Sheet.flxspritesheet)");
                break;
            }

            FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index) + " Name").GetComponent<Transform>()->is_active = true;
            FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index) + " Name").GetComponent<Text>()->text = FLX_STRING_NEW(character.name);

            FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index) + " Healthbar").GetComponent<Transform>()->is_active = true;
            FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index) + " Current Healthbar").GetComponent<Transform>()->is_active = true;
            FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index) + " Current Healthbar").GetComponent<Healthbar>()->original_scale =
                FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index) + " Current Healthbar").GetComponent<Scale>()->scale;
            FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index) + " Current Healthbar").GetComponent<Healthbar>()->original_position =
                FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index) + " Current Healthbar").GetComponent<Position>()->position;

            FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index) + " Buff 1").GetComponent<Transform>()->is_active = false;
            FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index) + " Buff 2").GetComponent<Transform>()->is_active = false;
            FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index) + " Buff 3").GetComponent<Transform>()->is_active = false;
            FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index) + " Buff 4").GetComponent<Transform>()->is_active = false;
            FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(index) + " Buff 5").GetComponent<Transform>()->is_active = false;

            FlexECS::Scene::GetEntityByName("Speed slot " + std::to_string(temp_index)).GetComponent<Transform>()->is_active = true;
            if (temp_index != 1)
            {
                FlexECS::Scene::GetEntityByName("Speedbar Accent " + std::to_string(temp_index - 1)).GetComponent<Transform>()->is_active = true;
            }
        }

        int player_num = 0;
        int enemy_num = 0;
        // can't have empty drifter or enemy vector
        for (auto &character : battle.drifters_and_enemies)
        {
            battle.speed_bar.push_back(&character);
            if (character.character_id <= 2)
            {
                player_num++;
            }
            else
            {
                enemy_num++;
            }
            if (character.character_id == 5)
            {
                character.protect_buff_duration = 4;
            }
        }
        FLX_ASSERT(player_num > 0, "Drifter slots cannot be empty.");
        FLX_ASSERT(enemy_num > 0, "Enemy slots cannot be empty.");

        //icon to show where character is ending up on the turn bar after using move
        battle.projected_character = FlexECS::Scene::CreateEntity("projected_char"); // can always use GetEntityByName to find the entity
        battle.projected_character.AddComponent<Transform>({});
        battle.projected_character.AddComponent<Position>({});
        battle.projected_character.AddComponent<Rotation>({});
        battle.projected_character.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Question Mark.png)") });
        battle.projected_character.AddComponent<Scale>({ Vector3(0.5, 0.5, 0) });
        battle.projected_character.AddComponent<ZIndex>({ 1021 + index });
        battle.projected_character.GetComponent<Transform>()->is_active = false;

        // Text that just says next for the projected character lol
        battle.projected_character_text = FlexECS::Scene::CreateEntity("projected_char_text"); 
        battle.projected_character_text.AddComponent<Transform>({});
        battle.projected_character_text.AddComponent<Position>({});
        battle.projected_character_text.AddComponent<Rotation>({});
        battle.projected_character_text.AddComponent<Scale>({ Vector3(0.5, 0.5, 0) });
        battle.projected_character_text.AddComponent<Text>({FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
                                                                 FLX_STRING_NEW(R"(NEXT)"),
                                                                 Vector3(1.0f, 1.0, 1.0f),
                                                                 { Renderer2DText::Alignment_Left, Renderer2DText::Alignment_Center },
                                                                 {                            600,                              320 }
                                                                 });
        battle.projected_character_text.AddComponent<ZIndex>({ 1021 + index });
        battle.projected_character_text.GetComponent<Transform>()->is_active = false;

        battle.projected_move = FlexECS::Scene::CreateEntity("move_used_textbox"); // can always use GetEntityByName to find the entity
        battle.projected_move.AddComponent<Transform>({});
        battle.projected_move.AddComponent<Position>({ Vector3(1595, 515, 0) });
        battle.projected_move.AddComponent<Rotation>({});
        battle.projected_move.AddComponent<Scale>({ Vector3(0.2f, 0.3f, 0) });
        battle.projected_move.AddComponent<ZIndex>({ 1021 + index });
        battle.projected_move.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_MenuBox.png)") });
        battle.projected_move.GetComponent<Transform>()->is_active = false;

        battle.projected_move_text = FlexECS::Scene::CreateEntity("move_used_text"); // can always use GetEntityByName to find the entity
        battle.projected_move_text.AddComponent<Transform>({});
        battle.projected_move_text.AddComponent<Position>({ Vector3(1475, 500, 0) });
        battle.projected_move_text.AddComponent<Rotation>({});
        battle.projected_move_text.AddComponent<Scale>({ Vector3(.5f, .5f, 0) });
        battle.projected_move_text.AddComponent<ZIndex>({ 1022 + index });
        battle.projected_move_text.AddComponent<Text>({
          FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
          FLX_STRING_NEW(R"()"),
          Vector3(1.0f, 1.0, 1.0f),
          { Renderer2DText::Alignment_Left, Renderer2DText::Alignment_Center },
          {                           1400,                              320 }
        });
        battle.projected_move_text.GetComponent<Transform>()->is_active = false;

        if (battle.is_tutorial)
        {
            battle.is_tutorial_running = true;
            //set up tutorial text

            FlexECS::Entity tutorial_box = FlexECS::Scene::CreateEntity("tutorial_textbox"); // can always use GetEntityByName to find the entity
            tutorial_box.AddComponent<Transform>({});
            tutorial_box.AddComponent<Position>({ Vector3(845, 110, 0) });
            tutorial_box.AddComponent<Rotation>({});
            tutorial_box.AddComponent<Scale>({ Vector3(0.6f, 0.6f, 0) });
            tutorial_box.AddComponent<ZIndex>({ 1021 + index });
            tutorial_box.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_MenuBox.png)") });

            FlexECS::Entity tutorial_press = FlexECS::Scene::CreateEntity("tutorial_press_button"); // can always use GetEntityByName to find the entity
            tutorial_press.AddComponent<Transform>({});
            tutorial_press.AddComponent<Position>({ Vector3(850, -20, 0) });
            tutorial_press.AddComponent<Rotation>({});
            tutorial_press.AddComponent<Scale>({ Vector3(0.9f, 0.75f, 0) });
            tutorial_press.AddComponent<ZIndex>({ 1021 + index });
            tutorial_press.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_Win_Text_Press Any Button To Continue.png)") });

            battle.tutorial_text = FlexECS::Scene::CreateEntity("tutorial_text"); // can always use GetEntityByName to find the entity
            battle.tutorial_text.AddComponent<Transform>({});
            battle.tutorial_text.AddComponent<Position>({ Vector3(525, 100, 0) });
            battle.tutorial_text.AddComponent<Rotation>({});
            battle.tutorial_text.AddComponent<Scale>({ Vector3(.5f, .5f, 0) });
            battle.tutorial_text.AddComponent<ZIndex>({ 1022 + index });
            battle.tutorial_text.AddComponent<Text>({
              FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
              FLX_STRING_NEW(R"()"),
              Vector3(1.0f, 1.0, 1.0f),
              { Renderer2DText::Alignment_Left, Renderer2DText::Alignment_Center },
              {                           1400,                              320 }
            });
        }

        Update_Character_Status();

        //Fade in
        Application::MessagingSystem::Send("TransitionStart", std::pair<int, double>{ 1, 1.0 });
    }

    void Update_Speed_Bar()
    {

        //speed bar update
        std::stable_sort(
      battle.speed_bar.begin(), battle.speed_bar.end(),
      [](const _Character* a, const _Character* b)
        {
            return a->current_speed < b->current_speed;
        }
        );

        // resolve the speed bar (just minus the speed by the value of the first character)
        int first_speed = battle.speed_bar[0]->current_speed;
        if (first_speed > 0)
            for (auto& character : battle.speed_bar)
            {
                character->current_speed -= first_speed;
            }

        // update the character id in the slot based on the speed bar order
        for (FlexECS::Entity& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, SpeedBarSlot>())
        {
            auto& speed_bar_slot = *entity.GetComponent<SpeedBarSlot>();

            if (speed_bar_slot.slot_number <= battle.speed_bar.size())
                speed_bar_slot.character = battle.speed_bar[speed_bar_slot.slot_number - 1]->character_id;
            else
            {
                speed_bar_slot.character = 0;
                if (speed_bar_slot.slot_number != 1)
                {
                    FlexECS::Scene::GetEntityByName("Speedbar Accent " + std::to_string(speed_bar_slot.slot_number - 1)).GetComponent<Transform>()->is_active = false;
                }
                entity.GetComponent<Transform>()->is_active = false;
            }
        }

        // upon each icon in the slot based on character id
        for (FlexECS::Entity& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Sprite, SpeedBarSlot>())
        {
            auto& sprite = *entity.GetComponent<Sprite>();
            auto& speed_bar_slot = *entity.GetComponent<SpeedBarSlot>();

            // get the character in the slot
            switch (speed_bar_slot.character)
            {
            case 0:
                sprite.sprite_handle = sprite_handles.empty;
                break;
            case 1:
                sprite.sprite_handle = sprite_handles.renko;
                break;
            case 2:
                sprite.sprite_handle = sprite_handles.grace;
                break;
            case 3:
                sprite.sprite_handle = sprite_handles.enemy1;
                break;
            case 4:
                sprite.sprite_handle = sprite_handles.enemy2;
                break;
            case 5:
                sprite.sprite_handle = sprite_handles.jack;
                break;
            }
        }

        /*
        //speed bar delay
        battle.disable_input_timer += 1.0f;
        */
    }

    bool Compare(int a, int b)
    {
        return a > b;
    }

    void Update_Character_Status()
    {
        // update the healthbar display
        // loop through each healthbar and update the scale based on the current health
        // there is the healthbarslot, the actual healthbar entity, and the character entity that are all needed
        for (auto& character : battle.drifters_and_enemies)
        {
            auto entity = FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(character.current_slot + 1) + " Current Healthbar");
            if (character.character_id > 2)
            entity = FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(character.current_slot + 1) + " Current Healthbar");
            else entity = FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(character.current_slot + 1) + " Current Healthbar");
            // guard
            if (!entity.HasComponent<Scale>() || !entity.HasComponent<Healthbar>()) continue;

            auto* scale = entity.GetComponent<Scale>();
            auto* healthbar = entity.GetComponent<Healthbar>();
            auto* position = entity.GetComponent<Position>();

            // Calculate the health percentage and new scale.
            float health_percentage = static_cast<float>(character.current_health) / static_cast<float>(character.health);
            if (health_percentage < 0) health_percentage = 0;
            // Update Scale
            scale->scale.x = healthbar->original_scale.x * health_percentage;

            // Update Position
            position->position.x = healthbar->original_position.x - static_cast<float>((70) * (1.0 - health_percentage));

            /*entity = FlexECS::Scene::GetEntityByName(character.name + " Stats");
            entity = FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(character.current_slot + 1) + " Stats");
            if (character.character_id > 2)
                entity = FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(character.current_slot + 1) + " Stats");
            else entity = FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(character.current_slot + 1) + " Stats");

            // guard
            if (!entity.HasComponent<Text>()) continue;

            // get the character's current health
            std::string stats = ""; // ENABLE THE BOTTOM LINE IF YOU WANT TEXT FOR HP SPD ETC
            //std::string stats = "HP: " + std::to_string(character.current_health) + " / " + std::to_string(character.health) + " , " + "SPD: " + std::to_string(character.current_speed);
            entity.GetComponent<Text>()->text = FLX_STRING_NEW(stats);
            */

            std::vector<int> unsorted_buff_duration;
            std::vector<int> sorted_buff_duration;
            unsorted_buff_duration.clear();
            sorted_buff_duration.clear();
            unsorted_buff_duration.push_back(character.attack_buff_duration);
            unsorted_buff_duration.push_back(character.attack_debuff_duration);
            unsorted_buff_duration.push_back(character.stun_debuff_duration);
            unsorted_buff_duration.push_back(character.shield_buff_duration);
            unsorted_buff_duration.push_back(character.protect_buff_duration);
            sorted_buff_duration = unsorted_buff_duration;
            sort(sorted_buff_duration.begin(), sorted_buff_duration.end(), Compare);

            for (int i = 1; i < 6; i++)
            {
                int buff_type = 0;
                for (int j = 0; j < unsorted_buff_duration.size(); j++)
                {
                    if (sorted_buff_duration[0] == unsorted_buff_duration[j])
                    {
                        if (sorted_buff_duration[0] == 0)
                        {
                            buff_type = 5;
                        }
                        else buff_type = j;

                        sorted_buff_duration.erase(sorted_buff_duration.begin());
                        unsorted_buff_duration[j] = -1;
                        break;
                    }
                }

                entity = FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(character.current_slot + 1) + " Buff " + std::to_string(i));
                if (character.character_id > 2)
                    entity = FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(character.current_slot + 1) + " Buff " + std::to_string(i));
                else entity = FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(character.current_slot + 1) + " Buff " + std::to_string(i));

                switch (buff_type)
                {
                case 0:
                    entity.GetComponent<Sprite>()->sprite_handle = FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Attack_+1.png)");
                    entity.GetComponent<Transform>()->is_active = true;
                    break;
                case 1:
                    entity.GetComponent<Sprite>()->sprite_handle = FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Attack_-1.png)");
                    entity.GetComponent<Transform>()->is_active = true;
                    break;
                case 2:
                    entity.GetComponent<Sprite>()->sprite_handle = FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Stun.png)");
                    entity.GetComponent<Transform>()->is_active = true;
                    break;
                case 3:
                    entity.GetComponent<Sprite>()->sprite_handle = FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Def.png)");
                    entity.GetComponent<Transform>()->is_active = true;
                    break;
                case 4:
                    entity.GetComponent<Sprite>()->sprite_handle = FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Heal.png)");
                    entity.GetComponent<Transform>()->is_active = true;
                    break;
                case 5:
                    entity.GetComponent<Transform>()->is_active = false;
                    break;

                }
            }
            /*entity = FlexECS::Scene::GetEntityByName(character.name + " Attack_Buff");

            // guard
            if (!entity) continue;

            // get the character's current health
            if (character.attack_buff_duration > 0)
                entity.GetComponent<Transform>()->is_active = true;
            else entity.GetComponent<Transform>()->is_active = false;


            entity = FlexECS::Scene::GetEntityByName(character.name + " Attack_Debuff");

            // guard
            if (!entity) continue;

            // get the character's current health
            if (character.attack_debuff_duration > 0)
                entity.GetComponent<Transform>()->is_active = true;
            else entity.GetComponent<Transform>()->is_active = false;


            entity = FlexECS::Scene::GetEntityByName(character.name + " Stun_Debuff");

            // guard
            if (!entity) continue;

            // get the character's current health
            if (character.stun_debuff_duration > 0)
                entity.GetComponent<Transform>()->is_active = true;
            else entity.GetComponent<Transform>()->is_active = false;


            entity = FlexECS::Scene::GetEntityByName(character.name + " Shield_Buff");

            // guard
            if (!entity) continue;

            // get the character's current health
            if (character.shield_buff_duration > 0)
                entity.GetComponent<Transform>()->is_active = true;
            else entity.GetComponent<Transform>()->is_active = false;


            entity = FlexECS::Scene::GetEntityByName(character.name + " Protect_Buff");

            // guard
            if (!entity) continue;

            // get the character's current health
            if (character.protect_buff_duration > 0)
                entity.GetComponent<Transform>()->is_active = true;
            else entity.GetComponent<Transform>()->is_active = false;*/
        }

    }

    void Update_Damage_And_Targetting_Previews()
    {
      //Update damage preview displays
      //As well as speed bar targeting icon
      //Turn off all previews is_active, then turn back on
      /*for (auto character : battle.drifters_and_enemies)
      {
        if (character.character_id <= 2) continue;

        //auto entity = FlexECS::Scene::GetEntityByName(character.name + " DamagePreview");
        auto entity = FlexECS::Scene::GetEntityByName("Enemy Healthbar Preview " + std::to_string(character.current_slot + 1));
        entity.GetComponent<Transform>()->is_active = false;
      }*/

        for (int i = 1; i < 6; i++)
        {
            auto e = FlexECS::Scene::GetEntityByName("Enemy Healthbar Preview " + std::to_string(i));
            e.GetComponent<Transform>()->is_active = false;
        }

      for (FlexECS::Entity& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Sprite, SpeedBarSlotTarget>())
      {
        entity.GetComponent<Transform>()->is_active = false;
      }

      //Find which enemies are targetted, turn on damage previews
      if (battle.current_move && battle.is_player_turn)
      {
        std::vector<_Character> targets;
        for (int i = 0; i < battle.current_move->effect.size(); i++)
        {
          if (battle.current_move->target[i] == "ALL_ENEMIES")
          {
            for (auto character : battle.drifters_and_enemies)
            {
              if (character.is_alive && character.character_id > 2)
              {
                targets.push_back(character);
              }
            }
          }
          else if (battle.current_move->target[i] == "ADJACENT_ENEMIES")
          {
            for (auto character : battle.drifters_and_enemies)
            {
              if (character.character_id <= 2) continue;
              if (character.is_alive &&
                (character.current_slot == battle.target_num - 1 || character.current_slot == battle.target_num ||
                  character.current_slot == battle.target_num + 1))
              {
                targets.push_back(character);
              }
            }
          }
          else if (battle.current_move->target[i] == "SINGLE_ENEMY")
          {
            for (auto character : battle.drifters_and_enemies)
            {
              if (character.is_alive && character.current_slot == battle.target_num && character.character_id > 2)
              {
                targets.push_back(character);
              }
            }
          }
          //Draw damage preview bar
          for (auto target : targets)
          {
              auto entity = FlexECS::Scene::GetEntityByName("Enemy Healthbar Preview " + std::to_string(target.current_slot + 1));

            //auto entity = FlexECS::Scene::GetEntityByName(target.name + " DamagePreview");
            if (!entity.HasComponent<Scale>() && !entity.HasComponent<Healthbar>()) continue;

            if (target.shield_buff_duration > 0)
            {
              //if current move is a buff stripper, add enemy to damage calculations
              bool stripping_effect = false;
              for (size_t j = 0; j < battle.current_move->effect.size(); j++)
              {
                if (battle.current_move->effect[j] == "Strip" || battle.god_mode)
                {
                  stripping_effect = true;
                  break;
                }
              }
              if (!stripping_effect) continue;
            }

            entity.GetComponent<Transform>()->is_active = true;

            auto* scale = entity.GetComponent<Scale>();
            auto* healthbar = entity.GetComponent<Healthbar>();
            auto* position = entity.GetComponent<Position>();

            float current_health_percentage = static_cast<float>(target.current_health) / static_cast<float>(target.health);
            float right_edge_pos = (healthbar->original_position.x - (70 * (1.0f - current_health_percentage)))
              + (70 * (current_health_percentage));

            float damage_taken = 0;
            for (int k = 0; k < battle.current_move->effect.size(); k++)
            {
                if (battle.current_move->effect[k] == "Damage")
                {
                    if (battle.current_move->target[k] == "SINGLE_ENEMY" && battle.initial_target->current_slot == target.current_slot)
                    {
                        float current_damage = static_cast<float>(battle.current_move->value[k]);
                        if (battle.current_character->attack_buff_duration > 0)
                        {
                            current_damage += current_damage / 2;
                        }
                        if (battle.current_character->attack_debuff_duration > 0)
                        {
                            current_damage -= current_damage / 2;
                        }
                        if (battle.god_mode)
                        {
                            current_damage *= 10;
                        }
                        damage_taken += current_damage;
                    }
                    else if (battle.current_move->target[k] == "ALL_ENEMIES")
                    {
                        float current_damage = static_cast<float>(battle.current_move->value[k]);
                        if (battle.current_character->attack_buff_duration > 0)
                        {
                            current_damage += current_damage / 2;
                        }
                        if (battle.current_character->attack_debuff_duration > 0)
                        {
                            current_damage -= current_damage / 2;
                        }
                        if (battle.god_mode)
                        {
                            current_damage *= 10;
                        }
                        damage_taken += current_damage;
                    }
                }
            }
            /*if (battle.current_move->effect[i] == "Damage")
            {
              damage_taken = static_cast<float>(battle.current_move->value[i]);
            }
            if (battle.current_character->attack_buff_duration > 0)
            {
              damage_taken += damage_taken / 2;
            }
            if (battle.current_character->attack_debuff_duration > 0)
            {
              damage_taken -= damage_taken / 2;
            }*/
            damage_taken = (damage_taken > static_cast<float>(target.current_health)) ? static_cast<float>(target.current_health) : damage_taken;
            float percentage_damage_taken = static_cast<float>(damage_taken) / static_cast<float>(target.health);

            scale->scale.x = healthbar->original_scale.x * percentage_damage_taken;
            position->position.x = right_edge_pos - (70 * percentage_damage_taken);
          }

          
          //Draw Targetting icon over targetted enemy in speed bar
          for (FlexECS::Entity& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Sprite, SpeedBarSlotTarget>())
          {
            int slot_number = entity.GetComponent<SpeedBarSlotTarget>()->slot_number;

            int size = static_cast<int>(battle.speed_bar.size());

            if (slot_number > size)
            {
              entity.GetComponent<Transform>()->is_active = false;
              continue;
            }

            //check if the character in the slot is being targetted.
            _Character* character = battle.speed_bar[slot_number - 1];
            for (_Character target : targets)
            {
              if (target.name == character->name)
              {
                entity.GetComponent<Transform>()->is_active = true;
              }
            }
          }

          targets.clear();
        }

      }
    }
    
    // Just delay the fucking battle start anim
    void Play_Battle_Start() 
    {
      static float time_played = 2.f;
      static bool is_init = false;

      if (!is_init)
      {
        time_played = 3.f;
        is_init = true;
        FlexECS::Entity overlay = FlexECS::Scene::GetEntityByName("Combat Overlay");
        overlay.GetComponent<Animator>()->spritesheet_handle
          = FLX_STRING_NEW(R"(/images/Screen_Overlays/BattleStart/UI_BattleStart_Spritesheet.flxspritesheet)");
        overlay.GetComponent<Transform>()->is_active = true;
        overlay.GetComponent<Animator>()->should_play = true;
        overlay.GetComponent<Animator>()->is_looping = false;
        overlay.GetComponent<Animator>()->return_to_default = false;
        overlay.GetComponent<Animator>()->current_frame = 0;
      }

      if (time_played > 0.f)
      {
        FlexECS::Entity overlay = FlexECS::Scene::GetEntityByName("Combat Overlay");

        time_played -= Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
      }
      else
      {
        // Flag to start the battle, and reset this outdated value
        is_init = false;
        battle.play_battle_start = false;
        battle.start_of_turn = true;
        FlexECS::Entity overlay = FlexECS::Scene::GetEntityByName("Combat Overlay");
        overlay.GetComponent<Transform>()->is_active = false;
      }
    }

    // Lerp function from a to b
    static float Lerp(float a, float b, float t)
    {
        return a + t * (b - a);
    }

    // Returns a value of 1-sin with a lowest value of 0.5. i.e Goes from 0.5 to 1 to 0.5
   float HalfSinCurve(float t) 
   {
      //float constexpr M_PI = 3.14f;
      float constexpr pi = 3.14f;
      return std::max(1 - std::sinf(t * pi), 0.5f); // Sin function maps t to 0 to pi
   }

    void PlaySpeedbarAnimation()
    {
        battle.curr_char_highlight.GetComponent<Transform>()->is_active = false; // Disable curr char accent otherwise animation will look weird

        constexpr float duration = 0.8f; // Duration for each phase
        constexpr float pulse_dur = 1.f;
        constexpr float max_arc_height = -200.f;

        static float time_played = 0.5f;
        static Vector3 dest[7];
        static bool is_init = false;
        static bool pulse_played = false;

        // Clamp to max number of alive characters
        battle.curr_char_pos_after_taking_turn = std::min(static_cast<int>(battle.speed_bar.size()) - 1, battle.curr_char_pos_after_taking_turn);

        if (!pulse_played)
        {
          // Pulse the character icon for duration amount of time 
          if (time_played < pulse_dur)
          {
            float t = time_played / duration;
            t = std::clamp(t, 0.f, 1.f);

            battle.speed_slot_icons[0].GetComponent<Scale>()->scale = Vector3(HalfSinCurve(t) + 0.2f, HalfSinCurve(t) + 0.2f, HalfSinCurve(t) + 0.2f);

            time_played += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() * 2.f;
            return;
          }

          pulse_played = true;
        }

        if (!is_init)
        {
          time_played = 0.f; // Reset from pulse played

            // Set the initial position of the element to move
            dest[0] = battle.speed_slot_icons[battle.curr_char_pos_after_taking_turn].GetComponent<Position>()->position;

            // Cache lerp values for all inbetween elements
            for (int i{ 1 }; i < battle.speed_slot_position.size(); ++i)
            {
                if (i == battle.curr_char_pos_after_taking_turn + 1)
                    break;

                dest[i] = battle.speed_slot_icons[i - 1].GetComponent<Position>()->position;
            }

            is_init = true;
        }

        // Lerp to the supposed position
        if (time_played < duration)
        {
            float t = time_played / duration;
            t = std::clamp(t, 0.f, 1.f);

            float arc_t = 1.f - (2.f * t - 1.f) * (2.f * t - 1.f); // Parabolic arc

            battle.speed_slot_icons[0].GetComponent<Position>()->position = Vector3(Lerp(battle.speed_slot_position[0].x, dest[0].x, t),
                                                                                       Lerp(battle.speed_slot_position[0].y, dest[0].y, t) + max_arc_height * arc_t,
                                                                                       Lerp(battle.speed_slot_position[0].z, dest[0].z, t));

            battle.speed_slot_icons[0].GetComponent<Scale>()->scale = Vector3(HalfSinCurve(t), HalfSinCurve(t), HalfSinCurve(t));

            // Lerp the rest of the icons
            for (int i{ 1 }; i < battle.speed_slot_position.size(); ++i)
            {
                if (i == battle.curr_char_pos_after_taking_turn + 1)
                {
                    break;
                }

                battle.speed_slot_icons[i].GetComponent<Position>()->position = Vector3(Lerp(battle.speed_slot_position[i].x, dest[i].x, t),
                                                                                           Lerp(battle.speed_slot_position[i].y, dest[i].y, t),
                                                                                           Lerp(battle.speed_slot_position[i].z, dest[i].z, t));
            }

            time_played += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
            return;
        }

        // Ensure final position is set
        for (int i{ 0 }; i < battle.speed_slot_position.size(); ++i)
        {
            if (i == battle.curr_char_pos_after_taking_turn + 1)
            {
                break;
            }

            battle.speed_slot_icons[i].GetComponent<Position>()->position = dest[i];
        }

        // Reset for next animation
        time_played = 0.f;

        battle.speedbar_animating = false;
        battle.end_of_turn = true;
        is_init = false;
        pulse_played = false;
    }

    void Start_Of_Game()
    {
        File& file = File::Open(Path::current("assets/saves/battlescene_v12.flxscene"));

        FlexECS::Scene::SetActiveScene(FlexECS::Scene::Load(file));

        battle.curr_char_highlight = FlexECS::Scene::GetEntityByName("Curr Char Highlight");

        #pragma region Load _Battle Data
        battle.speed_slot_icons[0] = FlexECS::Scene::GetEntityByName("Speed slot 1");
        battle.speed_slot_icons[1] = FlexECS::Scene::GetEntityByName("Speed slot 2");
        battle.speed_slot_icons[2] = FlexECS::Scene::GetEntityByName("Speed slot 3");
        battle.speed_slot_icons[3] = FlexECS::Scene::GetEntityByName("Speed slot 4");
        battle.speed_slot_icons[4] = FlexECS::Scene::GetEntityByName("Speed slot 5");
        battle.speed_slot_icons[5] = FlexECS::Scene::GetEntityByName("Speed slot 6");
        battle.speed_slot_icons[6] = FlexECS::Scene::GetEntityByName("Speed slot 7");

        battle.speed_slot_icons[0].GetComponent<Transform>()->is_active = false;
        battle.speed_slot_icons[1].GetComponent<Transform>()->is_active = false;
        battle.speed_slot_icons[2].GetComponent<Transform>()->is_active = false;
        battle.speed_slot_icons[3].GetComponent<Transform>()->is_active = false;
        battle.speed_slot_icons[4].GetComponent<Transform>()->is_active = false;
        battle.speed_slot_icons[5].GetComponent<Transform>()->is_active = false;
        battle.speed_slot_icons[6].GetComponent<Transform>()->is_active = false;

        battle.speed_slot_position[0] = battle.speed_slot_icons[0].GetComponent<Position>()->position;
        battle.speed_slot_position[1] = battle.speed_slot_icons[1].GetComponent<Position>()->position;
        battle.speed_slot_position[2] = battle.speed_slot_icons[2].GetComponent<Position>()->position;
        battle.speed_slot_position[3] = battle.speed_slot_icons[3].GetComponent<Position>()->position;
        battle.speed_slot_position[4] = battle.speed_slot_icons[4].GetComponent<Position>()->position;
        battle.speed_slot_position[5] = battle.speed_slot_icons[5].GetComponent<Position>()->position;
        battle.speed_slot_position[6] = battle.speed_slot_icons[6].GetComponent<Position>()->position;


        FlexECS::Scene::GetEntityByName("Speedbar Accent 1").GetComponent<Transform>()->is_active = false;
        FlexECS::Scene::GetEntityByName("Speedbar Accent 2").GetComponent<Transform>()->is_active = false;
        FlexECS::Scene::GetEntityByName("Speedbar Accent 3").GetComponent<Transform>()->is_active = false;
        FlexECS::Scene::GetEntityByName("Speedbar Accent 4").GetComponent<Transform>()->is_active = false;
        FlexECS::Scene::GetEntityByName("Speedbar Accent 5").GetComponent<Transform>()->is_active = false;
        FlexECS::Scene::GetEntityByName("Speedbar Accent 6").GetComponent<Transform>()->is_active = false;

        for (int i = 1; i < 6; i++)
        {
            auto entity = FlexECS::Scene::GetEntityByName("Enemy Healthbar Preview " + std::to_string(i));
            entity.GetComponent<Healthbar>()->original_position = entity.GetComponent<Position>()->position;
            entity.GetComponent<Healthbar>()->original_scale = entity.GetComponent<Scale>()->scale;
            entity.GetComponent<Transform>()->is_active = false;
        }

        Internal_ParseBattle(file_name);
        // load the battle
        //if (battle.is_tutorial)
        //Internal_ParseBattle(R"(/data/tutorial.flxbattle)");
        //else Internal_ParseBattle(R"(/data/debug.flxbattle)");

        for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, MoveUI>())
            entity.GetComponent<Transform>()->is_active = false;
        FLX_STRING_GET(FlexECS::Scene::GetEntityByName("Move 1 Text").GetComponent<Text>()->text) =
            "";
        FLX_STRING_GET(FlexECS::Scene::GetEntityByName("Move 2 Text").GetComponent<Text>()->text) =
            "";
        FLX_STRING_GET(FlexECS::Scene::GetEntityByName("Move 3 Text").GetComponent<Text>()->text) =
            "";
        FLX_STRING_GET(FlexECS::Scene::GetEntityByName("Move Description Text").GetComponent<Text>()->text) =
            "";

        //Create speed bar sprite handles
        sprite_handles.empty =
          FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_SpeedBar_AllyProfile_Empty.png)");
        sprite_handles.renko =
          FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_SpeedBar_AllyProfile_Renko.png)");
        sprite_handles.grace =
          FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_SpeedBar_AllyProfile_Grace.png)");
        sprite_handles.enemy1 =
          FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_SpeedBar_EnemyProfile_En1.png)");
        sprite_handles.enemy2 =
          FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_SpeedBar_EnemyProfile_En2.png)");
        sprite_handles.jack =
          FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_SpeedBar_EnemyProfile_Jack.png)");

        //start function cycle
        //battle.start_of_turn = true;  // This is commented out because we want to play the battle start animation first
        battle.play_battle_start = true;
        battle.current_move = nullptr;
        battle.move_select = false;
        battle.move_resolution = false;
        battle.end_of_turn = false;

        battle.change_phase = true;

        battle.m_originalWidth = CameraManager::GetMainGameCamera()->GetOrthoWidth();
        battle.m_originalHeight = CameraManager::GetMainGameCamera()->GetOrthoHeight();

      //Scripting initialize
      Application::MessagingSystem::Send<bool>("Initialize VFX", true);

      // Play bgm for game
      FlexECS::Scene::GetEntityByName("Background Music").GetComponent<Audio>()->should_play = true;

      Log::Debug("Start Game");
    }

    void Start_Of_Turn()
    {
      battle.curr_char_highlight.GetComponent<Transform>()->is_active = true; // Enable curr char accent

        //one-time call upon changing phase
        if (battle.change_phase)
        {
            Log::Debug("Start Turn");
            battle.change_phase = false;

            // Resets to original position in case any animation happened to move the speedbar
            for (int i{}; i < battle.speed_slot_position.size(); ++i)
            {
              battle.speed_slot_icons[i].GetComponent<Position>()->position = battle.speed_slot_position[i];
            }

            Update_Speed_Bar();

            //reset info
            battle.current_character = battle.speed_bar.front();
            battle.current_move = nullptr;
            battle.initial_target = nullptr;
            battle.move_num = 0;
            battle.target_num = 0;

            FLX_ASSERT(battle.current_character != nullptr, "Current character is null.");
            FLX_ASSERT(battle.current_character->is_alive, "Current character is dead.");

            battle.is_player_turn = (battle.current_character->character_id <= 2);

            //play start of turn sounds
            if (battle.is_player_turn)
            {// Plays sound if swap from enemy phase to player phase
                std::string audio_to_play = "/audio/start turn.mp3";
                //std::string audio_to_play = "/audio/" + battle.current_character->name + " start.mp3";
                //Log::Debug(audio_to_play);
                FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file = FLX_STRING_NEW(audio_to_play);
                FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                battle.curr_char_highlight.GetComponent<Sprite>()->sprite_handle = FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_SpeedBar_PlayerTurn_Indicator.png)");
            }
            else
            {
              battle.curr_char_highlight.GetComponent<Sprite>()->sprite_handle = FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_SpeedBar_EnemyTurn_Indicator.png)");
            }

            
        }

        //tutorial only
        if (battle.is_tutorial_running)
        {
            for (int key = GLFW_KEY_SPACE; key < GLFW_KEY_LAST; key++)
            {
                if (Input::GetKeyDown(key))
                {
                    battle.tutorial_info++;
                }
            }
        }
        if (battle.is_tutorial && battle.tutorial_info < 1)
        {
            return;
        }

        battle.change_phase = true;
        battle.start_of_turn = false;
        battle.move_select = true;
        battle.move_resolution = false;
        battle.end_of_turn = false;
        battle.enable_combat_camera = true;
    }

    void Move_Select()
    {
        //one-time call upon changing phase
        if (battle.change_phase)
        {
            Log::Debug("Move Select");
            battle.change_phase = false;

            //skip turn if stunned, go straight to end of turn resolution
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

                Update_Character_Status();

                //projected character UI
                int projected_speed = battle.current_character->speed_change;
                int slot_number = -1; //will always be bigger than first element (itself), account for +1 for slot 0.
                    for (auto character : battle.speed_bar)
                    {
                        if (projected_speed <= character->current_speed)
                        {
                            //if smaller than slot 1, -1 + 1 = 0 (always bigger than slot 0, aka itself, it will be displayed on slot 0 + offset to the right, between slot 0 and slot 1
                            break;
                        }
                        else
                        {
                            slot_number++;
                        }
                    }

                    if (slot_number < 0)
                    {
                        slot_number = 0;
                    }
                battle.curr_char_pos_after_taking_turn = slot_number; // might need to -1

                battle.current_character->current_speed = battle.current_character->speed_change;
                battle.current_character->speed_change = 0;

                battle.start_of_turn = false;
                battle.move_select = false;
                battle.move_resolution = false;
                battle.speedbar_animating = true;

                battle.change_phase = true;
                return;
            }
            else
            {
                //continue with move selection, default selects move and target
                if (battle.is_player_turn)
                {
                    //set move ui to true
                    for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, MoveUI>())
                        entity.GetComponent<Transform>()->is_active = true;

                    //default selects move 1
                    battle.move_num = 1;
                    battle.current_move = &battle.current_character->move_one;

                    //default selects move target
                    for (auto &character : battle.drifters_and_enemies)
                    {
                        if (battle.current_move->target[0] == "ALL_ALLIES" || battle.current_move->target[0] == "NEXT_ALLY" || battle.current_move->target[0] == "SINGLE_ALLY" || battle.current_move->target[0] == "SELF")
                        {
                            if (character.is_alive && character.character_id <= 2)
                            {
                                battle.initial_target = &character;
                                battle.target_num = character.current_slot;
                                break;
                            }
                        }
                        else
                        {
                            if (character.is_alive && character.character_id > 2)
                            {
                                battle.initial_target = &character;
                                battle.target_num = character.current_slot;
                                break;
                            }
                        }
                    }
                }//player default selects move and target
                else //enemy default selects move and target
                {
                    //default select move 1-3
                    battle.move_num = Range<int>(1, 3).Get();
                    battle.current_move = &battle.current_character->move_three;
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

                    //default selects move target
                    while (battle.initial_target == nullptr || !battle.initial_target->is_alive)
                    {
                        if (battle.current_move->target[0] == "ALL_ALLIES" || battle.current_move->target[0] == "NEXT_ALLY" || battle.current_move->target[0] == "SINGLE_ALLY" || battle.current_move->target[0] == "SELF")
                        {
                            battle.target_num = Range<int>(0, 4).Get();
                            for (auto &character : battle.drifters_and_enemies)
                            {
                                if (character.is_alive && character.current_slot == battle.target_num && character.character_id > 2)
                                {
                                    battle.initial_target = &character;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            battle.target_num = Range<int>(0, 1).Get();
                            for (auto &character : battle.drifters_and_enemies)
                            {
                                if (character.is_alive && character.current_slot == battle.target_num && character.character_id <= 2)
                                {
                                    battle.initial_target = &character;
                                    break;
                                }
                            }
                        }
                    }

                    //projected character UI
                    int projected_speed = battle.current_move->speed + battle.current_character->speed_change;
                    int slot_number = -1; //will always be bigger than first element (itself), account for +1 for slot 0.
                        for (auto character : battle.speed_bar)
                        {
                            if (projected_speed <= character->current_speed)
                            {
                                //if smaller than slot 1, -1 + 1 = 0 (always bigger than slot 0, aka itself, it will be displayed on slot 0 + offset to the right, between slot 0 and slot 1
                                break;
                            }
                            else
                            {
                                slot_number++;
                            }
                        }
                        if (slot_number < 0)
                        {
                            slot_number = 0;
                        }
                    battle.curr_char_pos_after_taking_turn = slot_number; // might need to -1
                }
            }
        }

        //enable move changes if player turn
        if (battle.is_player_turn)
        {
            //swap moves
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

                for (auto &character : battle.drifters_and_enemies)
                {
                    if (battle.current_move->target[0] == "ALL_ALLIES" || battle.current_move->target[0] == "NEXT_ALLY" || battle.current_move->target[0] == "SINGLE_ALLY" || battle.current_move->target[0] == "SELF")
                    {
                        if (character.is_alive && character.character_id <= 2)
                        {
                            battle.initial_target = &character;
                            battle.target_num = character.current_slot;
                            break;
                        }
                    }
                    else
                    {
                        if (character.is_alive && character.character_id > 2)
                        {
                            battle.initial_target = &character;
                            battle.target_num = character.current_slot;
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

                for (auto &character : battle.drifters_and_enemies)
                {
                    if (battle.current_move->target[0] == "ALL_ALLIES" || battle.current_move->target[0] == "NEXT_ALLY" || battle.current_move->target[0] == "SINGLE_ALLY" || battle.current_move->target[0] == "SELF")
                    {
                        if (character.is_alive && character.character_id <= 2)
                        {
                            battle.initial_target = &character;
                            battle.target_num = character.current_slot;
                            break;
                        }
                    }
                    else
                    {
                        if (character.is_alive && character.character_id > 2)
                        {
                            battle.initial_target = &character;
                            battle.target_num = character.current_slot;
                            break;
                        }
                    }
                }
            }

            //swap targets
            if (Input::GetKeyDown(GLFW_KEY_A))
            {
                battle.initial_target = nullptr;
                while (battle.initial_target == nullptr || !battle.initial_target->is_alive)
                {
                    battle.target_num--;
                    for (auto &character : battle.drifters_and_enemies)
                    {
                        if (battle.current_move->target[0] == "ALL_ALLIES" || battle.current_move->target[0] == "NEXT_ALLY" || battle.current_move->target[0] == "SINGLE_ALLY" || battle.current_move->target[0] == "SELF")
                        {
                            if (battle.target_num < 0)
                            {
                                battle.target_num = 1;
                            }

                            if (character.is_alive && character.current_slot == battle.target_num && character.character_id <= 2)
                            {
                                battle.initial_target = &character;
                                break;
                            }
                        }
                        else
                        {
                            if (battle.target_num < 0)
                            {
                                battle.target_num = 4;
                            }

                            if (character.is_alive && character.current_slot == battle.target_num && character.character_id > 2)
                            {
                                battle.initial_target = &character;
                                break;
                            }
                        }
                    }
                }
            }
            else if (Input::GetKeyDown(GLFW_KEY_D))
            {
                battle.initial_target = nullptr;
                while (battle.initial_target == nullptr || !battle.initial_target->is_alive)
                {
                    battle.target_num++;
                    for (auto &character : battle.drifters_and_enemies)
                    {
                        if (battle.current_move->target[0] == "ALL_ALLIES" || battle.current_move->target[0] == "NEXT_ALLY" || battle.current_move->target[0] == "SINGLE_ALLY" || battle.current_move->target[0] == "SELF")
                        {
                            if (battle.target_num > 1)
                            {
                                battle.target_num = 0;
                            }

                            if (character.is_alive && character.current_slot == battle.target_num && character.character_id <= 2)
                            {
                                battle.initial_target = &character;
                                break;
                            }
                        }
                        else
                        {
                            if (battle.target_num > 4)
                            {
                                battle.target_num = 0;
                            }

                            if (character.is_alive && character.current_slot == battle.target_num && character.character_id > 2)
                            {
                                battle.initial_target = &character;
                                break;
                            }
                        }
                    }
                }
            }

            //button sprite
            FLX_STRING_GET(FlexECS::Scene::GetEntityByName("Move 1").GetComponent<Sprite>()->sprite_handle) =
                (battle.move_num == 1) ? ("/images/battle ui/Battle_UI_Skill_Selected.png")
                : ("/images/battle ui/Battle_UI_Skill_Unselected.png");
            FLX_STRING_GET(FlexECS::Scene::GetEntityByName("Move 2").GetComponent<Sprite>()->sprite_handle) =
                (battle.move_num == 2) ? ("/images/battle ui/Battle_UI_Skill_Selected.png")
                : ("/images/battle ui/Battle_UI_Skill_Unselected.png");
            FLX_STRING_GET(FlexECS::Scene::GetEntityByName("Move 3").GetComponent<Sprite>()->sprite_handle) =
                (battle.move_num == 3) ? ("/images/battle ui/Battle_UI_Skill_Selected.png")
                : ("/images/battle ui/Battle_UI_Skill_Unselected.png");

            //button text
            FLX_STRING_GET(FlexECS::Scene::GetEntityByName("Move 1 Text").GetComponent<Text>()->text) =
                battle.current_character->move_one.name;
            FLX_STRING_GET(FlexECS::Scene::GetEntityByName("Move 2 Text").GetComponent<Text>()->text) =
                battle.current_character->move_two.name;
            FLX_STRING_GET(FlexECS::Scene::GetEntityByName("Move 3 Text").GetComponent<Text>()->text) =
                battle.current_character->move_three.name;
            FLX_STRING_GET(FlexECS::Scene::GetEntityByName("Move Description Text").GetComponent<Text>()->text) =
                battle.current_move->description;

            switch (battle.current_character->current_slot + 1)
            {
            case 1:
                FlexECS::Scene::GetEntityByName("Move 1").GetComponent<Position>()->position = Vector3(475, 510, 0);
                FlexECS::Scene::GetEntityByName("Move 2").GetComponent<Position>()->position = Vector3(475, 475, 0);
                FlexECS::Scene::GetEntityByName("Move 3").GetComponent<Position>()->position = Vector3(475, 440, 0);
                FlexECS::Scene::GetEntityByName("Move 1 Text").GetComponent<Position>()->position = Vector3(375, 500, 0);
                FlexECS::Scene::GetEntityByName("Move 2 Text").GetComponent<Position>()->position = Vector3(375, 465, 0);
                FlexECS::Scene::GetEntityByName("Move 3 Text").GetComponent<Position>()->position = Vector3(375, 430, 0);

                FlexECS::Scene::GetEntityByName("Move Description Text").GetComponent<Position>()->position = Vector3(615, 500, 0);
                FlexECS::Scene::GetEntityByName("Move Description").GetComponent<Position>()->position = Vector3(785, 420, 0);

                FlexECS::Scene::GetEntityByName("Move Accent").GetComponent<Position>()->position = Vector3(423, 540, 0);
                break;
            case 2:
                FlexECS::Scene::GetEntityByName("Move 1").GetComponent<Position>()->position = Vector3(475, 305, 0);
                FlexECS::Scene::GetEntityByName("Move 2").GetComponent<Position>()->position = Vector3(475, 270, 0);
                FlexECS::Scene::GetEntityByName("Move 3").GetComponent<Position>()->position = Vector3(475, 235, 0);
                FlexECS::Scene::GetEntityByName("Move 1 Text").GetComponent<Position>()->position = Vector3(375, 295, 0);
                FlexECS::Scene::GetEntityByName("Move 2 Text").GetComponent<Position>()->position = Vector3(375, 260, 0);
                FlexECS::Scene::GetEntityByName("Move 3 Text").GetComponent<Position>()->position = Vector3(375, 225, 0);

                FlexECS::Scene::GetEntityByName("Move Description Text").GetComponent<Position>()->position = Vector3(615, 295, 0);
                FlexECS::Scene::GetEntityByName("Move Description").GetComponent<Position>()->position = Vector3(785, 215, 0);

                FlexECS::Scene::GetEntityByName("Move Accent").GetComponent<Position>()->position = Vector3(423, 335, 0);
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
            int projected_speed = battle.current_move->speed + battle.current_character->speed_change;
            int slot_number = -1; //will always be bigger than first element (itself), account for +1 for slot 0.
                battle.projected_character.GetComponent<Transform>()->is_active = true;
                battle.projected_character_text.GetComponent<Transform>()->is_active = true;
                for (auto character : battle.speed_bar)
                {
                    if (projected_speed <= character->current_speed)
                    {
                        //if smaller than slot 1, -1 + 1 = 0 (always bigger than slot 0, aka itself, it will be displayed on slot 0 + offset to the right, between slot 0 and slot 1
                        break;
                    }
                    else
                    {
                        slot_number++;
                    }
                }
                if (slot_number < 0)
                {
                    slot_number = 0;
                }
                battle.curr_char_pos_after_taking_turn = slot_number; // might need to -1
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
                        battle.projected_character_text.GetComponent<Position>()->position = battle.projected_character.GetComponent<Position>()->position + Vector3{ -30, -75, 0 };
                        break;
                    }
                    else
                    {
                        slot_number--; //counts backwards to the slot it's supposed to be
                    }
                }

            //tutorial only
            if (battle.is_tutorial_running)
            {
                for (int key = GLFW_KEY_SPACE; key < GLFW_KEY_LAST; key++)
                {
                    if (key == GLFW_KEY_W || key == GLFW_KEY_A || key == GLFW_KEY_S || key == GLFW_KEY_D)
                        continue;
                        if (Input::GetKeyDown(key))
                        {
                            battle.tutorial_info++;
                        }
                }
            }
            if (battle.is_tutorial && battle.tutorial_info < 4)
            {
                return;
            }
        }

        //confirm the use of move for AI or player
        if (!battle.is_player_turn || battle.is_player_turn && Input::GetKeyDown(GLFW_KEY_SPACE) && battle.initial_target != nullptr)
        {
            if (battle.is_player_turn) //disable move selection UI + resolve all move effects + play attack animation
            {
                //disable move UI
                for (FlexECS::Entity& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, MoveUI>())
                    entity.GetComponent<Transform>()->is_active = false;
                //disable target UI
                for (FlexECS::Entity& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, CharacterSlot>())
                    entity.GetComponent<Transform>()->is_active = false;
                //disable move text
                FLX_STRING_GET(FlexECS::Scene::GetEntityByName("Move 1 Text").GetComponent<Text>()->text) = "";
                FLX_STRING_GET(FlexECS::Scene::GetEntityByName("Move 2 Text").GetComponent<Text>()->text) = "";
                FLX_STRING_GET(FlexECS::Scene::GetEntityByName("Move 3 Text").GetComponent<Text>()->text) = "";
                FLX_STRING_GET(FlexECS::Scene::GetEntityByName("Move Description Text").GetComponent<Text>()->text) = "";

                //disable projected character icon
                battle.projected_character.GetComponent<Transform>()->is_active = false;
                battle.projected_character_text.GetComponent<Transform>()->is_active = false;
            }

            battle.disable_input_timer = 1.0f;
            battle.anim_timer = battle.disable_input_timer;
            if (battle.current_character->attack_buff_duration > 0)
                battle.current_character->attack_buff_duration -= 1;

            if (battle.current_character->attack_debuff_duration > 0)
                battle.current_character->attack_debuff_duration -= 1;

            if (battle.current_character->shield_buff_duration > 0)
                battle.current_character->shield_buff_duration -= 1;

            if (battle.current_character->protect_buff_duration > 0)
                battle.current_character->protect_buff_duration -= 1;

            Update_Character_Status();

            battle.start_of_turn = false;
            battle.move_select = false;
            battle.move_resolution = true;
            battle.end_of_turn = false;

            battle.change_phase = true;
            
        }
        
    }

    void Move_Resolution()
    {
        if (battle.anim_timer > 0.0f)
        battle.anim_timer -= Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();

        if (battle.is_player_turn)
        {
            // Temporarily move the character if targeting enemy
            if (battle.current_move->target[0] == "ALL_ALLIES" || battle.current_move->target[0] == "NEXT_ALLY" || battle.current_move->target[0] == "SINGLE_ALLY" || battle.current_move->target[0] == "SELF")
            {
                // If targeting allies, does nothing
            }
            else
            {
                float time_elapsed = battle.anim_timer;
                if (time_elapsed > 1.0f)
                {
                    time_elapsed = 1.0f;
                }
                float percent_moved = ((1.0f - time_elapsed) / 1.0f) * 3;
                if (percent_moved > 1.0f)
                {
                    percent_moved = 1.0f;
                }
                Vector3 new_position = battle.sprite_slot_positions[battle.initial_target->current_slot + 2] + Vector3{ -120, 0, 0 };

                Vector3 original_position = battle.sprite_slot_positions[battle.current_character->current_slot];

                Vector3 interpolated_position = original_position * (1.0f - percent_moved) + new_position * percent_moved;

                FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(battle.current_character->current_slot + 1)).GetComponent<Position>()->position = interpolated_position;

                FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(battle.current_character->current_slot + 1)).GetComponent<ZIndex>()->z = 50;
            }
        }
        else
        {
            if (battle.current_move->target[0] == "ALL_ALLIES" || battle.current_move->target[0] == "NEXT_ALLY" || battle.current_move->target[0] == "SINGLE_ALLY" || battle.current_move->target[0] == "SELF")
            {
                // If targeting allies, does nothing
            }
            else
            {
                float time_elapsed = battle.anim_timer;
                if (time_elapsed > 1.0f)
                {
                    time_elapsed = 1.0f;
                }
                float percent_moved = ((1.0f - time_elapsed) / 1.0f) * 3;
                if (percent_moved > 1.0f)
                {
                    percent_moved = 1.0f;
                }
                Vector3 new_position = battle.sprite_slot_positions[battle.initial_target->current_slot] + Vector3{ 120, 0, 0 };

                Vector3 original_position = battle.sprite_slot_positions[battle.current_character->current_slot + 2];

                Vector3 interpolated_position = original_position * (1.0f - percent_moved) + new_position * percent_moved;

                FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(battle.current_character->current_slot + 1)).GetComponent<Position>()->position = interpolated_position;

                FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(battle.current_character->current_slot + 1)).GetComponent<ZIndex>()->z = 50;
            }
        }
        if (battle.disable_input_timer > .0f)
        {
            return;
        }
        if (battle.change_phase)
        {
            Log::Debug("Move Resolution");
            battle.change_phase = false;

            FlexECS::Scene::GetEntityByName("move_used_textbox").GetComponent<Transform>()->is_active = true;
            FlexECS::Scene::GetEntityByName("move_used_text").GetComponent<Transform>()->is_active = true;

            // Set move used text cleanly
            std::string strip_underscore_text = battle.current_move->name;
            ReplaceUnderscoresWithSpaces(strip_underscore_text);
            FLX_STRING_GET(FlexECS::Scene::GetEntityByName("move_used_text").GetComponent<Text>()->text) = strip_underscore_text;

            if (battle.is_player_turn) //disable move selection UI + resolve all move effects + play attack animation
            {
                for (int i = 1; i < 6; i++)
                {
                    auto entity = FlexECS::Scene::GetEntityByName("Enemy Healthbar Preview " + std::to_string(i));
                    entity.GetComponent<Transform>()->is_active = false;
                }
                for (FlexECS::Entity& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Sprite, SpeedBarSlotTarget>())
                {
                    entity.GetComponent<Transform>()->is_active = false;
                }

                // apply the move
                std::vector<_Character*> targets;
                for (int i = 0; i < battle.current_move->effect.size(); i++) {
                    if (battle.current_move->target[i] == "ALL_ENEMIES")
                    {
                        for (auto& character : battle.drifters_and_enemies)
                        {
                            if (character.is_alive && character.character_id > 2)
                            {
                                targets.push_back(&character);
                            }
                        }
                    }
                    else if (battle.current_move->target[i] == "ADJACENT_ENEMIES")
                    {
                        for (auto& character : battle.drifters_and_enemies)
                        {
                            if (character.is_alive && (character.current_slot == battle.target_num - 1 && character.character_id > 2 ||
                                character.current_slot == battle.target_num && character.character_id > 2 || character.current_slot == battle.target_num + 1 && character.character_id > 2))
                            {
                                targets.push_back(&character);
                            }
                        }
                    }
                    else if (battle.current_move->target[i] == "NEXT_ENEMY")
                    {
                        for (auto& character : battle.drifters_and_enemies)
                        {
                            if (character.is_alive && character.character_id > 2)
                            {
                                targets.push_back(&character);
                                break;
                            }
                        }
                    }
                    else if (battle.current_move->target[i] == "SINGLE_ENEMY")
                    {
                        for (auto& character : battle.drifters_and_enemies)
                        {
                            if (character.is_alive && character.current_slot == battle.target_num && character.character_id > 2)
                            {
                                targets.push_back(&character);
                            }
                        }
                    }
                    else if (battle.current_move->target[i] == "ALL_ALLIES")
                    {
                        for (auto& character : battle.drifters_and_enemies)
                        {
                            if (character.is_alive && character.character_id <= 2)
                            {
                                targets.push_back(&character);
                            }
                        }
                    }
                    else if (battle.current_move->target[i] == "NEXT_ALLY")
                    {
                        for (auto& character : battle.drifters_and_enemies)
                        {
                            if (character.is_alive && character.character_id <= 2 && character.character_id != battle.current_character->character_id)
                            {
                                targets.push_back(&character);
                                break;
                            }
                        }
                    }
                    else if (battle.current_move->target[i] == "SINGLE_ALLY")
                    {
                        if (battle.current_move->target[0] == "ALL_ALLIES" || battle.current_move->target[0] == "NEXT_ALLY" || battle.current_move->target[0] == "SINGLE_ALLY" || battle.current_move->target[0] == "SELF") //fizzles if not targeting ally
                        {
                            for (auto& character : battle.drifters_and_enemies)
                            {
                                if (character.is_alive && character.current_slot == battle.target_num && character.character_id <= 2)
                                {
                                    targets.push_back(&character);
                                }
                            }
                        }
                        //else targets.push_back(*(battle.current_character)); //defaults to targeting self
                    }
                    else if (battle.current_move->target[i] == "ALL")
                    {
                        for (auto& character : battle.drifters_and_enemies)
                        {
                            if (character.is_alive)
                                targets.push_back(&character);
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

                            if (battle.god_mode)
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
                                final_damage *= 10;
                                character->current_health -= final_damage;
                            }
                            else if (character->shield_buff_duration > 0)
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
                            character->speed_change -= battle.current_move->value[i];
                            /*if (character->current_speed - battle.current_move->value[i] < 0)
                            {
                                character->current_speed = 0;
                            }
                            else character->current_speed -= battle.current_move->value[i];*/
                        }
                    }
                    else if (battle.current_move->effect[i] == "Speed_Down")
                    {
                        for (auto character : targets)
                        {
                            character->speed_change += battle.current_move->value[i];
                            /*for (auto character : targets)
                            {
                                character->current_speed += battle.current_move->value[i];
                            }*/
                        }
                    }
                    else if (battle.current_move->effect[i] == "Attack_Up")
                    {
                        for (auto character : targets)
                        {
                            character->attack_buff_duration += battle.current_move->value[i];
                            if (character->attack_buff_duration > battle.current_move->value[i])
                            {
                                character->attack_buff_duration = battle.current_move->value[i];
                            }
                        }
                    }
                    else if (battle.current_move->effect[i] == "Attack_Down")
                    {
                        for (auto character : targets)
                        {
                            if (character->protect_buff_duration > 0) continue;
                            else 
                            {
                                character->attack_debuff_duration += battle.current_move->value[i];
                                if (character->attack_debuff_duration > battle.current_move->value[i])
                                {
                                    character->attack_debuff_duration = battle.current_move->value[i];
                                }
                            }
                        }
                    }
                    else if (battle.current_move->effect[i] == "Stun")
                    {
                        for (auto character : targets)
                        {
                            if (character->protect_buff_duration > 0) continue;
                            else
                            {
                                character->stun_debuff_duration += battle.current_move->value[i];
                                if (character->stun_debuff_duration > battle.current_move->value[i])
                                {
                                    character->stun_debuff_duration = battle.current_move->value[i];
                                }
                            }
                        }
                    }
                    else if (battle.current_move->effect[i] == "Shield")
                    {
                        for (auto character : targets)
                        {
                            character->shield_buff_duration += battle.current_move->value[i];
                            if (character->shield_buff_duration > battle.current_move->value[i])
                            {
                                character->shield_buff_duration = battle.current_move->value[i];
                            }
                        }
                    }
                    else if (battle.current_move->effect[i] == "Protect")
                    {
                        for (auto character : targets)
                        {
                            character->protect_buff_duration += battle.current_move->value[i];
                            if (character->protect_buff_duration > battle.current_move->value[i])
                            {
                                character->protect_buff_duration = battle.current_move->value[i];
                            }
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

                //add speed based on move used
                battle.current_character->current_speed = battle.current_move->speed + battle.current_character->speed_change;
                battle.current_character->speed_change = 0;
                //apply player attack animation based on move used
                auto& current_character_animator = *FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(battle.current_character->current_slot + 1) ).GetComponent<Animator>();
                switch (battle.move_num)
                {
                case 1:
                    switch (battle.current_character->character_id)
                    {
                    case 1:
                        current_character_animator.spritesheet_handle =
                            FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Attack_Anim_Sheet.flxspritesheet)");
                        break;
                    case 2:
                        current_character_animator.spritesheet_handle =
                            FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Attack_Anim_Sheet.flxspritesheet)");
                        break;
                    }
                    break;
                case 2:
                    switch (battle.current_character->character_id)
                    {
                    case 1:
                        current_character_animator.spritesheet_handle =
                            FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Attack_Anim_Sheet.flxspritesheet)");
                        break;
                    case 2:
                        current_character_animator.spritesheet_handle =
                            FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Attack_Anim_Sheet.flxspritesheet)");
                        break;
                    }
                    break;
                case 3:
                    switch (battle.current_character->character_id)
                    {
                    case 1:
                        current_character_animator.spritesheet_handle =
                            FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Ult_Anim_Sheet.flxspritesheet)");
                        break;
                    case 2:
                        current_character_animator.spritesheet_handle =
                            FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Ult_Anim_Sheet.flxspritesheet)");

                        break;
                    }
                    break;
                }

                /*float animation_time =
                    FLX_ASSET_GET(Asset::Spritesheet, FLX_STRING_GET(current_character_animator.spritesheet_handle))
                    .total_frame_time;*/

                current_character_animator.should_play = true;
                current_character_animator.is_looping = false;
                current_character_animator.return_to_default = true;
                current_character_animator.frame_time = 0.f;
                current_character_animator.current_frame = 0;

                // Special delay for renko
                if (battle.current_character->character_id == 1)
                {
                  switch (battle.move_num)
                  {
                  case 3: // Renko ult
                    battle.disable_input_timer += 1.f;
                    break;

                  default:
                    break;
                  }
                }

                // Special delay for grace
                if (battle.current_character->character_id == 2)
                {
                  switch (battle.move_num)
                  {
                  case 1:
                    battle.disable_input_timer -= 0.5f;
                    break;

                  case 2:
                    battle.disable_input_timer -= 0.5f;
                    break;

                  case 3:
                      battle.disable_input_timer += 0.5f;
                      break;

                  default:
                    break;
                  }
                }

                battle.disable_input_timer += 1.f;
            }
            else //resolve all move effects + play attack animation
            {
                //apply the move
                std::vector<_Character*> targets;
                for (int i = 0; i < battle.current_move->effect.size(); i++) 
                {
                    if (battle.current_move->target[i] == "ALL_ENEMIES")
                    {
                        for (auto& character : battle.drifters_and_enemies)
                        {
                            if (character.is_alive && character.character_id <= 2)
                            {
                                targets.push_back(&character);
                            }
                        }
                    }
                    else if (battle.current_move->target[i] == "ADJACENT_ENEMIES")
                    {
                        for (auto& character : battle.drifters_and_enemies)
                        {
                            if (character.is_alive && (character.current_slot == battle.target_num - 1 && character.character_id <= 2 ||
                                character.current_slot == battle.target_num && character.character_id <= 2 || character.current_slot == battle.target_num + 1 && character.character_id <= 2))
                            {
                                targets.push_back(&character);
                            }
                        }
                    }
                    else if (battle.current_move->target[i] == "NEXT_ENEMY")
                    {
                        for (auto& character : battle.drifters_and_enemies)
                        {
                            if (character.is_alive && character.character_id <= 2)
                            {
                                targets.push_back(&character);
                                break;
                            }
                        }
                    }
                    else if (battle.current_move->target[i] == "SINGLE_ENEMY")
                    {
                        for (auto& character : battle.drifters_and_enemies)
                        {
                            if (character.is_alive && character.current_slot == battle.target_num && character.character_id <= 2)
                            {
                                targets.push_back(&character);
                            }
                        }
                    }
                    else if (battle.current_move->target[i] == "ALL_ALLIES")
                    {
                        for (auto& character : battle.drifters_and_enemies)
                        {
                            if (character.is_alive && character.character_id > 2)
                            {
                                targets.push_back(&character);
                            }
                        }
                    }
                    else if (battle.current_move->target[i] == "NEXT_ALLY")
                    {
                        for (auto& character : battle.drifters_and_enemies)
                        {
                            if (character.is_alive && character.character_id > 2 && character.character_id != battle.current_character->character_id)
                            {
                                targets.push_back(&character);
                                break;
                            }
                        }
                    }
                    else if (battle.current_move->target[i] == "SINGLE_ALLY")
                    {
                        if (battle.current_move->target[0] == "ALL_ALLIES" || battle.current_move->target[0] == "NEXT_ALLY" || battle.current_move->target[0] == "SINGLE_ALLY" || battle.current_move->target[0] == "SELF") //fizzles if not targeting ally
                        {
                            for (auto& character : battle.drifters_and_enemies)
                            {
                                if (character.is_alive && character.current_slot == battle.target_num && character.character_id > 2)
                                {
                                    targets.push_back(&character);
                                }
                            }
                        }
                    }
                    else if (battle.current_move->target[i] == "ALL")
                    {
                        for (auto& character : battle.drifters_and_enemies)
                        {
                            if (character.is_alive)
                                targets.push_back(&character);
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
                            if (battle.god_mode)
                            {
                                continue;
                            }
                            else if (character->shield_buff_duration > 0)
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
                            character->speed_change -= battle.current_move->value[i];
                            /*if (character->current_speed - battle.current_move->value[i] < 0)
                            {
                                character->current_speed = 0;
                            }
                            else character->current_speed -= battle.current_move->value[i];*/
                        }
                    }
                    else if (battle.current_move->effect[i] == "Speed_Down")
                    {
                        for (auto character : targets)
                        {
                            character->speed_change += battle.current_move->value[i];
                            /*for (auto character : targets)
                            {
                                character->current_speed += battle.current_move->value[i];
                            }*/
                        }
                    }
                    else if (battle.current_move->effect[i] == "Attack_Up")
                    {
                        for (auto character : targets)
                        {
                            character->attack_buff_duration += battle.current_move->value[i];
                            if (character->attack_buff_duration > battle.current_move->value[i])
                            {
                                character->attack_buff_duration = battle.current_move->value[i];
                            }
                        }
                    }
                    else if (battle.current_move->effect[i] == "Attack_Down")
                    {
                        for (auto character : targets)
                        {
                            if (character->protect_buff_duration > 0) continue;
                            else
                            {
                                character->attack_debuff_duration += battle.current_move->value[i];
                                if (character->attack_debuff_duration > battle.current_move->value[i])
                                {
                                    character->attack_debuff_duration = battle.current_move->value[i];
                                }
                            }
                        }
                    }
                    else if (battle.current_move->effect[i] == "Stun")
                    {
                        for (auto character : targets)
                        {
                            if (character->protect_buff_duration > 0) continue;
                            else
                            {
                                character->stun_debuff_duration += battle.current_move->value[i];
                                if (character->stun_debuff_duration > battle.current_move->value[i])
                                {
                                    character->stun_debuff_duration = battle.current_move->value[i];
                                }
                            }
                        }
                    }
                    else if (battle.current_move->effect[i] == "Shield")
                    {
                        for (auto character : targets)
                        {
                            character->shield_buff_duration += battle.current_move->value[i];
                            if (character->shield_buff_duration > battle.current_move->value[i])
                            {
                                character->shield_buff_duration = battle.current_move->value[i];
                            }
                        }
                    }
                    else if (battle.current_move->effect[i] == "Protect")
                    {
                        for (auto character : targets)
                        {
                            character->protect_buff_duration += battle.current_move->value[i];
                            if (character->protect_buff_duration > battle.current_move->value[i])
                            {
                                character->protect_buff_duration = battle.current_move->value[i];
                            }
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

                //update speed based on move used
                battle.current_character->current_speed = battle.current_move->speed + battle.current_character->speed_change;
                battle.current_character->speed_change = 0;

                // Temporarily move the character if targeting enemy
                if (battle.current_move->target[0] == "ALL_ALLIES" || battle.current_move->target[0] == "NEXT_ALLY" || battle.current_move->target[0] == "SINGLE_ALLY" || battle.current_move->target[0] == "SELF")
                {
                    // Do nothing if enemy targets its own allies
                }
                else
                {
                    FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(battle.current_character->current_slot + 1)).GetComponent<Position>()->position =
                        battle.sprite_slot_positions[battle.initial_target->current_slot] + Vector3{ 120, 0, 0 };;

                    FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(battle.current_character->current_slot + 1)).GetComponent<ZIndex>()->z = 50;
                }

                // play the attack animation
                auto& current_character_animator = *FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(battle.current_character->current_slot + 1)).GetComponent<Animator>();
                switch (battle.current_character->character_id)
                {
                case 3:
                    current_character_animator.spritesheet_handle =
                        FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_01_Attack_Anim_Sheet.flxspritesheet)");
                    break;
                case 4:
                    current_character_animator.spritesheet_handle =
                        FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_02_Attack_Anim_Sheet.flxspritesheet)");
                    break;
                case 5:
                    if (battle.move_num == 3)
                    {
                        current_character_animator.spritesheet_handle =
                            FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Ult_Anim_Sheet.flxspritesheet)");
                    }
                    else
                    {
                        current_character_animator.spritesheet_handle =
                            FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Attack_Anim_Sheet.flxspritesheet)");
                    }
                    break;
                }
                /*float animation_time =
                    FLX_ASSET_GET(Asset::Spritesheet, FLX_STRING_GET(current_character_animator.spritesheet_handle))
                    .total_frame_time;*/

                current_character_animator.should_play = true;
                current_character_animator.is_looping = false;
                current_character_animator.return_to_default = true;
                current_character_animator.frame_time = 0.f;
                current_character_animator.current_frame = 0;
                battle.force_disable_combat_camera = true;

                // Delay for machine gun 
                if (battle.current_character->character_id == 3 || battle.current_character->character_id == 4)
                    battle.disable_input_timer += 1.5f;
                else battle.disable_input_timer += 0.5f;
            }
        }

        #if 0
        {
        // Camera Animation
        if (battle.current_move->target[0] == "ALL_ENEMIES" ||
            battle.current_move->target[0] == "ADJACENT_ENEMIES" ||
            battle.current_move->target[0] == "NEXT_ENEMY" ||
            battle.current_move->target[0] == "SINGLE_ENEMY")
        {
            // First, calculate the attacker and defender positions based on character id.
            FlexECS::Entity attacker, defender;
            if (battle.current_character->character_id > 2)
            {
                attacker = FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(battle.current_character->current_slot + 1));
                defender = FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(battle.initial_target->current_slot + 1));
            }
            else
            {
                attacker = FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(battle.current_character->current_slot + 1));
                defender = FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(battle.initial_target->current_slot + 1));
            }

            Vector3 attackerPos = attacker.GetComponent<Position>()->position;
            Vector3 defenderPos = defender.GetComponent<Position>()->position;
            Vector3 centerPos = (attackerPos + defenderPos) * 0.5f;

            // Compute a normalized direction from center towards defender and offset for jerk.
            Vector3 direction = (defenderPos - centerPos).Normalize();
            float jerkDistance = 2.0f; // Adjust as needed.
            Vector3 jerkPos = centerPos + direction * jerkDistance;

            // A static container to hold active camera animations (lambdas).
            static std::vector<std::function<bool(float)>> animations;

            // For each camera, create a lambda that tracks its animation progress.
            for (auto& elem : FlexECS::Scene::GetActiveScene()->CachedQuery<Position, Camera>())
            {
                auto posComp = elem.GetComponent<Position>();
                Vector3 originalPos = posComp->position;

                // Initialize per-animation state variables.
                int stage = 0;                      // 0: move to center, 1: jerk toward defender, 2: jerk back, 3: return to original.
                float elapsed = 0.0f;
                float durations[4] = { 1.0f, 0.2f, 0.2f, 1.0f };

                // Create a lambda that will be updated every frame.
                auto anim = [=, &posComp](float deltaTime) mutable -> bool
                {
                    // When all stages are done, return true to signal removal.
                    if (stage >= 4)
                        return true;

                    // Update the elapsed time.
                    elapsed += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
                    float t = std::min(elapsed / durations[stage], 1.0f);
                    Vector3 startPos, targetPos;

                    // Determine the start and target positions for the current stage.
                    switch (stage)
                    {
                    case 0:
                        startPos = originalPos;
                        targetPos = centerPos;
                        break;
                    case 1:
                        startPos = centerPos;
                        targetPos = jerkPos;
                        break;
                    case 2:
                        startPos = jerkPos;
                        targetPos = centerPos;
                        break;
                    case 3:
                        startPos = centerPos;
                        targetPos = originalPos;
                        break;
                    }

                    // Update the camera position.
                    posComp->position = Lerp(startPos, targetPos, t);

                    // If the current stage is complete, move to the next one.
                    if (t >= 1.0f)
                    {
                        stage++;
                        elapsed = 0.0f;
                    }

                    // Return whether the animation is finished.
                    return (stage >= 4);
                };

                // Add the lambda to the animations container.
                animations.push_back(anim);
            }

            // Update all active animations. Remove any that have finished.
            for (auto it = animations.begin(); it != animations.end(); )
            {
                if ((*it)(Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime()))
                    it = animations.erase(it);
                else
                    ++it;
            }
        }

        }
        #endif
        if (battle.is_player_turn)
        {// Temporarily move the character if targeting enemy
            if (battle.current_move->target[0] == "ALL_ALLIES" || battle.current_move->target[0] == "NEXT_ALLY" || battle.current_move->target[0] == "SINGLE_ALLY" || battle.current_move->target[0] == "SELF")
            {
                // If targeting allies, does nothing
            }
            else
            {
                float time_elapsed = battle.disable_input_timer;
                if (time_elapsed > 1.0f)
                {
                    time_elapsed = 1.0f;
                }
                float percent_moved = ((1.0f - time_elapsed) / 1.0f) * 3;
                if (percent_moved > 1.0f)
                {
                    percent_moved = 1.0f;
                }
                Vector3 new_position = battle.sprite_slot_positions[battle.initial_target->current_slot + 2] + Vector3{ -120, 0, 0 };

                Vector3 original_position = battle.sprite_slot_positions[battle.current_character->current_slot];

                Vector3 interpolated_position = original_position * (1.0f - percent_moved) + new_position * percent_moved;

                FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(battle.current_character->current_slot + 1)).GetComponent<Position>()->position = interpolated_position;

                FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(battle.current_character->current_slot + 1)).GetComponent<ZIndex>()->z = 50;
            }
        }
        else
        {
            if (battle.current_move->target[0] == "ALL_ALLIES" || battle.current_move->target[0] == "NEXT_ALLY" || battle.current_move->target[0] == "SINGLE_ALLY" || battle.current_move->target[0] == "SELF")
            {
                // If targeting allies, does nothing
            }
            else
            {
                float time_elapsed = battle.disable_input_timer;
                if (time_elapsed > 1.0f)
                {
                    time_elapsed = 1.0f;
                }
                float percent_moved = ((1.0f - time_elapsed) / 1.0f) * 3;
                if (percent_moved > 1.0f)
                {
                    percent_moved = 1.0f;
                }
                Vector3 new_position = battle.sprite_slot_positions[battle.initial_target->current_slot] + Vector3{ 120, 0, 0 };

                Vector3 original_position = battle.sprite_slot_positions[battle.current_character->current_slot + 2];

                Vector3 interpolated_position = original_position * (1.0f - percent_moved) + new_position * percent_moved;

                FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(battle.current_character->current_slot + 1)).GetComponent<Position>()->position = interpolated_position;

                FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(battle.current_character->current_slot + 1)).GetComponent<ZIndex>()->z = 50;
            }
        }

        battle.enable_combat_camera = true;
        battle.force_disable_combat_camera = false;

        //let attack animation play finish
        if (battle.disable_input_timer > 0.f)
        {
          return;
        }
        
        //update health bar
        Update_Character_Status();

        if (battle.is_player_turn) //secondary animation of enemies getting hit
        {
            //play attack audio
            switch (battle.move_num)
            {
            case 1:
                switch (battle.current_character->character_id)
                {
                case 1:
                    FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                        FLX_STRING_NEW(R"(/audio/generic attack.mp3)");
                    FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                    break;
                case 2:
                    FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                        FLX_STRING_NEW(R"(/audio/generic attack.mp3)");
                    FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                    break;
                }
                break;
            case 2:
                switch (battle.current_character->character_id)
                {
                case 1:
                    FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                        FLX_STRING_NEW(R"(/audio/generic attack.mp3)");
                    FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                    break;
                case 2:
                    FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                        FLX_STRING_NEW(R"(/audio/generic attack.mp3)");
                    FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                    break;
                }
                break;
            case 3:
                switch (battle.current_character->character_id)
                {
                case 1:
                    FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                        FLX_STRING_NEW(R"(/audio/chrono gear activation (SCI-FI-POWER-UP_GEN-HDF-20770).wav)");
                    Application::MessagingSystem::Send("ActivatePseudoColorDistortion", true);
                    FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                    break;
                case 2:
                    FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                        FLX_STRING_NEW(R"(/audio/Big Hammer Ground Hit_1.wav)");
                    FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                    Application::MessagingSystem::Send("Spawn VFX", std::tuple<std::vector<FlexECS::Entity>, std::string, Vector3, Vector3>
                    { {FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(battle.current_character->current_slot + 1))}, VFXPresets.vfx_grace_ult, {-75.0f, -100.0f, 0.0f}, {2.0f,2.0f,2.0f} });
                    Application::MessagingSystem::Send("ActivatePseudoColorDistortion", true);
                    break;
                }
                break;
            }

            //play damage animation for enemies
            float animation_time = .0f;
            if (battle.current_move->target[0] == "ADJACENT_ENEMIES" || battle.current_move->target[0] == "ALL_ENEMIES")
            {
              std::vector<FlexECS::Entity> hit_entities;
                for (auto& character : battle.drifters_and_enemies)
                {
                    if (character.is_alive && character.character_id > 2)
                    {
                        if (battle.current_move->target[0] == "ADJACENT_ENEMIES")
                        {
                            if (character.current_slot == battle.target_num - 1 || character.current_slot == battle.target_num || character.current_slot == battle.target_num + 1)
                            {
                                auto target_entity = FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(character.current_slot + 1));
                                auto& target_animator = *target_entity.GetComponent<Animator>();
                                switch (character.character_id)
                                {
                                case 3:
                                    target_animator.spritesheet_handle =
                                        FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_01_Hurt_Anim_Sheet.flxspritesheet)");
                                    break;
                                case 4:
                                    target_animator.spritesheet_handle =
                                        FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_02_Hurt_Anim_Sheet.flxspritesheet)");
                                    break;
                                case 5:
                                      target_animator.spritesheet_handle =
                                        FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Hurt_Anim_Sheet.flxspritesheet)");
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

                                hit_entities.push_back(target_entity);
                            }
                        }
                        else
                        {
                            auto target_entity = FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(character.current_slot + 1));
                            auto& target_animator = *target_entity.GetComponent<Animator>();
                            switch (character.character_id)
                            {
                            case 3:
                                target_animator.spritesheet_handle =
                                    FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_01_Hurt_Anim_Sheet.flxspritesheet)");
                                break;
                            case 4:
                                target_animator.spritesheet_handle =
                                    FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_02_Hurt_Anim_Sheet.flxspritesheet)");
                                break;
                            case 5:
                                  target_animator.spritesheet_handle =
                                    FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Hurt_Anim_Sheet.flxspritesheet)");
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

                            hit_entities.push_back(target_entity);
                        }
                    }
                }
                battle.disable_input_timer += 0.5f;

                //Hit VFX
                Application::MessagingSystem::Send("Spawn VFX", std::tuple<std::vector<FlexECS::Entity>, std::string, Vector3, Vector3>
                { {hit_entities}, VFXPresets.vfx_player_attack, {}, {5.0f, 5.0f, 5.0f} });
                Application::MessagingSystem::Send("ActivateChromaticAlteration", true);
                battle.jerk_towards_defender = true;
            }
            else if (battle.current_move->target[0] == "SINGLE_ENEMY" || battle.current_move->target[0] == "NEXT_ENEMY")
            {
                auto target_entity = FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(battle.initial_target->current_slot + 1));
                auto& target_animator = *target_entity.GetComponent<Animator>();
                switch (battle.initial_target->character_id)
                {
                case 3:
                    target_animator.spritesheet_handle =
                        FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_01_Hurt_Anim_Sheet.flxspritesheet)");
                    break;
                case 4:
                    target_animator.spritesheet_handle =
                        FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_02_Hurt_Anim_Sheet.flxspritesheet)");
                    break;
                case 5:
                     target_animator.spritesheet_handle =
                        FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Hurt_Anim_Sheet.flxspritesheet)");
                    break;
                }

                /*animation_time =
                    FLX_ASSET_GET(Asset::Spritesheet, FLX_STRING_GET(target_animator.spritesheet_handle))
                    .total_frame_time;*/

                 
                target_animator.should_play = true;
                target_animator.is_looping = false;
                target_animator.return_to_default = true;
                target_animator.frame_time = 0.f;
                target_animator.current_frame = 0;

                battle.disable_input_timer += 0.5f;

                //Hit VFX
                Application::MessagingSystem::Send("Spawn VFX", std::tuple<std::vector<FlexECS::Entity>, std::string, Vector3, Vector3>
                { {target_entity}, VFXPresets.vfx_player_attack, {}, { 5.0f, 5.0f, 5.0f } });
                Application::MessagingSystem::Send("ActivateChromaticAlteration", true);
                battle.jerk_towards_defender = true;
            }
        }
        else //secondary animation of players getting hit
        {
            //play attack audio
            switch (battle.current_character->character_id)
            {
            case 3:
                FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                    FLX_STRING_NEW(R"(/audio/robot shooting.mp3)");
                FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                break;
            case 4:
                FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                    FLX_STRING_NEW(R"(/audio/robot shooting.mp3)");
                FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                break;
            case 5:
                if (battle.move_num == 3)
                {
                    FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                        FLX_STRING_NEW(R"(/audio/jack attack (SCI-FI-IMPACT_GEN-HDF-20694).wav)");
                    FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                }
                else
                {
                    FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                        FLX_STRING_NEW(R"(/audio/jack attack (SCI-FI-IMPACT_GEN-HDF-20694).wav)");
                    FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                }
                break;
            }

            //play damage animation for players
            float animation_time = .0f;
            if (battle.current_move->target[0] == "ADJACENT_ENEMIES" || battle.current_move->target[0] == "ALL_ENEMIES")
            {
              std::vector<FlexECS::Entity> hit_entities;
                for (auto& character : battle.drifters_and_enemies)
                {
                    if (character.is_alive && character.character_id <= 2)
                    {
                        auto target_entity = FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(character.current_slot + 1));;
                        auto& target_animator = *target_entity.GetComponent<Animator>();
                        switch (character.character_id)
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

                        // Screen Shake
                        Application::MessagingSystem::Send("CameraShakeStart", std::pair<double, double>{ 0.5, 3 });
                        battle.jerk_towards_defender = true;

                        hit_entities.push_back(target_entity);
                    }
                }
                battle.disable_input_timer += animation_time - 0.1f;// +1.f;

                if (battle.current_character->character_id == 5)
                {
                    if (battle.move_num == 3)
                    {
                        Application::MessagingSystem::Send("Spawn VFX", std::tuple<std::vector<FlexECS::Entity>, std::string, Vector3, Vector3>
                        { {hit_entities}, VFXPresets.vfx_jack_ult, {}, { 2.0f, 2.0f, 2.0f } });
                        Application::MessagingSystem::Send("ActivateJackUlt", true);
                    }
                }
                else
                {
                    Application::MessagingSystem::Send("Spawn VFX", std::tuple<std::vector<FlexECS::Entity>, std::string, Vector3, Vector3>
                    { hit_entities, VFXPresets.vfx_enemy_attack1, {}, { 2.0f, 2.0f, 2.0f } });
                }
            }
            else if (battle.current_move->target[0] == "SINGLE_ENEMY" || battle.current_move->target[0] == "NEXT_ENEMY")
            {
                auto target_entity = FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(battle.initial_target->current_slot + 1));;
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

                battle.disable_input_timer += animation_time - 0.1f;// +1.f;
        
                // Screen Shake
                Application::MessagingSystem::Send("CameraShakeStart", std::pair<double, double>{ 0.5, 3 });
                Application::MessagingSystem::Send("ActivateChromaticAlteration", true);
                battle.jerk_towards_defender = true;

                //Hit VFX
                //Specific check for Jack's attack
                if (battle.current_character->character_id == 5)
                {
                    if (battle.move_num == 3)
                    {
                        Application::MessagingSystem::Send("Spawn VFX", std::tuple<std::vector<FlexECS::Entity>, std::string, Vector3, Vector3>
                        { {target_entity}, VFXPresets.vfx_jack_ult, {}, { 2.0f, 2.0f, 2.0f } });
                        Application::MessagingSystem::Send("ActivateJackUlt", true);
                    }
                }
                else
                {
                  Application::MessagingSystem::Send("Spawn VFX", std::tuple<std::vector<FlexECS::Entity>, std::string, Vector3, Vector3>
                  { {target_entity}, VFXPresets.vfx_enemy_attack1, {}, { 2.0f, 2.0f, 2.0f } });
                }
            }
        }

        battle.start_of_turn = false;
        battle.move_select = false;
        battle.move_resolution = false;
        battle.speedbar_animating = true;
        battle.end_of_turn = true;

        battle.change_phase = true;
    }

    void End_Of_Turn()
    {
        // wait for all prior animations to stop playing before continuing with phase change
        if (battle.disable_input_timer > 0.f)
        {
          return;
        }

        //tutorial code
        if (battle.is_tutorial_running)
        {
          if (Input::AnyKeyDown())
          {
              battle.tutorial_info++;
          }
        }
        if (battle.is_tutorial && battle.tutorial_info < 5)
        {
            return;
        }

        //upon start of end of turn, checks for any deaths and plays death animation accordingly
        if (battle.change_phase)
        {
            Log::Debug("End Turn");
            battle.change_phase = false;

            FlexECS::Scene::GetEntityByName("move_used_textbox").GetComponent<Transform>()->is_active = false;
            FlexECS::Scene::GetEntityByName("move_used_text").GetComponent<Transform>()->is_active = false;
            FLX_STRING_GET(FlexECS::Scene::GetEntityByName("move_used_text").GetComponent<Text>()->text) = "";

            //death animation
            float animation_time = .0f;
            for (auto& character : battle.drifters_and_enemies)
            {
                // check if any characters are dead
                if (character.is_alive && character.current_health <= 0)
                {
                    // update state
                    character.is_alive = false;
                    character.current_health = 0;

                    // remove from speed bar
                    battle.speed_bar.erase(
                      std::remove(battle.speed_bar.begin(), battle.speed_bar.end(), &character), battle.speed_bar.end()
                    );

                    character.attack_buff_duration = 0;
                    character.attack_debuff_duration = 0;
                    character.stun_debuff_duration = 0;
                    character.shield_buff_duration = 0;
                    character.protect_buff_duration = 0;


                    if (character.character_id > 2)
                    {
                        //FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(character.current_slot + 1)).GetComponent<Transform>()->is_active = false;
                        FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(character.current_slot + 1) + " Name").GetComponent<Transform>()->is_active = false;
                        FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(character.current_slot + 1) + " Healthbar").GetComponent<Transform>()->is_active = false;
                        FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(character.current_slot + 1) + " Current Healthbar").GetComponent<Transform>()->is_active = false;
                        FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(character.current_slot + 1) + " Buff 1").GetComponent<Transform>()->is_active = false;
                        FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(character.current_slot + 1) + " Buff 2").GetComponent<Transform>()->is_active = false;
                        FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(character.current_slot + 1) + " Buff 3").GetComponent<Transform>()->is_active = false;
                        FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(character.current_slot + 1) + " Buff 4").GetComponent<Transform>()->is_active = false;
                        FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(character.current_slot + 1) + " Buff 5").GetComponent<Transform>()->is_active = false;

                        auto& target_animator = *FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(character.current_slot + 1)).GetComponent<Animator>();
                        switch (character.character_id)
                        {
                        case 3:
                            target_animator.spritesheet_handle =
                                FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_01_Death_Anim_Sheet.flxspritesheet)");
                            // FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                            //   FLX_STRING_NEW(R"(/audio/robot death.mp3)");
                            // FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                            break;
                        case 4:
                            target_animator.spritesheet_handle =
                                FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_02_Death_Anim_Sheet.flxspritesheet)");
                            // FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                            //   FLX_STRING_NEW(R"(/audio/robot death.mp3)");
                            // FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
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
                        target_animator.return_to_default = false;
                        target_animator.frame_time = 0.f;
                        target_animator.current_frame = 0;


                        switch (character.character_id)
                        {
                        case 3:
                            FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                                FLX_STRING_NEW(R"(/audio/Robot Destroyed SFX_1.wav)");
                            FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                            break;
                        case 4:
                            FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                                FLX_STRING_NEW(R"(/audio/Robot Destroyed SFX_1.wav)");
                            FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                            break;
                        case 5:
                            //FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
                               // FLX_STRING_NEW(R"(/audio/jack attack (SCI-FI-IMPACT_GEN-HDF-20694).wav)");
                           // FlexECS::Scene::GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
                            break;
                        }

                    }
                    else
                    {
                        //FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(character.current_slot + 1)).GetComponent<Transform>()->is_active = false;
                        FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(character.current_slot + 1) + " Name").GetComponent<Transform>()->is_active = false;
                        FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(character.current_slot + 1) + " Healthbar").GetComponent<Transform>()->is_active = false;
                        FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(character.current_slot + 1) + " Current Healthbar").GetComponent<Transform>()->is_active = false;
                        FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(character.current_slot + 1) + " Buff 1").GetComponent<Transform>()->is_active = false;
                        FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(character.current_slot + 1) + " Buff 2").GetComponent<Transform>()->is_active = false;
                        FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(character.current_slot + 1) + " Buff 3").GetComponent<Transform>()->is_active = false;
                        FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(character.current_slot + 1) + " Buff 4").GetComponent<Transform>()->is_active = false;
                        FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(character.current_slot + 1) + " Buff 5").GetComponent<Transform>()->is_active = false;
                        auto& target_animator = *FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(character.current_slot + 1)).GetComponent<Animator>();
                        switch (character.character_id)
                        {
                        case 1:
                            target_animator.spritesheet_handle =
                                FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Death_Anim_Sheet.flxspritesheet)");
                            break;
                        case 2:
                            target_animator.spritesheet_handle =
                                FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Death_Anim_Sheet.flxspritesheet)");
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
                        target_animator.return_to_default = false;
                        target_animator.frame_time = 0.f;
                        target_animator.current_frame = 0;
                    }
                }
            }
            battle.disable_input_timer += animation_time;// + 1.f;

            battle.current_character->previous_health = battle.current_character->current_health;
            //reset position, then delay a bit
                if (battle.current_character->character_id > 2)
                {
                    FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(battle.current_character->current_slot + 1)).GetComponent<Position>()->position = 
                        battle.sprite_slot_positions[battle.current_character->current_slot + 2];

                    FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(battle.current_character->current_slot + 1)).GetComponent<ZIndex>()->z = 10;
                }
                else
                {
                    switch (battle.current_character->current_slot + 1)
                    {
                    case 1:
                        FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(battle.current_character->current_slot + 1)).GetComponent<Position>()->position =
                            battle.sprite_slot_positions[battle.current_character->current_slot] + Vector3{100, 90, 0};
                        break;
                    case 2:
                        FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(battle.current_character->current_slot + 1)).GetComponent<Position>()->position =
                            battle.sprite_slot_positions[battle.current_character->current_slot] + Vector3{100, 10, 0};
                        break;
                    }


                    FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(battle.current_character->current_slot + 1)).GetComponent<ZIndex>()->z = 10;
                }
            if (battle.disable_input_timer <= 0.f) battle.disable_input_timer += 1.f;
        }

        //let death animation play finish
        if (battle.disable_input_timer > .0f)
        {
            return;
        }

        //set loop death animation for dead enemies
        for (auto& character : battle.drifters_and_enemies)
        {
            // check if any characters are dead
            if (!character.is_alive && character.character_id > 2)
            {
                auto& target_animator = *FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(character.current_slot + 1)).GetComponent<Animator>();
                switch (character.character_id)
                {
                case 3:
                    target_animator.spritesheet_handle =
                        FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_01_Death_Loop_Sheet.flxspritesheet)");
                    break;
                case 4:
                    target_animator.spritesheet_handle =
                        FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_02_Death_Loop_Sheet.flxspritesheet)");
                    break;
                case 5:
                    // goto jack death cutscene
                    break;
                }
                target_animator.should_play = true;
                target_animator.is_looping = true;
                target_animator.return_to_default = false;
                target_animator.frame_time = 0.f;
                //target_animator.current_frame = 0;
            }
        }

        //check for game over
        int player_count = 0;
        int enemy_count = 0;
        for (auto& character : battle.drifters_and_enemies)
        {
            if (character.is_alive && character.character_id <= 2)
            {
                player_count++;
            }
            else if (character.is_alive && character.character_id > 2)
            {
                enemy_count++;
            }
        }

        if (!player_count)
        {
            Lose_Battle();
            return;
        }
        else if (!enemy_count)
        {
            Win_Battle();
            return;
        }

        //change phase
        battle.start_of_turn = true;
        battle.move_select = false;
        battle.move_resolution = false;
        battle.end_of_turn = false;

        battle.change_phase = true;
    }

    void Win_Battle()
    {
        battle.is_win = true;

        // A bit lame, but need to find by name to set, like the old Unity days
        FlexECS::Entity overlay = FlexECS::Scene::GetEntityByName("Combat Overlay");
        overlay.GetComponent<Transform>()->is_active = true;
        overlay.GetComponent<Animator>()->spritesheet_handle = FLX_STRING_NEW(R"(/images/Screen_Overlays/Victory/Victory_Sprite_Sheet.flxspritesheet)");
        overlay.GetComponent<Animator>()->default_spritesheet_handle = FLX_STRING_NEW(R"(/images/Screen_Overlays/Victory/Victory_Sprite_Sheet.flxspritesheet)"); // Dont think this is needed but laze, in case.
        overlay.GetComponent<Animator>()->should_play = true;
        overlay.GetComponent<Animator>()->return_to_default = false;
        overlay.GetComponent<Animator>()->current_frame = 0;
        FlexECS::Scene::GetEntityByName("Background Music").GetComponent<Audio>()->should_play = false;
        FlexECS::Scene::GetEntityByName("win audio").GetComponent<Audio>()->audio_file =
            FLX_STRING_NEW(R"(/audio/Win Musical SFX.wav)");
        FlexECS::Scene::GetEntityByName("win audio").GetComponent<Audio>()->should_play = true;
    }

    void Lose_Battle()
    {
      battle.is_lose = true;
      FlexECS::Entity overlay = FlexECS::Scene::GetEntityByName("Combat Overlay");
      overlay.GetComponent<Transform>()->is_active = true;
      overlay.GetComponent<Animator>()->spritesheet_handle = FLX_STRING_NEW(R"(/images/Screen_Overlays/Lose/Lose_Sprite_Sheet.flxspritesheet)");
      overlay.GetComponent<Animator>()->default_spritesheet_handle = FLX_STRING_NEW(R"(/images/Screen_Overlays/Lose/Lose_Sprite_Sheet.flxspritesheet)"); // Dont think this is needed but laze, in case.
      overlay.GetComponent<Animator>()->should_play = true;
      overlay.GetComponent<Animator>()->return_to_default = false;
      overlay.GetComponent<Animator>()->is_looping = false;
      overlay.GetComponent<Animator>()->current_frame = 0;

      FlexECS::Scene::GetEntityByName("Background Music").GetComponent<Audio>()->should_play = false;
      FlexECS::Scene::GetEntityByName("lose audio").GetComponent<Audio>()->audio_file =
          FLX_STRING_NEW(R"(/audio/Lose Musical SFX.wav)");
      FlexECS::Scene::GetEntityByName("lose audio").GetComponent<Audio>()->should_play = true;

      FlexECS::Scene::GetEntityByName("Press any button").GetComponent<Transform>()->is_active = true;

      static float opacity = 1.f;
      float fade_speed = -1.f;
      // Pingpong between opacity 0 and 1
      if (opacity <= 0.f || opacity >= 1.f) 
        fade_speed *= -1;
      opacity += fade_speed * Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
      //FlexECS::Scene::GetEntityByName("Press any button").GetComponent<Text>()->color = opacity;
    }

    void Set_Up_Pause_Menu() {
      #pragma region Load pause menu data
      std::array<std::string, 9> slider_names = {
        "SFX Slider Background",
        "SFX Slider Fill",
        "SFX Knob",
        "BGM Slider Background",
        "BGM Slider Fill",
        "BGM Knob",
        "Master Slider Background",
        "Master Slider Fill",
        "Master Knob"
      };
      for (int i = 0; i < 9; i += 3) {
        FlexECS::Entity tempEntity = FlexECS::Scene::GetEntityByName(slider_names[i]);

        float min_value = tempEntity.GetComponent<Position>()->position.x - tempEntity.GetComponent<Sprite>()->scale.x - 20.f;
        float max_value = tempEntity.GetComponent<Position>()->position.x + tempEntity.GetComponent<Sprite>()->scale.x + 15.f;

        FlexECS::Entity sliderEntity = FlexECS::Scene::GetEntityByName(slider_names[i + 1]);
        sliderEntity.GetComponent<Slider>()->min_position = min_value;
        sliderEntity.GetComponent<Slider>()->max_position = max_value;
        sliderEntity.GetComponent<Slider>()->slider_length = max_value - min_value;
        sliderEntity.GetComponent<Slider>()->original_value = ((FlexECS::Scene::GetEntityByName(slider_names[i + 2]).GetComponent<Position>()->position.x - min_value)) / (max_value - min_value);
        
        // Set Up Volume Positions
        if (i == 0) {
          FlexECS::Scene::GetEntityByName("SFX Knob").GetComponent<Position>()->position.x =
            FMODWrapper::Core::GetGroupVolume(FMODWrapper::Core::CHANNELGROUP::SFX) * (max_value - min_value) + min_value;
        }
        else if (i == 3) {
          FlexECS::Scene::GetEntityByName("BGM Knob").GetComponent<Position>()->position.x =
            FMODWrapper::Core::GetGroupVolume(FMODWrapper::Core::CHANNELGROUP::BGM) * (max_value - min_value) + min_value;
        }
        else if (i == 6) {
          FlexECS::Scene::GetEntityByName("Master Knob").GetComponent<Position>()->position.x = max_value;
        }
      }
      for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<PauseUI, Slider>()) {
        entity.GetComponent<Slider>()->original_scale = entity.GetComponent<Scale>()->scale;
      }
      #pragma endregion
    }

    void Pause_Functionality() {
      for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI>()) {
        bool& state_to_set = entity.GetComponent<Transform>()->is_active;
        if (entity.HasComponent<PauseHoverUI>() || entity.HasComponent<CreditsUI>()) state_to_set = false;
        else state_to_set ^= true;
      }

      battle.is_paused ^= true;
      if (battle.is_paused) {
        battle.active_pause_button = "Resume Button Sprite";
        battle.active_volume_button = "Master Volume Sprite";
        FlexECS::Scene::GetEntityByName("Resume Button Sprite").GetComponent<Transform>()->is_active = true;
      }
    }
    
    void BattleLayer::OnAttach()
    {
      ClearBattleStruct();
      Start_Of_Game();
      Set_Up_Pause_Menu();
    }

    void BattleLayer::OnDetach()
    {
      // Make sure nothing carries over in the way of sound
      FMODWrapper::Core::ForceFadeOut(1.f);
    }

    void BattleLayer::Update()
    {
        FLX_STRING_GET(FlexECS::Scene::GetEntityByName("FPS Display").GetComponent<Text>()->text) = "FPS: " + std::to_string(Application::GetCurrentWindow()->GetFramerateController().GetFPS());
        if (Input::GetKeyDown(GLFW_KEY_F1))
        {
            FlexECS::Scene::GetEntityByName("FPS Display").GetComponent<Transform>()->is_active ^= true;
        }

        bool resume_game = Application::MessagingSystem::Receive<bool>("Resume Game");
        std::pair<std::string, bool> active_pause_sprite = Application::MessagingSystem::Receive<std::pair<std::string, bool>>("Pause Sprite");
        std::pair<std::string, bool> active_volume_sprite = Application::MessagingSystem::Receive<std::pair<std::string, bool>>("Volume Sprite");

        /*bool move_one_click = Application::MessagingSystem::Receive<bool>("MoveOne clicked");
        bool move_two_click = Application::MessagingSystem::Receive<bool>("MoveTwo clicked");
        bool move_three_click = Application::MessagingSystem::Receive<bool>("MoveThree clicked");

        bool target_one_click = Application::MessagingSystem::Receive<bool>("TargetOne clicked");
        bool target_two_click = Application::MessagingSystem::Receive<bool>("TargetTwo clicked");
        bool target_three_click = Application::MessagingSystem::Receive<bool>("TargetThree clicked");
        bool target_four_click = Application::MessagingSystem::Receive<bool>("TargetFour clicked");
        bool target_five_click = Application::MessagingSystem::Receive<bool>("TargetFive clicked");*/

        // Special Combat Camera
        if (battle.enable_combat_camera)
        {
            UpdateCombatCamera();
        }

        if (battle.is_win || battle.is_lose)
        {
            static bool SendMSG = false;
            if (Input::AnyKeyDown() && !SendMSG)
            {
                //Fade in
                SendMSG = true;
                Application::MessagingSystem::Send("TransitionStart", std::pair<int, double>{ 2, 1.0 });
            }
            
            int transitionMSG = Application::MessagingSystem::Receive<int>("TransitionCompleted");
            if (transitionMSG == 2)
            {
                if (battle.is_win)
                {
                    switch (battle.battle_num)
                    {
                    case 0:
                        Application::MessagingSystem::Send("Tutorial win to Town", true);
                        break;
                    case 1:
                        Application::MessagingSystem::Send("Battle 1 win to Town", true);
                        break;
                    case 2:
                        Input::Cleanup();
                        Application::MessagingSystem::Send("Battle Boss win to Menu", true);
                        break;
                    }
                }
                else
                {
                    switch (battle.battle_num)
                    {
                    case 0:
                        Application::MessagingSystem::Send("Tutorial lose to Tutorial", true);
                        break;
                    case 1:
                        Application::MessagingSystem::Send("Battle 1 lose to Battle 1", true);
                        break;
                    case 2:
                        Input::Cleanup();
                        Application::MessagingSystem::Send("Battle Boss lose to Battle Boss", true);
                        break;
                    }
                }
                SendMSG = false;
            }
            else return;
        }


        // insta win
        if (Input::GetKeyDown(GLFW_KEY_F4))
            Win_Battle();
            // insta lose
            if (Input::GetKeyDown(GLFW_KEY_F5))
                Lose_Battle();

        // check for escape key
        if ((Input::GetKeyDown(GLFW_KEY_ESCAPE) && !battle.is_paused) || resume_game)
        {
          Pause_Functionality();
        }

        if (battle.is_paused && !FlexECS::Scene::GetEntityByName("How To Play Background").GetComponent<Transform>()->is_active) {
          if (active_pause_sprite.second) {
            FlexECS::Scene::GetEntityByName(battle.active_pause_button).GetComponent<Transform>()->is_active = false;
            FlexECS::Scene::GetEntityByName(active_pause_sprite.first).GetComponent<Scale>()->scale.x = 0.f;
            FlexECS::Scene::GetEntityByName(active_pause_sprite.first).GetComponent<Transform>()->is_active = true;
            battle.active_pause_button = active_pause_sprite.first;
          }

          if (FlexECS::Scene::GetEntityByName(battle.active_pause_button).GetComponent<Scale>()->scale.x !=
            FlexECS::Scene::GetEntityByName(battle.active_pause_button).GetComponent<Slider>()->original_scale.x) {
            FlexECS::Scene::GetEntityByName(battle.active_pause_button).GetComponent<Scale>()->scale.x +=
              Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() * 10.f;
            FlexECS::Scene::GetEntityByName(battle.active_pause_button).GetComponent<Scale>()->scale.x =
              std::clamp(FlexECS::Scene::GetEntityByName(battle.active_pause_button).GetComponent<Scale>()->scale.x,
              0.f, FlexECS::Scene::GetEntityByName(battle.active_pause_button).GetComponent<Slider>()->original_scale.x);
          }

          if (active_volume_sprite.second) {
            if (battle.active_pause_button != "Settings Button Sprite") {
              FlexECS::Scene::GetEntityByName(battle.active_pause_button).GetComponent<Transform>()->is_active = false;
              FlexECS::Scene::GetEntityByName("Settings Button Sprite").GetComponent<Scale>()->scale.x = 0.f;
              FlexECS::Scene::GetEntityByName("Settings Button Sprite").GetComponent<Transform>()->is_active = true;
              battle.active_pause_button = "Settings Button Sprite";
            }
            FlexECS::Scene::GetEntityByName(battle.active_volume_button).GetComponent<Transform>()->is_active = false;
            FlexECS::Scene::GetEntityByName(active_volume_sprite.first).GetComponent<Scale>()->scale.x = 0.f;
            FlexECS::Scene::GetEntityByName(active_volume_sprite.first).GetComponent<Transform>()->is_active = true;
            battle.active_volume_button = active_volume_sprite.first;
          }

          if (FlexECS::Scene::GetEntityByName(battle.active_volume_button).GetComponent<Scale>()->scale.x !=
            FlexECS::Scene::GetEntityByName(battle.active_volume_button).GetComponent<Slider>()->original_scale.x) {
            FlexECS::Scene::GetEntityByName(battle.active_volume_button).GetComponent<Scale>()->scale.x +=
              Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() * 10.f;
            FlexECS::Scene::GetEntityByName(battle.active_volume_button).GetComponent<Scale>()->scale.x =
              std::clamp(FlexECS::Scene::GetEntityByName(battle.active_volume_button).GetComponent<Scale>()->scale.x,
              0.f, FlexECS::Scene::GetEntityByName(battle.active_volume_button).GetComponent<Slider>()->original_scale.x);
          }

          return;
        }

        if (battle.is_tutorial && battle.is_tutorial_running)
        {
            std::string text_to_show;
            switch (battle.tutorial_info)
            {
            case 0:
                text_to_show = "The turn bar shows the order in which characters will take their turns, from left to right.";
                break;
            case 1:
                text_to_show = "Press W & S to swap moves. Press A & D to swap targets. Try it out!";
                break;
            case 2:
                text_to_show = "The smaller icon of your character on the turn bar indicates your next turn. Stronger moves tend to put you further back on the turn bar.";
                break;
            case 3:
                text_to_show = "Press SPACEBAR to confirm your move.";
                break;
            case 4:
              text_to_show = "You're a natural. Looks like we may still have a shot at saving the world after all. Now, finish this!";
                break;
            case 5:
              text_to_show = "";
                battle.is_tutorial_running = false;
                FlexECS::Scene::GetEntityByName("tutorial_textbox").GetComponent<Transform>()->is_active = false;
                FlexECS::Scene::GetEntityByName("tutorial_press_button").GetComponent<Transform>()->is_active = false;
                break;
            default:
              text_to_show = "";
                break;
            }

            FlexECS::Scene::GetEntityByName("tutorial_text").GetComponent<Text>()->text = FLX_STRING_NEW(text_to_show);
        }

        if (Input::GetKeyDown(GLFW_KEY_X))
        {
            if (!battle.god_mode)
                battle.god_mode = true;
            else battle.god_mode = false;
        }

        if (battle.disable_input_timer > 0.f)
        {
            battle.disable_input_timer -= Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
            //return;
        }

        

        if (battle.play_battle_start)
        {
           Play_Battle_Start();
        }
        else if (battle.start_of_turn)
        {
            Start_Of_Turn();
        }
        else if (battle.move_select)
        {
            Move_Select();
            Update_Damage_And_Targetting_Previews();
        }
        else if (battle.move_resolution)
        {
            Move_Resolution();
        }
        else if (battle.speedbar_animating)
        {
          PlaySpeedbarAnimation();
        }
        else if (battle.end_of_turn)
        {
            End_Of_Turn();
        }

    }

    void BattleLayer::UpdateCombatCamera()
    {
        // --- Static variables to store original values ---
        static Vector3 originalPos;
        static bool hasStoredOriginalPos = false;

        static bool hasStoredOriginalZoom = false;
        static float originalOrthoWidth = 0.0f;
        static float originalOrthoHeight = 0.0f;
        static float currentOrthoWidth = 0.0f;
        static float baseAspectRatio = 1.0f;
        static float minOrthoWidth = 0.0f;

        // --- Static variable to handle the jerk effect ---
        static Vector3 jerkEffect(0, 0, 0);

        // --- Early out if neither combat camera nor force disable is active ---
        if (!battle.enable_combat_camera && !battle.force_disable_combat_camera)
            return;

        // --- Get the camera entity and its position component ---
        FlexECS::Entity cameraEntity = FlexECS::Scene::GetActiveScene()->GetEntityByName("Camera");
        auto posComp = cameraEntity.GetComponent<Position>();
        if (!posComp)
            return;

        // --- Store original camera position once ---
        if (!hasStoredOriginalPos)
        {
            originalPos = posComp->position;
            hasStoredOriginalPos = true;
        }

        // --- Retrieve main camera for zoom controls ---
        Camera* mainCam = CameraManager::GetMainGameCamera();
        if (!mainCam)
            return;

        // --- Store original zoom values once ---
        if (!hasStoredOriginalZoom)
        {
            // Assuming the camera provides these getter functions.
            originalOrthoWidth = mainCam->GetOrthoWidth();
            originalOrthoHeight = mainCam->GetOrthoHeight();
            currentOrthoWidth = originalOrthoWidth;
            baseAspectRatio = originalOrthoWidth / originalOrthoHeight;
            minOrthoWidth = originalOrthoWidth * 0.5f; // Example: allow zooming in to half the width.
            hasStoredOriginalZoom = true;
        }

        // --- If battle pointers are invalid, force disable the combat camera ---
        if (!battle.initial_target || !battle.current_character)
        {
            battle.force_disable_combat_camera = true;
        }

        // --- Determine the base target position (for camera movement) ---
        Vector3 baseTargetPos = originalPos;
        FlexECS::Entity attacker, defender;
        if (battle.enable_combat_camera && battle.initial_target && battle.current_character)
        {
            if (battle.current_character->character_id > 2)
            {
                attacker = FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(battle.current_character->current_slot + 1));
                defender = FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(battle.initial_target->current_slot + 1));
            }
            else
            {
                attacker = FlexECS::Scene::GetEntityByName("Drifter " + std::to_string(battle.current_character->current_slot + 1));
                defender = FlexECS::Scene::GetEntityByName("Enemy " + std::to_string(battle.initial_target->current_slot + 1));
            }
            if (attacker != FlexECS::Entity::Null && defender != FlexECS::Entity::Null)
            {
                if (attacker.GetComponent<Position>() && defender.GetComponent<Position>())
                {
                    Vector3 attackerPos = attacker.GetComponent<Position>()->position;
                    Vector3 defenderPos = defender.GetComponent<Position>()->position;
                    baseTargetPos = (attackerPos + defenderPos) * 0.5f;
                }
            }

        }

        float deltaTime = Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
        float lerpSpeed = 5.0f; // Adjust for smoother or snappier interpolation.

        // --- Handle the jerk effect ---
        // When the jerk flag is set, compute and store a sudden jerk offset.
        if (battle.jerk_towards_defender && battle.enable_combat_camera && battle.initial_target && battle.current_character)
        {
            if (defender.GetComponent<Position>())
            {
                Vector3 defenderPos = defender.GetComponent<Position>()->position;
                Vector3 direction = (defenderPos - baseTargetPos).Normalize();
                float jerkMagnitude = 400.f; // Adjust for desired sudden jerk intensity.
                jerkEffect = direction * jerkMagnitude;
            }
            // Reset the flag so the jerk is applied only once.
            battle.jerk_towards_defender = false;
        }
        else
        {
            // Decay the jerk effect smoothly back to zero.
            float jerkDecay = 10.0f; // Adjust decay speed as needed.
            jerkEffect = Lerp(jerkEffect, Vector3(0, 0, 0), 1.0f - exp(-jerkDecay * deltaTime));
        }

        // Final target position includes both the base target and the jerk offset.
        Vector3 finalTargetPos = baseTargetPos + jerkEffect;

        // --- Lerp the camera position toward the final target ---
        posComp->position = Lerp(posComp->position, finalTargetPos, 1.0f - exp(-lerpSpeed * deltaTime));

        // --- Integrate zooming as part of the lerp ---
        // Define a delta zoom value; for example, a negative value zooms in.
        float deltaZoom = -500.0f; // Adjust as needed.
        float targetOrthoWidth = originalOrthoWidth;
        if (!battle.force_disable_combat_camera && baseTargetPos != originalPos)
        {
            // Calculate the target width, ensuring it does not go below the minimum allowed.
            float newWidth = originalOrthoWidth + deltaZoom;
            if (newWidth < minOrthoWidth)
                newWidth = minOrthoWidth;
            targetOrthoWidth = newWidth;
        }
        // If not enabled (or force disabling), target zoom is the original zoom.
        else
        {
            targetOrthoWidth = originalOrthoWidth;
        }
        // Lerp the current orthographic width toward the target.
        currentOrthoWidth = Lerp(currentOrthoWidth, targetOrthoWidth, 1.0f - exp(-lerpSpeed * deltaTime));
        float effectiveOrthoHeight = currentOrthoWidth / baseAspectRatio;
        mainCam->SetOrthographic(-currentOrthoWidth / 2, currentOrthoWidth / 2,
                                   -effectiveOrthoHeight / 2, effectiveOrthoHeight / 2);

        // --- When force disabling, check if both position and zoom have nearly returned to their original values ---
        if (battle.force_disable_combat_camera)
        {
            if ((posComp->position - originalPos).Length() < 0.01f && fabs(currentOrthoWidth - originalOrthoWidth) < 0.01f)
            {
                posComp->position = originalPos;
                currentOrthoWidth = originalOrthoWidth;
                battle.enable_combat_camera = false;
                battle.force_disable_combat_camera = false;
                battle.jerk_towards_defender = false;
                hasStoredOriginalPos = false;
                hasStoredOriginalZoom = false;
            }
        }
    }

} // namespace Game