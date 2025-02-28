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

namespace Game
{

  // intermediary oop data structure to store the battle data
  // we're doing this because our team is struggling with the ECS system

#pragma region Battle Data

  struct _Move
  {
    std::string name = "";
    int damage = 0;
    int speed = 0;

    std::vector<std::string> effect;
    std::vector<int> value;
    std::vector<std::string> target;

    //std::string effect = "";
    //int value = 0;
    //std::string target = "";

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
    bool is_alive = true;
  };

  struct _Battle
  {
    std::vector<_Character> drifters = {};
    std::vector<_Character> enemies = {};
    int boss_battle = 0; // 0 = false, 1-5 = true, pointing out which enemy slot is the boss

    std::vector<_Character*> drifters_and_enemies = {};
    std::array<_Character*, 2> drifter_slots = { nullptr, nullptr };
    std::array<_Character*, 5> enemy_slots = { nullptr, nullptr, nullptr, nullptr, nullptr };
    std::vector<_Character*> speed_bar = {};

    FlexECS::Entity projected_character;
    int projected_speed = 0;

    int target = 0; // 0-4, pointing out which enemy slot is the target
    std::array<Vector3, 7> sprite_slot_positions = {};
    // std::array<Vector3, 7> speedbar_slot_positions; // needed for animated speed bar
    std::array<Vector3, 7> healthbar_slot_positions = {};

    //
    _Move* current_move = nullptr;
    _Character* current_character = nullptr;
    _Character* initial_target = nullptr;

    // game state
    bool is_player_turn = true;
    float disable_input_timer = 0.f;
    bool prev_state = is_player_turn; // used to cache the previous state, but also can be set to false even when the player takes a turn and still their turn next

    int drifter_alive_count = 0;
    int enemy_alive_count = 0;
    bool is_win = false;
    bool is_lose = false;

    // Return to original position
    FlexECS::Entity previous_character_entity = FlexECS::Entity::Null;
    _Character* previous_character = nullptr;
  };

  _Battle battle;

  void Internal_ParseBattle(AssetKey assetkey)
  {
    // get the battle asset
    auto& asset = FLX_ASSET_GET(Asset::Battle, assetkey);

    // reserve the vectors
    // pushing back will invalidate the pointers in the arrays
    // TODO: this is a temporary solution
    battle.drifters.reserve(asset.GetDrifterSlots().size() + 1);
    battle.enemies.reserve(asset.GetEnemySlots().size() + 1);

    // parse the battle data
    int index = 0;
    for (auto& slot : asset.GetDrifterSlots())
    {
      _Character character;
      if (*slot == "None")
      {
        battle.drifter_slots[index] = nullptr;
        index++;
        continue;
      }
      auto& character_asset = FLX_ASSET_GET(Asset::Character, *slot);
      character.name = character_asset.character_name;
      character.health = character_asset.health;
      character.speed = character_asset.speed;
      character.character_id = character_asset.character_id;

      if (character_asset.move_one != "None")
      {
        auto& move_one_asset = FLX_ASSET_GET(Asset::Move, character_asset.move_one);

        _Move move_one;
        move_one.name = move_one_asset.name;
        move_one.damage = move_one_asset.damage;
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
        move_two.damage = move_two_asset.damage;
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
        move_three.damage = move_three_asset.damage;
        move_three.speed = move_three_asset.speed;

        move_three.effect = move_three_asset.effect;
        move_three.value = move_three_asset.value;
        move_three.target = move_three_asset.target;

        move_three.description = move_three_asset.description;
        character.move_three = move_three;
      }

      character.current_slot = index;

      battle.drifters.push_back(character);
      battle.drifter_slots[index] = &battle.drifters.back();
      index++;
    }

    index = 0;
    for (auto& slot : asset.GetEnemySlots())
    {
      _Character character;
      if (*slot == "None")
      {
        battle.enemy_slots[index] = nullptr;
        index++;
        continue;
      }
      auto& character_asset = FLX_ASSET_GET(Asset::Character, *slot);
      character.name = character_asset.character_name;
      character.health = character_asset.health;
      character.speed = character_asset.speed;
      character.character_id = character_asset.character_id;

      if (character_asset.move_one != "None")
      {
          auto& move_one_asset = FLX_ASSET_GET(Asset::Move, character_asset.move_one);

          _Move move_one;
          move_one.name = move_one_asset.name;
          move_one.damage = move_one_asset.damage;
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
          move_two.damage = move_two_asset.damage;
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
          move_three.damage = move_three_asset.damage;
          move_three.speed = move_three_asset.speed;

          move_three.effect = move_three_asset.effect;
          move_three.value = move_three_asset.value;
          move_three.target = move_three_asset.target;

          move_three.description = move_three_asset.description;
          character.move_three = move_three;
      }

      character.current_slot = index;

      battle.enemies.push_back(character);
      battle.enemy_slots[index] = &battle.enemies.back();
      index++;
    }

    battle.boss_battle = asset.boss_battle;

    // can't have empty drifter or enemy vector
    FLX_ASSERT(battle.drifter_slots.size() > 0, "Drifter slots cannot be empty.");
    FLX_ASSERT(battle.enemy_slots.size() > 0, "Enemy slots cannot be empty.");
  }

#pragma endregion
  
