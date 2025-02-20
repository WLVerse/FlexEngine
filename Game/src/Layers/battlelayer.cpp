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

    int current_health = 0;
    int current_speed = 0;
    int current_slot = 0;          // 0-4, 0-1 for drifters, 0-4 for enemies
    bool is_alive = true;
    int current_selected_move = 0; // 0 no selection, 1 2 3 otherwise
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
    int target = 0; // 1-5, pointing out which enemy slot is the target
    std::array<Vector3, 7> sprite_slot_positions = {};
    // std::array<Vector3, 7> speedbar_slot_positions; // needed for animated speed bar
    std::array<Vector3, 7> healthbar_slot_positions = {};

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

      _Move move_one;
      if (character_asset.move_one != "None")
      {
        auto& move_one_asset = FLX_ASSET_GET(Asset::Move, character_asset.move_one);
        move_one.name = move_one_asset.name;
        move_one.damage = move_one_asset.damage;
        move_one.speed = move_one_asset.speed;
        move_one.description = move_one_asset.description;
        character.move_one = move_one;
      }

      _Move move_two;
      if (character_asset.move_two != "None")
      {
        auto& move_two_asset = FLX_ASSET_GET(Asset::Move, character_asset.move_two);
        move_two.name = move_two_asset.name;
        move_two.damage = move_two_asset.damage;
        move_two.speed = move_two_asset.speed;
        move_two.description = move_two_asset.description;
        character.move_two = move_two;
      }

      _Move move_three;
      if (character_asset.move_three != "None")
      {
        auto& move_three_asset = FLX_ASSET_GET(Asset::Move, character_asset.move_three);
        move_three.name = move_three_asset.name;
        move_three.damage = move_three_asset.damage;
        move_three.speed = move_three_asset.speed;
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

      _Move move_one;
      if (character_asset.move_one != "None")
      {
        auto& move_one_asset = FLX_ASSET_GET(Asset::Move, character_asset.move_one);
        move_one.name = move_one_asset.name;
        move_one.damage = move_one_asset.damage;
        move_one.speed = move_one_asset.speed;
        character.move_one = move_one;
      }

      _Move move_two;
      if (character_asset.move_two != "None")
      {
        auto& move_two_asset = FLX_ASSET_GET(Asset::Move, character_asset.move_two);
        move_two.name = move_two_asset.name;
        move_two.damage = move_two_asset.damage;
        move_two.speed = move_two_asset.speed;
        character.move_two = move_two;
      }

      _Move move_three;
      if (character_asset.move_three != "None")
      {
        auto& move_three_asset = FLX_ASSET_GET(Asset::Move, character_asset.move_three);
        move_three.name = move_three_asset.name;
        move_three.damage = move_three_asset.damage;
        move_three.speed = move_three_asset.speed;
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
    File& file = File::Open(Path::current("assets/saves/battlescene.flxscene"));
    FlexECS::Scene::SetActiveScene(FlexECS::Scene::Load(file));

    CameraManager::SetMainGameCameraID(FlexECS::Scene::GetEntityByName("Camera"));

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
      // note that the system doesn’t deal with duplicates
      // dupes break the targetting system and anything that uses GetEntityByName
      int index = 0;
      FlexECS::Entity e;
      auto scene = FlexECS::Scene::GetActiveScene();

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
      }
      for (auto& character : battle.enemies)
      {
        e = scene->CreateEntity(character.name + " Healthbar"); // can always use GetEntityByName to find the entity
        e.AddComponent<Healthbar>({});
        e.AddComponent<Transform>({});
        e.AddComponent<Position>({ battle.healthbar_slot_positions[character.current_slot + 2] });
        e.AddComponent<Rotation>({});
        e.AddComponent<Scale>({ Vector3(.1, .1, 0) });
        e.GetComponent<Healthbar>()->original_position = e.GetComponent<Position>()->position;
        e.GetComponent<Healthbar>()->original_scale = e.GetComponent<Scale>()->scale;
        e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_BattleScreen_HealthBar_Red.png)") });
        e.AddComponent<ZIndex>({ 35 });

        e = scene->CreateEntity(character.name + " Stats"); // can always use GetEntityByName to find the entity
        e.AddComponent<Transform>({});
        e.AddComponent<Position>({ battle.sprite_slot_positions[character.current_slot + 2] + Vector3(-105, -100, 0) });
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
      }

  #pragma endregion

    main_camera = FlexECS::Scene::GetEntityByName("Camera");

    // Just set some random shit as the target for the start of game lmao
    battle.target = 1;
  }

  void BattleLayer::OnDetach()
  {
    // Make sure nothing carries over in the way of sound
    FMODWrapper::Core::ForceStop();
  }

  void BattleLayer::Update()
  {
    bool move_one_click = Application::MessagingSystem::Receive<bool>("MoveOne clicked");
    bool move_two_click = Application::MessagingSystem::Receive<bool>("MoveTwo clicked");
    bool move_three_click = Application::MessagingSystem::Receive<bool>("MoveThree clicked");

    // check for escape key
    // this goes back to the main menu
    if (Input::GetKeyDown(GLFW_KEY_ESCAPE))
    {
      // set the main camera
      CameraManager::SetMainGameCameraID(main_camera);

      // unload win layer
      auto win_layer = Application::GetCurrentWindow()->GetLayerStack().GetOverlay("Win Layer");
      if (win_layer != nullptr) FLX_COMMAND_REMOVE_WINDOW_OVERLAY("Game", win_layer);

      // unload lose layer
      auto lose_layer = Application::GetCurrentWindow()->GetLayerStack().GetOverlay("Lose Layer");
      if (lose_layer != nullptr) FLX_COMMAND_REMOVE_WINDOW_OVERLAY("Game", lose_layer);
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
    _Character* current_character = battle.speed_bar.front();
    auto current_character_entity = FlexECS::Scene::GetActiveScene()->GetEntityByName(current_character->name);
    auto& current_character_animator = *current_character_entity.GetComponent<Animator>();

    FLX_ASSERT(current_character != nullptr, "Current character is null.");
    FLX_ASSERT(current_character->is_alive, "Current character is dead.");

    // determine if it's the player's turn
    // player is 1-2, enemy is 3-5
    battle.is_player_turn = (current_character->character_id <= 2);

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
        battle.sprite_slot_positions[2 + battle.previous_character->current_slot];
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
#endif

    // Just swapped from enemy phase to player phase
    if (battle.prev_state != battle.is_player_turn && battle.is_player_turn)
    {
      // Plays sound if swap from enemy phase to player phase
      FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->audio_file =
        FLX_STRING_NEW(R"(/audio/start turn.mp3)");
      FlexECS::Scene::GetActiveScene()->GetEntityByName("Play SFX").GetComponent<Audio>()->should_play = true;

      // Defaults target selection
      for (int i{0}; i < battle.enemy_slots.size(); ++i)
      {
        if (battle.enemy_slots[i]->is_alive && battle.enemy_slots[i] != nullptr)
        {
          battle.target = i + 1;
          break;
        }
      }

      battle.prev_state = true; // dont forget to reset, of course...
    }


#pragma endregion


#pragma region AI Move

    // enemy AI
    if (!battle.is_player_turn)
    {
      // randomly pick a target
      _Character* target_character = nullptr;
      while (target_character == nullptr) target_character = battle.drifter_slots[Range<int>(0, 1).Get()];

      // randomly pick a move
      // TODO: not yet
      int move_num = Range<int>(0, 2).Get();

      _Move* move = &current_character->move_one;

      switch (move_num)
      {
      case 0:
          move = &current_character->move_one;
          break;
      case 1:
          move = &current_character->move_two;
          break;
      case 2:
          move = &current_character->move_three;
          break;
      }

      // Temporarily move the character
      battle.previous_character = current_character;
      battle.previous_character_entity = current_character_entity;
      current_character_entity.GetComponent<Position>()->position = battle.sprite_slot_positions[target_character->current_slot];
      
      // apply the move
      target_character->current_health -= move->damage;
      target_character->current_speed += move->damage;

      // update the character's speed
      current_character->current_speed += current_character->speed + move->speed;

      // reset the target
      battle.target = 0;

      // play the animation
#pragma region Animation

      switch (current_character->character_id)
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

      auto target_entity = FlexECS::Scene::GetActiveScene()->GetEntityByName(target_character->name);
      auto& target_animator = *target_entity.GetComponent<Animator>();
      switch (target_character->character_id)
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
    if (battle.is_player_turn)
    {
      if (Input::GetKeyDown(GLFW_KEY_1))
        battle.target = 1;
      else if (Input::GetKeyDown(GLFW_KEY_2))
        battle.target = 2;
      else if (Input::GetKeyDown(GLFW_KEY_3))
        battle.target = 3;
      else if (Input::GetKeyDown(GLFW_KEY_4))
        battle.target = 4;
      else if (Input::GetKeyDown(GLFW_KEY_5))
        battle.target = 5;

      // Unselect illegal choices
      if (battle.target != 0)
      {
        // if targeting nothing, untarget
        if (battle.enemy_slots[battle.target - 1] == nullptr) battle.target = 0;

        // if targeting dead enemy, untarget
        if (battle.enemy_slots[battle.target - 1] != nullptr && !battle.enemy_slots[battle.target - 1]->is_alive)
          battle.target = 0;
      }
    }

#pragma endregion

#pragma region Moves
    // Executes if player turn and target is selected and some move is already selected
    if (battle.is_player_turn && battle.target != 0)
    {
      _Move* current_move = nullptr;

      if (move_one_click
        || (Input::GetKeyDown(GLFW_KEY_Z) && current_character->current_selected_move == 1))
      {
        current_move = &current_character->move_one;

        switch (current_character->character_id)
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

      if (move_two_click
        || Input::GetKeyDown(GLFW_KEY_X) && current_character->current_selected_move == 2)
      {
        current_move = &current_character->move_two;

        switch (current_character->character_id)
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

      // Ultimate move
      if (move_three_click
        || Input::GetKeyDown(GLFW_KEY_C) && current_character->current_selected_move == 3)
      {
        current_move = &current_character->move_three;

        switch (current_character->character_id)
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

      // if a move is selected, resolve the move
      if (current_move != nullptr)
      {
        // get the target character
        _Character* target_character = battle.enemy_slots[battle.target - 1];
        if (target_character == nullptr)
        {
          Log::Error("Target not found.");
          return;
        }

        // temporarily move character
        battle.previous_character = current_character;
        battle.previous_character_entity = current_character_entity;
        current_character_entity.GetComponent<Position>()->position = battle.sprite_slot_positions[2 + target_character->current_slot];

        // apply the move
        target_character->current_health -= current_move->damage;
        target_character->current_speed += current_move->damage;

        // update the character's speed
        current_character->current_speed += current_character->speed + current_move->speed;

        // reset the target
        battle.target = 0;

        // play the animation
        float animation_time =
          FLX_ASSET_GET(Asset::Spritesheet, FLX_STRING_GET(current_character_animator.spritesheet_handle))
            .total_frame_time;
        current_character_animator.should_play = true;
        current_character_animator.is_looping = false;
        current_character_animator.return_to_default = true;
        current_character_animator.frame_time = 0.f;
        current_character_animator.current_frame = 0;

        auto target_entity = FlexECS::Scene::GetActiveScene()->GetEntityByName(target_character->name);
        auto& target_animator = *target_entity.GetComponent<Animator>();
        switch (target_character->character_id)
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
        current_character->current_selected_move = 0;
        std::string& description = FLX_STRING_GET(
          FlexECS::Scene::GetActiveScene()->GetEntityByName("Move Description Text").GetComponent<Text>()->text
        );
        std::string& move1 =
          FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 1 Text").GetComponent<Text>()->text);
        std::string& move2 =
          FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 2 Text").GetComponent<Text>()->text);
        std::string& move3 =
          FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 3 Text").GetComponent<Text>()->text);
        description = "";
        move1 = "";
        move2 = "";
        move3 = "";

        battle.prev_state = false; // Just swapped from player phase to enemy phase (even if its the player turn next, take it as so.)
      }
    }

    // Sets the description for the current character and locks the current selected move for the next loop to consume
    if (battle.is_player_turn && current_character->current_speed <= 0)
    {
      if (Input::GetKeyDown(GLFW_KEY_Z) || move_one_click)
      {
        current_character->current_selected_move = 1;
        std::string& description = FLX_STRING_GET(
          FlexECS::Scene::GetActiveScene()->GetEntityByName("Move Description Text").GetComponent<Text>()->text
        );
        description = current_character->move_one.description;
      }
      if (Input::GetKeyDown(GLFW_KEY_X) || move_two_click)
      {
        current_character->current_selected_move = 2;
        std::string& description = FLX_STRING_GET(
          FlexECS::Scene::GetActiveScene()->GetEntityByName("Move Description Text").GetComponent<Text>()->text
        );
        description = current_character->move_two.description;
      }
      if (Input::GetKeyDown(GLFW_KEY_C) || move_three_click)
      {
        current_character->current_selected_move = 3;
        std::string& description = FLX_STRING_GET(
          FlexECS::Scene::GetActiveScene()->GetEntityByName("Move Description Text").GetComponent<Text>()->text
        );
        description = current_character->move_three.description;
      }

      FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 1 Text").GetComponent<Text>()->text) =
        "[Z] " + current_character->move_one.name;
      FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 2 Text").GetComponent<Text>()->text) =
        "[X] " + current_character->move_two.name;
      FLX_STRING_GET(FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 3 Text").GetComponent<Text>()->text) =
        "[C] " + current_character->move_three.name;

      FlexECS::Entity move1 = FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 1");
      FlexECS::Entity move2 = FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 2");
      FlexECS::Entity move3 = FlexECS::Scene::GetActiveScene()->GetEntityByName("Move 3");
      FLX_STRING_GET(move1.GetComponent<Sprite>()->sprite_handle) =
        (current_character->current_selected_move == 1) ? ("/images/battle ui/Battle_UI_Skill_Selected.png")
                                                        : ("/images/battle ui/Battle_UI_Skill_Unselected.png");
      FLX_STRING_GET(move2.GetComponent<Sprite>()->sprite_handle) =
        (current_character->current_selected_move == 2) ? ("/images/battle ui/Battle_UI_Skill_Selected.png")
                                                        : ("/images/battle ui/Battle_UI_Skill_Unselected.png");
      FLX_STRING_GET(move3.GetComponent<Sprite>()->sprite_handle) =
        (current_character->current_selected_move == 3) ? ("/images/battle ui/Battle_UI_Skill_Selected.png")
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
      // load win layer
      FLX_COMMAND_ADD_WINDOW_OVERLAY("Game", std::make_shared<WinLayer>());
    }
    else if (battle.drifter_alive_count == 0 && !battle.is_lose)
    {
      battle.is_lose = true;
      // load lose layer
      FLX_COMMAND_ADD_WINDOW_OVERLAY("Game", std::make_shared<LoseLayer>());
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

      // update the scale
      entity.GetComponent<Text>()->text = FLX_STRING_NEW(stats);
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
      if (battle.target != 0)
        transform.is_active = (character_slot.slot_number == (battle.target + 2));
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