  FlexECS::Entity main_camera = FlexECS::Entity::Null;

  void BattleLayer::OnAttach()
  {
    File& file = File::Open(Path::current("assets/saves/battlescene_v5.flxscene"));
    FlexECS::Scene::SetActiveScene(FlexECS::Scene::Load(file));

    CameraManager::SetMainGameCameraID(FlexECS::Scene::GetEntityByName("Camera"));

    // Start battle music by default on scene load
    FlexECS::Scene::GetEntityByName("Background Music").GetComponent<Audio>()->should_play = true;

  #pragma region Load Battle Data

      // load the battle
      Internal_ParseBattle(R"(/data/debug.flxbattle)");

      // init non-loaded values
      // these values are used internally for the battle system and are not saved
      for (auto& character : battle.drifters)
      {
        battle.drifters_and_enemies.push_back(&character);
        battle.speed_bar.push_back(&character);
        battle.drifter_alive_count++;
      }
      for (auto& character : battle.enemies)
      {
        battle.drifters_and_enemies.push_back(&character);
        battle.speed_bar.push_back(&character);
        battle.enemy_alive_count++;
      }

      for (auto character : battle.speed_bar)
      {
        character->current_health = character->health;
        character->current_speed = character->speed;
        character->is_alive = true;
      }

      // cache the slot positions
      // this is purely just so that the editor can actually position the slots
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

      // create entities for the characters using the battle data
      // note that the system doesn�t deal with duplicates
      // dupes break the targetting system and anything that uses GetEntityByName
      int index = 0;
      FlexECS::Entity e;
      auto scene = FlexECS::Scene::GetActiveScene();

      //projected_character icon creation
      battle.projected_character = scene->CreateEntity("projected_char"); // can always use GetEntityByName to find the entity
      battle.projected_character.AddComponent<Transform>({});
      battle.projected_character.AddComponent<Position>({});
      battle.projected_character.AddComponent<Rotation>({});
      battle.projected_character.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Question Mark.png)") });
      battle.projected_character.AddComponent<Scale>({ Vector3(0.5, 0.5, 0) });
      battle.projected_character.AddComponent<ZIndex>({ 21 });
      battle.projected_character.GetComponent<Transform>()->is_active = false;

      // Spawn characters
      for (auto& character : battle.drifters)
      {
        e = scene->CreateEntity(character.name); // can always use GetEntityByName to find the entity
        e.AddComponent<Transform>({});
        e.AddComponent<Character>({});
        e.AddComponent<Drifter>({});

        // find the slot position
        e.AddComponent<Position>({ battle.sprite_slot_positions[character.current_slot] });
        e.AddComponent<Rotation>({});
        e.AddComponent<Sprite>({  });//FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Question Mark.png)") });

        switch (character.character_id)
        {
        case 1:
          e.AddComponent<Scale>({ Vector3(2, 2, 0) });
          e.AddComponent<Animator>(
            { FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
              FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Attack_Anim_Sheet.flxspritesheet)") }
          );
          break;
        case 2:
          e.AddComponent<Scale>({ Vector3(2, 2, 0) });
          e.AddComponent<Animator>(
            { FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)"),
              FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)") }
          );
          break;
        }

        e.AddComponent<ZIndex>({ 25 + index++ });
      }

      // Spawn enemies
      index = 0;
      for (auto& character : battle.enemies)
      {
        e = scene->CreateEntity(character.name); // can always use GetEntityByName to find the entity
        e.AddComponent<Transform>({});
        e.AddComponent<Character>({});
        e.AddComponent<Enemy>({});

        e.AddComponent<Position>({ battle.sprite_slot_positions[character.current_slot + 2] + Vector3(-18, 25, 0) }
        ); // offset by 2 for enemy slots, and offset the position
        e.AddComponent<Rotation>({});
        e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Question Mark.png)") });

        switch (character.character_id)
        {
        case 3:
          e.AddComponent<Scale>({ Vector3(2, 2, 0) });
          e.AddComponent<Animator>(
            { FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_01_Idle_Anim_Sheet.flxspritesheet)"),
              FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_01_Idle_Anim_Sheet.flxspritesheet)") }
          );
          break;
        case 4:
          e.AddComponent<Scale>({ Vector3(2, 2, 0) });
          e.AddComponent<Animator>(
            { FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_02_Idle_Anim_Sheet.flxspritesheet)"),
              FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_02_Idle_Anim_Sheet.flxspritesheet)") }
          );
          break;
        case 5:
          e.AddComponent<Scale>({ Vector3(2, 2, 0) });
          e.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Idle_Anim_Sheet.flxspritesheet)"),
                                     FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Idle_Anim_Sheet.flxspritesheet)") }
          );
          break;
        }

        e.AddComponent<ZIndex>({ 21 + index++ });
      }

      // create the healthbars
      for (auto& character : battle.drifters)
      {
        e = scene->CreateEntity(character.name + " Healthbar"); // can always use GetEntityByName to find the entity
        e.AddComponent<Healthbar>({});
        e.AddComponent<Transform>({});
        e.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot] });
        e.AddComponent<Rotation>({});
        e.AddComponent<Scale>({ Vector3(.1, .1, 0) });
        e.GetComponent<Healthbar>()->original_position = e.GetComponent<Position>()->position;
        e.GetComponent<Healthbar>()->original_scale = e.GetComponent<Scale>()->scale;

        e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_HealthBar_Green.png)") });
        e.AddComponent<ZIndex>({ 35 });

        e = scene->CreateEntity(character.name + " Stats"); // can always use GetEntityByName to find the entity
        e.AddComponent<Transform>({});
        e.AddComponent<Position>({ battle.sprite_slot_positions[character.current_slot] + Vector3(-30, -100, 0) });
        e.AddComponent<Rotation>({});
        e.AddComponent<Scale>({ Vector3(0.3f, 0.3f, 0) });
        e.AddComponent<ZIndex>({ 3 });
        e.AddComponent<Text>({
          FLX_STRING_NEW(R"(/fonts/Closeness/Closeness.ttf)"),
          FLX_STRING_NEW(R"(Itches)"),
          Vector3(1.0f, 1.0, 1.0f),
          { Renderer2DText::Alignment_Left, Renderer2DText::Alignment_Center },
          {                            600,                              320 }
        });

        e = scene->CreateEntity(character.name + " Attack_Buff"); // can always use GetEntityByName to find the entity
        e.AddComponent<Transform>({});
        e.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot] + Vector3(-110, 0, 0) });
        e.AddComponent<Rotation>({});
        e.AddComponent<Scale>({ Vector3(.05, .05, 0) });

        e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Attack_+1.png)") });
        e.AddComponent<ZIndex>({ 36 });

        e = scene->CreateEntity(character.name + " Attack_Debuff"); // can always use GetEntityByName to find the entity
        e.AddComponent<Transform>({});
        e.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot] + Vector3(-70, 0, 0) });
        e.AddComponent<Rotation>({});
        e.AddComponent<Scale>({ Vector3(.05, .05, 0) });

        e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Attack_-1.png)") });
        e.AddComponent<ZIndex>({ 36 });

        e = scene->CreateEntity(character.name + " Stun_Debuff"); // can always use GetEntityByName to find the entity
        e.AddComponent<Transform>({});
        e.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot] + Vector3(-30, 0, 0) });
        e.AddComponent<Rotation>({});
        e.AddComponent<Scale>({ Vector3(.05, .05, 0) });

        e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Stun.png)") });
        e.AddComponent<ZIndex>({ 36 });

        e = scene->CreateEntity(character.name + " Shield_Buff"); // can always use GetEntityByName to find the entity
        e.AddComponent<Transform>({});
        e.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot] + Vector3(10, 0, 0) });
        e.AddComponent<Rotation>({});
        e.AddComponent<Scale>({ Vector3(.05, .05, 0) });

        e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Def_+1.png)") });
        e.AddComponent<ZIndex>({ 36 });

        e = scene->CreateEntity(character.name + " Protect_Buff"); // can always use GetEntityByName to find the entity
        e.AddComponent<Transform>({});
        e.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot] + Vector3(50, 0, 0) });
        e.AddComponent<Rotation>({});
        e.AddComponent<Scale>({ Vector3(.05, .05, 0) });

        e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Heal_+1.png)") });
        e.AddComponent<ZIndex>({ 36 });
      }
      for (auto& character : battle.enemies)
      {
        e = scene->CreateEntity(character.name + " Healthbar"); // can always use GetEntityByName to find the entity
        e.AddComponent<Healthbar>({});
        e.AddComponent<Transform>({});
        e.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot + 2] } );
        e.AddComponent<Rotation>({});
        e.AddComponent<Scale>({ Vector3(.1, .1, 0) });
        e.GetComponent<Healthbar>()->original_position = e.GetComponent<Position>()->position;
        e.GetComponent<Healthbar>()->original_scale = e.GetComponent<Scale>()->scale;
        e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_HealthBar_Red.png)") });
        e.AddComponent<ZIndex>({ 35 });

        e = scene->CreateEntity(character.name + " Stats"); // can always use GetEntityByName to find the entity
        e.AddComponent<Transform>({});
        e.AddComponent<Position>({ battle.sprite_slot_positions[character.current_slot + 2] + Vector3(-105, -75, 0) });
        e.AddComponent<Rotation>({});
        e.AddComponent<Scale>({ Vector3(0.3f, 0.3f, 0) });
        e.AddComponent<ZIndex>({ 3 });
        e.AddComponent<Text>({
          FLX_STRING_NEW(R"(/fonts/Closeness/Closeness.ttf)"),
          FLX_STRING_NEW(R"(Itches)"),
          Vector3(1.0f, 1.0, 1.0f),
          { Renderer2DText::Alignment_Left, Renderer2DText::Alignment_Center },
          {                            600,                              320 }
        });

        e = scene->CreateEntity(character.name + " Attack_Buff"); // can always use GetEntityByName to find the entity
        e.AddComponent<Transform>({});
        e.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot + 2] + Vector3(-80, 15, 0) });
        e.AddComponent<Rotation>({});
        e.AddComponent<Scale>({ Vector3(.025, .025, 0) });

        e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Attack_+1.png)") });
        e.AddComponent<ZIndex>({ 36 });

        e = scene->CreateEntity(character.name + " Attack_Debuff"); // can always use GetEntityByName to find the entity
        e.AddComponent<Transform>({});
        e.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot + 2] + Vector3(-50, 15, 0) });
        e.AddComponent<Rotation>({});
        e.AddComponent<Scale>({ Vector3(.025, .025, 0) });

        e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Attack_-1.png)") });
        e.AddComponent<ZIndex>({ 36 });

        e = scene->CreateEntity(character.name + " Stun_Debuff"); // can always use GetEntityByName to find the entity
        e.AddComponent<Transform>({});
        e.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot + 2] + Vector3(-20, 15, 0) });
        e.AddComponent<Rotation>({});
        e.AddComponent<Scale>({ Vector3(.025, .025, 0) });

        e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Stun.png)") });
        e.AddComponent<ZIndex>({ 36 });

        e = scene->CreateEntity(character.name + " Shield_Buff"); // can always use GetEntityByName to find the entity
        e.AddComponent<Transform>({});
        e.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot + 2] + Vector3(10, 15, 0) });
        e.AddComponent<Rotation>({});
        e.AddComponent<Scale>({ Vector3(.025, .025, 0) });

        e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Def_+1.png)") });
        e.AddComponent<ZIndex>({ 36 });

        e = scene->CreateEntity(character.name + " Protect_Buff"); // can always use GetEntityByName to find the entity
        e.AddComponent<Transform>({});
        e.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot + 2] + Vector3(40, 15, 0) });
        e.AddComponent<Rotation>({});
        e.AddComponent<Scale>({ Vector3(.025, .025, 0) });

        e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_Heal_+1.png)") });
        e.AddComponent<ZIndex>({ 36 });
      }

  #pragma endregion

    main_camera = FlexECS::Scene::GetEntityByName("Camera");

    // Just set some random shit as the target for the start of game lmao
    //battle.target = 1;
  }

  void BattleLayer::OnDetach()
  {
    // Make sure nothing carries over in the way of sound
    FMODWrapper::Core::ForceStop();
  }

  void BattleLayer::Update()
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
      CameraManager::SetMainGameCameraID(main_camera);
    }
    
    if (battle.is_win && Input::AnyKeyDown())
    {
      Application::MessagingSystem::Send("Game win to menu", true);
    }

    if (battle.is_lose && Input::GetKeyDown(GLFW_KEY_R))
    {
      // TODO: Retry or something
      Application::MessagingSystem::Send("Game lose to menu", true);
    }

    // return if the battle is over
    if (battle.is_win || battle.is_lose) return;

    // start of the battle system
    //
    // Battle System Preparation
    // - return when playing animations (disable_input_timer > 0)
    // - get the current character from the speed bar
    //
    // AI Move
    // - if it's not the player's turn, do the AI move and skip the player input code
    //
    // Player Input
    // - if it's the player's turn, skip the AI move and check for input
    // - if the player has selected a target, check for input for the move
    // - if the player has selected a move, apply the move and update the speed
    // - play the animations and disable input for the duration of the move animation
    //
    // Resolve Game State
    // - resolve speed bar
    //
    // Update Displays
    // - update the targeting display
    // - update the speed bar display
    //
    // end of the battle system

  #pragma region Battle System Preparation

    // current active character

    //_Character current_character
    battle.current_character = battle.speed_bar.front();

    auto current_character_entity = FlexECS::Scene::GetActiveScene()->GetEntityByName(battle.current_character->name);
    auto& current_character_animator = *current_character_entity.GetComponent<Animator>();

    FLX_ASSERT(battle.current_character != nullptr, "Current character is null.");
    FLX_ASSERT(battle.current_character->is_alive, "Current character is dead.");

    // determine if it's the player's turn
    // player is 1-2, enemy is 3-5
    battle.is_player_turn = (battle.current_character->character_id <= 2);

    // Flip only if not true to cache for later
    if (!battle.is_player_turn) battle.prev_state = battle.is_player_turn;

    // disable input if the timer is active
    if (battle.disable_input_timer > 0.f)
    {
      battle.disable_input_timer -= Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();

      // hide the move UI during the animation
      for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, MoveUI>())
        entity.GetComponent<Transform>()->is_active = false;

      return;
    }

    if (battle.previous_character != nullptr) {
      Vector3 previous_position = (battle.previous_character->character_id <= 2) ?
        battle.sprite_slot_positions[battle.previous_character->current_slot] : 
        battle.sprite_slot_positions[battle.previous_character->current_slot + 2];
      battle.previous_character_entity.GetComponent<Position>()->position = previous_position;
    }
    // hide or display the move UI
    for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, MoveUI>())
      entity.GetComponent<Transform>()->is_active = battle.is_player_turn;

#if 0
    // lock characters to their slots
    for (auto character : battle.drifters)
    {
      auto entity = FlexECS::Scene::GetActiveScene()->GetEntityByName(character.name);
      entity.GetComponent<Position>()->position = battle.sprite_slot_positions[character.current_slot];
    }
    // offset current player character to the right a bit
    if (battle.is_player_turn)
    {
        current_character_entity.GetComponent<Position>()->position.x += 100;
    }
#endif

    // Just swapped from enemy phase to player phase
    if (battle.prev_state != battle.is_player_turn && battle.is_player_turn)
    {
      // Plays sound if swap from enemy phase to player phase
        std::string audio_to_play = "/audio/" + battle.current_character->name + " start.mp3";
        FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file = FLX_STRING_NEW(audio_to_play);
        //FLX_STRING_NEW(R"(/audio/start turn.mp3)");
      FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;

      /*
      // Defaults target selection
      for (int i{0}; i < battle.enemy_slots.size(); ++i)
      {
        if (battle.enemy_slots[i]->is_alive && battle.enemy_slots[i] != nullptr)
        {
          battle.target = i + 1;
          break;
        }
      }
      */
      battle.current_move = nullptr;
      battle.initial_target = nullptr;
      battle.target = 0;
      battle.prev_state = battle.is_player_turn; // dont forget to reset, of course...
    }


#pragma endregion


#pragma region AI Move

    // enemy AI
    if (!battle.is_player_turn && battle.current_character->stun_debuff_duration == 0)
    {
      // randomly pick a target
        //battle.current_move = nullptr;

        // randomly pick a move
      // TODO: not yet
        battle.current_move = nullptr;
        battle.initial_target = nullptr;
        battle.target = 0;

        int move_num = Range<int>(0, 2).Get();
        switch (move_num)
        {
        case 0:
            battle.current_move = &battle.current_character->move_one;
            break;
        case 1:
            battle.current_move = &battle.current_character->move_two;
            break;
        case 2:
            battle.current_move = &battle.current_character->move_three;
            break;
        }

      while (battle.initial_target == nullptr || !battle.initial_target->is_alive) battle.initial_target = battle.drifter_slots[Range<int>(0, 1).Get()];

      // Temporarily move the character
      battle.previous_character = battle.current_character;
      battle.previous_character_entity = current_character_entity;
      current_character_entity.GetComponent<Position>()->position = battle.sprite_slot_positions[battle.initial_target->current_slot];
      
      // apply the move
      //Move_Resolution();
      //target_character->current_health -= move->damage;
      //target_character->current_speed += move->damage;

      // update the character's speed
      battle.current_character->current_speed += battle.current_character->speed + battle.current_move->speed;

          if (battle.current_character->attack_buff_duration > 0)
              battle.current_character->attack_buff_duration -= 1;

          if (battle.current_character->attack_debuff_duration > 0)
              battle.current_character->attack_debuff_duration -= 1;

          if (battle.current_character->shield_buff_duration > 0)
              battle.current_character->shield_buff_duration -= 1;

          if (battle.current_character->protect_buff_duration > 0)
              battle.current_character->protect_buff_duration -= 1;

      std::vector<_Character*> targets;
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
                  if (character->is_alive && (character->current_slot == battle.target - 1 && character->character_id <= 2 || character->current_slot == battle.target && character->character_id <= 2 || character->current_slot == battle.target + 1 && character->character_id <= 2))
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
                  if (character->is_alive && character->current_slot == battle.target && character->character_id <= 2)
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
                      if (character->is_alive && character->current_slot == battle.target && character->character_id > 2)
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
#pragma region Animation

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
      target_animator.should_play = true;
      target_animator.is_looping = false;
      target_animator.return_to_default = true;
      target_animator.frame_time = 0.f;
      target_animator.current_frame = 0;

#pragma endregion

      // disable input for the duration of the move animation
      battle.disable_input_timer += animation_time + 1.f;
    }

#pragma endregion


#pragma region Player Input

#pragma region Targeting

    // TODO: support multi-targeting patterns
    if (battle.is_player_turn && battle.current_move != nullptr)
    {
        if (Input::GetKeyDown(GLFW_KEY_1) || target_one_click)
        {
            if (battle.target == 0)
            {
                battle.target = -1;
            }
            else battle.target = 0;
        }
      else if (Input::GetKeyDown(GLFW_KEY_2) || target_two_click)
        {
            if (battle.target == 1)
            {
                battle.target = -1;
            }
            else battle.target = 1;
        }
      else if (Input::GetKeyDown(GLFW_KEY_3) || target_three_click)
        {
            if (battle.target == 2)
            {
                battle.target = -1;
            }
            else battle.target = 2;
        }
      else if (Input::GetKeyDown(GLFW_KEY_4) || target_four_click)
        {
            if (battle.target == 3)
            {
                battle.target = -1;
            }
            else battle.target = 3;
        }
      else if (Input::GetKeyDown(GLFW_KEY_5) || target_five_click)
        {
            if (battle.target == 4)
            {
                battle.target = -1;
            }
            else battle.target = 4;
        }

      if (battle.current_move->target[0] == "ALL_ALLIES" || battle.current_move->target[0] == "NEXT_ALLY" || battle.current_move->target[0] == "SINGLE_ALLY" || battle.current_move->target[0] == "SELF")
      {
          for (auto character : battle.drifters_and_enemies)
          {
              if (character->current_slot == battle.target && (character->character_id <= 2))
              {
                  battle.initial_target = character;
                  if (!battle.initial_target->is_alive)
                      battle.initial_target = nullptr;
                  break;
              }
              else battle.initial_target = nullptr;
          }
          if (battle.initial_target == nullptr)
          {
              for (auto character : battle.drifters_and_enemies)
              {
                  if (character->character_id <= 2 && character->is_alive)
                  {
                      battle.initial_target = character;
                      battle.target = character->current_slot;
                      break;
                  }
              }
          }
      }
      else
      {
          for (auto character : battle.drifters_and_enemies)
          {
              if (character->current_slot == battle.target && (character->character_id > 2))
              {
                  battle.initial_target = character;
                  if (!battle.initial_target->is_alive)
                      battle.initial_target = nullptr;
                  break;
              }
              else battle.initial_target = nullptr;
          }
          if (battle.initial_target == nullptr)
          {
              for (auto character : battle.drifters_and_enemies)
              {
                  if (character->character_id > 2 && character->is_alive)
                  {
                      battle.initial_target = character;
                      battle.target = character->current_slot;
                      break;
                  }
              }
          }
      }
    }

#pragma endregion

#pragma region Moves
    if (battle.current_character->stun_debuff_duration > 0)
    {
        battle.current_character->stun_debuff_duration--;

        if (battle.current_character->attack_buff_duration > 0)
            battle.current_character->attack_buff_duration -= 1;

        if (battle.current_character->attack_debuff_duration > 0)
            battle.current_character->attack_debuff_duration -= 1;

        if (battle.current_character->shield_buff_duration > 0)
            battle.current_character->shield_buff_duration -= 1;

        if (battle.current_character->protect_buff_duration > 0)
            battle.current_character->protect_buff_duration -= 1;

        battle.current_character->current_speed = battle.current_character->speed + 10;

        // Reset move selection, as well as description
        FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move Description Text").GetComponent<Text>()->text) = "";
        FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 1 Text").GetComponent<Text>()->text) = "";
        FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 2 Text").GetComponent<Text>()->text) = "";
        FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 3 Text").GetComponent<Text>()->text) = "";

        battle.prev_state = false; // Just swapped from player phase to enemy phase (even if its the player turn next, take it as so.)
    }
    // Executes if player turn and target is selected and some move is already selected
    else if (battle.is_player_turn && Input::GetKeyDown(GLFW_KEY_SPACE) && battle.current_move != nullptr && battle.initial_target != nullptr)
    {
        if (battle.current_move == &battle.current_character->move_one)
        {
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
        }
        if (battle.current_move == &battle.current_character->move_two)
        {
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
        }
        if (battle.current_move == &battle.current_character->move_three)
        {
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
        }

        // temporarily move character
        if (battle.current_move->target[0] == "ALL_ALLIES" || battle.current_move->target[0] == "NEXT_ALLY" || battle.current_move->target[0] == "SINGLE_ALLY" || battle.current_move->target[0] == "SELF")
        {
        }
        else
        {
            battle.previous_character = battle.current_character;
            battle.previous_character_entity = current_character_entity;
            current_character_entity.GetComponent<Position>()->position = battle.sprite_slot_positions[battle.initial_target->current_slot + 2];
        }

        // update the character's speed
        battle.current_character->current_speed += battle.current_character->speed + battle.current_move->speed;

        if (battle.current_character->attack_buff_duration > 0)
        battle.current_character->attack_buff_duration--;

        if (battle.current_character->attack_debuff_duration > 0)
        battle.current_character->attack_debuff_duration--;

        if (battle.current_character->shield_buff_duration > 0)
        battle.current_character->shield_buff_duration--;

        if (battle.current_character->protect_buff_duration > 0)
        battle.current_character->protect_buff_duration--;

        // apply the move
        std::vector<_Character*> targets;
        for (int i = 0; i < battle.current_move->effect.size(); i++) {
            Log::Debug((battle.current_move->effect[i]));
            Log::Debug(std::to_string(battle.current_move->value[i]));
            Log::Debug((battle.current_move->target[i]));
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
                    if (character->is_alive && (character->current_slot == battle.target - 1 && character->character_id > 2 || character->current_slot == battle.target && character->character_id > 2 || character->current_slot == battle.target + 1 && character->character_id > 2))
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
                    if (character->is_alive && character->current_slot == battle.target && character->character_id > 2)
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
                        if (character->is_alive && character->current_slot == battle.target && character->character_id <= 2)
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

        // reset the target
        //battle.target = 0;

        // play the animation
        float animation_time =
          FLX_ASSET_GET(Asset::Spritesheet, FLX_STRING_GET(current_character_animator.spritesheet_handle))
            .total_frame_time;
        current_character_animator.should_play = true;
        current_character_animator.is_looping = false;
        current_character_animator.return_to_default = true;
        current_character_animator.frame_time = 0.f;
        current_character_animator.current_frame = 0;

        auto target_entity = FlexECS::Scene::GetActiveScene()->GetEntityByName(battle.initial_target->name);
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
          // special case, the screen will flash red
          break;
        }
        target_animator.should_play = true;
        target_animator.is_looping = false;
        target_animator.return_to_default = true;
        target_animator.frame_time = 0.f;
        target_animator.current_frame = 0;

        // disable input for the duration of the move animation
        battle.disable_input_timer += animation_time + 1.f;

        // Reset move selection, as well as description
        FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move Description Text").GetComponent<Text>()->text) = "";
        FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 1 Text").GetComponent<Text>()->text) = "";
        FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 2 Text").GetComponent<Text>()->text) = "";
        FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 3 Text").GetComponent<Text>()->text) = "";

        battle.projected_character.GetComponent<Transform>()->is_active = false;
        battle.projected_speed = 0;

        battle.is_player_turn = false;
        battle.prev_state = false; // Just swapped from player phase to enemy phase (even if its the player turn next, take it as so.)
    }

    // Sets the description for the current character and locks the current selected move for the next loop to consume
    if (battle.is_player_turn)
    {
      if (Input::GetKeyDown(GLFW_KEY_Z) || move_one_click)
      {
        battle.current_move = &battle.current_character->move_one;
        FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move Description Text").GetComponent<Text>()->text) = 
            battle.current_character->move_one.description;
        battle.projected_speed = battle.current_character->speed + battle.current_move->speed;
      }
      if (Input::GetKeyDown(GLFW_KEY_X) || move_two_click)
      {
        battle.current_move = &battle.current_character->move_two;
        FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move Description Text").GetComponent<Text>()->text) =  
            battle.current_character->move_two.description;
        battle.projected_speed = battle.current_character->speed + battle.current_move->speed;
      }
      if (Input::GetKeyDown(GLFW_KEY_C) || move_three_click)
      {
        battle.current_move = &battle.current_character->move_three;
        FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move Description Text").GetComponent<Text>()->text) = 
            battle.current_character->move_three.description;
        battle.projected_speed = battle.current_character->speed + battle.current_move->speed;
      }

      int slot_number = -1; //will always be bigger than first element (itself), account for +1 for slot 0.
      if (battle.projected_speed > 0)
      {
          battle.projected_character.GetComponent<Transform>()->is_active = true;
          for (auto character : battle.speed_bar)
          {
              if (battle.projected_speed < character->current_speed)
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

      // update the character id in the slot based on the speed bar order
      for (FlexECS::Entity& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<SpeedBarSlot>())
      {
          auto& speed_bar_slot = *entity.GetComponent<SpeedBarSlot>();

          if (speed_bar_slot.slot_number <= battle.speed_bar.size())
              speed_bar_slot.character = battle.speed_bar[speed_bar_slot.slot_number - 1]->character_id;
          else
              speed_bar_slot.character = 0;
      }

      //button text
      FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 1 Text").GetComponent<Text>()->text) =
        "[Z] " + battle.current_character->move_one.name;
      FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 2 Text").GetComponent<Text>()->text) =
        "[X] " + battle.current_character->move_two.name;
      FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 3 Text").GetComponent<Text>()->text) =
        "[C] " + battle.current_character->move_three.name;

      //button UI
      FlexECS::Entity move1 = FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 1");
      FlexECS::Entity move2 = FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 2");
      FlexECS::Entity move3 = FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 3");
      FLX_STRING_GET(move1.GetComponent<Sprite>()->sprite_handle) =
        (battle.current_move == &battle.current_character->move_one) ? ("/images/battle ui/Battle_UI_Skill_Selected.png")
                                                        : ("/images/battle ui/Battle_UI_Skill_Unselected.png");
      FLX_STRING_GET(move2.GetComponent<Sprite>()->sprite_handle) =
        (battle.current_move == &battle.current_character->move_two) ? ("/images/battle ui/Battle_UI_Skill_Selected.png")
                                                        : ("/images/battle ui/Battle_UI_Skill_Unselected.png");
      FLX_STRING_GET(move3.GetComponent<Sprite>()->sprite_handle) =
        (battle.current_move == &battle.current_character->move_three) ? ("/images/battle ui/Battle_UI_Skill_Selected.png")
                                                        : ("/images/battle ui/Battle_UI_Skill_Unselected.png");
    }

#pragma endregion

#pragma endregion


#pragma region Resolve Game State

#pragma region Alive/Dead Resolution

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

        // update alive count
        if (character->character_id <= 2)
          battle.drifter_alive_count--;
        else
          battle.enemy_alive_count--;

        // play death animation
        auto entity = FlexECS::Scene::GetActiveScene()->GetEntityByName(character->name);
        auto& animator = *entity.GetComponent<Animator>();
        switch (character->character_id)
        {
        case 1:
          animator.spritesheet_handle =
            FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Death_Anim_Sheet.flxspritesheet)");
          break;
        case 2:
          animator.spritesheet_handle =
            FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Death_Anim_Sheet.flxspritesheet)");
          break;
        case 3:
          animator.spritesheet_handle =
            FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_01_Death_Anim_Sheet.flxspritesheet)");
          // FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
          //   FLX_STRING_NEW(R"(/audio/robot death.mp3)");
          // FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
          break;
        case 4:
          animator.spritesheet_handle =
            FLX_STRING_NEW(R"(/images/spritesheets/Char_Enemy_02_Death_Anim_Sheet.flxspritesheet)");
          // FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
          //   FLX_STRING_NEW(R"(/audio/robot death.mp3)");
          // FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;
          break;
        case 5:
          // goto jack death cutscene
          break;
        }
        animator.should_play = true;
        animator.is_looping = false;
        animator.return_to_default = false;
        animator.frame_time = 0.f;
        animator.current_frame = 0;
      }
    }

#pragma endregion

#pragma region Game Over Resolution

    // insta win/lose
    if (Input::GetKeyDown(GLFW_KEY_9)) battle.enemy_alive_count = 0;
    if (Input::GetKeyDown(GLFW_KEY_0)) battle.drifter_alive_count = 0;

    // check if the battle is over
    if (battle.enemy_alive_count == 0 && !battle.is_win)
    {
      battle.is_win = true;

      // A bit lame, but need to find by name to set, like the old Unity days
      FlexECS::Scene::GetEntityByName("win audio").GetComponent<Audio>()->should_play = true;
      FlexECS::Scene::GetEntityByName("renko text").GetComponent<Transform>()->is_active = true;
      FlexECS::Scene::GetEntityByName("completion time value").GetComponent<Transform>()->is_active = true;
      FlexECS::Scene::GetEntityByName("dmg value").GetComponent<Transform>()->is_active = true;
      FlexECS::Scene::GetEntityByName("dmg dealt").GetComponent<Transform>()->is_active = true;
      FlexECS::Scene::GetEntityByName("Press any button").GetComponent<Transform>()->is_active = true;
      FlexECS::Scene::GetEntityByName("Win base").GetComponent<Transform>()->is_active = true;
      FlexECS::Scene::GetEntityByName("Player Stats").GetComponent<Transform>()->is_active = true;
      FlexECS::Scene::GetEntityByName("UI_Lose_V").GetComponent<Transform>()->is_active = true;
    }
    else if (battle.drifter_alive_count == 0 && !battle.is_lose)
    {
      battle.is_lose = true;
      
      FlexECS::Scene::GetEntityByName("lose audio").GetComponent<Audio>()->should_play = true;
      FlexECS::Scene::GetEntityByName("Press any button").GetComponent<Transform>()->is_active = true;
      FlexECS::Scene::GetEntityByName("Lose Base").GetComponent<Transform>()->is_active = true;
      FlexECS::Scene::GetEntityByName("git gud noob").GetComponent<Transform>()->is_active = true;
      FlexECS::Scene::GetEntityByName("UI_Lose_V").GetComponent<Transform>()->is_active = true;
    }

#pragma endregion

#pragma region Speed Bar Resolution

    // sort the speed bar slots
    // 0 is the fastest so use < instead of >
    std::sort(
      battle.speed_bar.begin(), battle.speed_bar.end(),
      [](const _Character* a, const _Character* b)
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

#pragma endregion

#pragma endregion


#pragma region Update Displays

#pragma region Healthbar Display

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
      position->position.x = healthbar->original_position.x - static_cast<float>(healthbar->pixelLength/2) * (1.0-health_percentage);

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

#pragma region Targeting Display

    // show the target
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
              transform.is_active = (character_slot.slot_number == (battle.target + 1));
          }
          else transform.is_active = (character_slot.slot_number == (battle.target + 3));
      }
      else
        transform.is_active = false;
    }

#pragma endregion

#pragma region Speed Bar Display

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

#pragma endregion

#pragma endregion
  }

} // namespace Game
