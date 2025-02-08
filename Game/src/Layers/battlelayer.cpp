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

  // internal oop data structure to store the battle data
  // we're doing this because our team is struggling with the ECS system

#pragma region Battle Data

  struct _Move
  {
    std::string name;
    int damage;
    int speed;
  };

  struct _Character
  {
    std::string name;
    int character_id; // 1-5
    int health;
    int speed;
    _Move move_one;
    _Move move_two;
    _Move move_three;

    int current_health;
    int current_speed;
  };

  struct _Battle
  {
    std::vector<_Character> drifter_slots;
    std::vector<_Character> enemy_slots;
    int boss_battle = 0; // 0 = false, 1-5 = true, pointing out which enemy slot is the boss

    std::vector<_Character*> speed_bar;
  };

  _Battle battle;

  void Internal_ParseBattle(AssetKey assetkey)
  {
    // get the battle asset
    auto& asset = FLX_ASSET_GET(Asset::Battle, assetkey);
    // parse the battle data
    for (auto& slot : asset.GetDrifterSlots())
    {
      _Character character;
      if (*slot == "None") continue;
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

      battle.drifter_slots.push_back(character);
    }
    for (auto& slot : asset.GetEnemySlots())
    {
      _Character character;
      if (*slot == "None") continue;
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

      battle.enemy_slots.push_back(character);
    }
    battle.boss_battle = asset.boss_battle;

    // can't have empty drifter or enemy vector
    FLX_ASSERT(battle.drifter_slots.size() > 0, "Drifter slots cannot be empty.");
    FLX_ASSERT(battle.enemy_slots.size() > 0, "Enemy slots cannot be empty.");
  }

#pragma endregion

#pragma region OOP Animation System

  // always default to idle
  // attack, hurt, and ult will return to idle
  // death will stop on the last frame

  struct AnimationData
  {
    FlexECS::Scene::StringIndex spritesheet_handle;
    int frame_count;
    float frame_time;
    bool looping;
  };

  // Global animation storage (external to ECS)
  // Converts animation data to ECS savable components
  struct AnimationSystem
  {
    std::vector<AnimationData> animations;

    int RegisterAnimation(FlexECS::Scene::StringIndex spritesheet, int frame_count, float frame_time, bool looping)
    {
      animations.push_back({ spritesheet, frame_count, frame_time, looping });
      return animations.size() - 1; // Return animation index
    }

    const AnimationData* GetAnimation(int index) const
    {
      return (index >= 0 && index < animations.size()) ? &animations[index] : nullptr;
    }
  };

  // Create animation system instance
  AnimationSystem animation_system;


#pragma endregion

  void BattleLayer::OnAttach()
  {
#pragma region Scene Generation
#if 1

    auto scene = FlexECS::Scene::CreateScene();
    FlexECS::Scene::SetActiveScene(scene);
    FlexECS::Entity e = FlexECS::Entity::Null;

  #define WIDTH Application::GetCurrentWindow()->GetWidth()
  #define HEIGHT Application::GetCurrentWindow()->GetHeight()

    e = scene->CreateEntity("Camera");
    e.AddComponent<Transform>({});
    e.AddComponent<Position>({});
    e.AddComponent<Rotation>({});
    e.AddComponent<Scale>({});
    Camera gameCamera({ WIDTH / 2.f, HEIGHT / 2.f, 0 }, (float)WIDTH, (float)HEIGHT, -2.0f, 2.0f);
    e.AddComponent<Camera>(gameCamera);

  #pragma region Backgrounds

    e = scene->CreateEntity("Mockup");
    e.AddComponent<Transform>({});
    e.AddComponent<Position>({ Vector3(WIDTH / 2.f, HEIGHT / 2.f, 0.0f) });
    e.AddComponent<Rotation>({ Vector3(0.f, 0.f, 0.f) });
    e.AddComponent<Scale>({ Vector3((float)WIDTH, (float)HEIGHT, 0.0f) });
    e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/Battle UI_04_Revamp_05.png)"), -1 });
    e.GetComponent<Sprite>()->center_aligned = true;
    e.AddComponent<ZIndex>({ 0 });

    e = scene->CreateEntity("Background");
    e.AddComponent<Transform>({});
    e.AddComponent<Position>({ Vector3(WIDTH / 2.f, HEIGHT / 2.f, 0.0f) });
    e.AddComponent<Rotation>({ Vector3(0.f, 0.f, 0.f) });
    e.AddComponent<Scale>({ Vector3((float)WIDTH, (float)HEIGHT, 0.0f) });
    e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/Background_Final.jpg)"), -1 });
    e.GetComponent<Sprite>()->center_aligned = true;
    e.AddComponent<ZIndex>({ 0 });

  #pragma endregion

  #pragma region Character Slots

    // slots for the characters
    e = scene->CreateEntity("Drifter Slot 1");
    e.AddComponent<Transform>({});
    e.AddComponent<Position>({ Vector3(200, 200, 0) });
    e.AddComponent<Rotation>({});
    e.AddComponent<Scale>({ Vector3(100, 100, 0) });
    e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_Indicator_Enemy.png)") });
    e.AddComponent<Animator>(
      { FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
        FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Attack_Anim_Sheet.flxspritesheet)") }
    );
    e.AddComponent<ZIndex>({ 30 });
    e.AddComponent<CharacterSlot>({ 1 });

    e = scene->CreateEntity("Drifter Slot 2");
    e.AddComponent<Transform>({});
    e.AddComponent<Position>({ Vector3(200, 300, 0) });
    e.AddComponent<Rotation>({});
    e.AddComponent<Scale>({ Vector3(100, 100, 0) });
    e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_Indicator_Enemy.png)") });
    e.AddComponent<ZIndex>({ 30 });
    e.AddComponent<CharacterSlot>({ 2 });

    e = scene->CreateEntity("Enemy Slot 1");
    e.AddComponent<Transform>({});
    e.AddComponent<Position>({ Vector3(1400, 200, 0) });
    e.AddComponent<Rotation>({});
    e.AddComponent<Scale>({ Vector3(100, 100, 0) });
    e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_Indicator_Enemy.png)") });
    e.AddComponent<ZIndex>({ 30 });
    e.AddComponent<CharacterSlot>({ 3 });

    e = scene->CreateEntity("Enemy Slot 2");
    e.AddComponent<Transform>({});
    e.AddComponent<Position>({ Vector3(1400, 300, 0) });
    e.AddComponent<Rotation>({});
    e.AddComponent<Scale>({ Vector3(100, 100, 0) });
    e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_Indicator_Enemy.png)") });
    e.AddComponent<ZIndex>({ 30 });
    e.AddComponent<CharacterSlot>({ 4 });

    e = scene->CreateEntity("Enemy Slot 3");
    e.AddComponent<Transform>({});
    e.AddComponent<Position>({ Vector3(1400, 400, 0) });
    e.AddComponent<Rotation>({});
    e.AddComponent<Scale>({ Vector3(100, 100, 0) });
    e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_Indicator_Enemy.png)") });
    e.AddComponent<ZIndex>({ 30 });
    e.AddComponent<CharacterSlot>({ 5 });

    e = scene->CreateEntity("Enemy Slot 4");
    e.AddComponent<Transform>({});
    e.AddComponent<Position>({ Vector3(1400, 500, 0) });
    e.AddComponent<Rotation>({});
    e.AddComponent<Scale>({ Vector3(100, 100, 0) });
    e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_Indicator_Enemy.png)") });
    e.AddComponent<ZIndex>({ 30 });
    e.AddComponent<CharacterSlot>({ 6 });

    e = scene->CreateEntity("Enemy Slot 5");
    e.AddComponent<Transform>({});
    e.AddComponent<Position>({ Vector3(1400, 600, 0) });
    e.AddComponent<Rotation>({});
    e.AddComponent<Scale>({ Vector3(100, 100, 0) });
    e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_Indicator_Enemy.png)") });
    e.AddComponent<ZIndex>({ 30 });
    e.AddComponent<CharacterSlot>({ 7 });

  #pragma endregion

  #pragma region Moves

    e = scene->CreateEntity("Move Accent");
    e.AddComponent<Transform>({});
    e.AddComponent<Position>({ Vector3(500, 300, 0) });
    e.AddComponent<Rotation>({});
    e.AddComponent<Scale>({ Vector3(100, 100, 0) });
    e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_Skill_Accent.png)") });
    e.AddComponent<ZIndex>({ 1 });

    e = scene->CreateEntity("Move 1");
    e.AddComponent<Transform>({});
    e.AddComponent<Position>({ Vector3(700, 300, 0) });
    e.AddComponent<Rotation>({});
    e.AddComponent<Scale>({ Vector3(100, 100, 0) });
    e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_Skill_Description.png)") });
    e.AddComponent<ZIndex>({ 1 });
    e.AddComponent<BoundingBox2D>({});
    e.AddComponent<Button>({});
    e.AddComponent<Script>({ FLX_STRING_NEW(R"(MoveOne)") });
    e.AddComponent<MoveOneComponent>({});

    e = scene->CreateEntity("Move 2");
    e.AddComponent<Transform>({});
    e.AddComponent<Position>({ Vector3(700, 400, 0) });
    e.AddComponent<Rotation>({});
    e.AddComponent<Scale>({ Vector3(100, 100, 0) });
    e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_Skill_Description.png)") });
    e.AddComponent<ZIndex>({ 1 });
    e.AddComponent<BoundingBox2D>({});
    e.AddComponent<Button>({});
    e.AddComponent<Script>({ FLX_STRING_NEW(R"(MoveTwo)") });
    e.AddComponent<MoveTwoComponent>({});

    e = scene->CreateEntity("Move 3");
    e.AddComponent<Transform>({});
    e.AddComponent<Position>({ Vector3(700, 500, 0) });
    e.AddComponent<Rotation>({});
    e.AddComponent<Scale>({ Vector3(100, 100, 0) });
    e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_Skill_Description.png)") });
    e.AddComponent<ZIndex>({ 1 });
    e.AddComponent<BoundingBox2D>({});
    e.AddComponent<Button>({});
    e.AddComponent<Script>({ FLX_STRING_NEW(R"(MoveThree)") });
    e.AddComponent<MoveThreeComponent>({});

    e = scene->CreateEntity("Move Description");
    e.AddComponent<Transform>({});
    e.AddComponent<Position>({ Vector3(900, 300, 0) });
    e.AddComponent<Rotation>({});
    e.AddComponent<Scale>({ Vector3(100, 100, 0) });
    e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_Skill_Description.png)") });
    e.AddComponent<ZIndex>({ 1 });

  #pragma endregion

  #pragma region Speed bar

    e = scene->CreateEntity("Speed bar left");
    e.AddComponent<Transform>({});
    e.AddComponent<Position>({ Vector3(400, 900, 0) });
    e.AddComponent<Rotation>({});
    e.AddComponent<Scale>({ Vector3(-100, 100, 0) });
    e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_SpeedBar_Accent_01.png)") });
    e.AddComponent<ZIndex>({ 1 });

    e = scene->CreateEntity("Speed bar right");
    e.AddComponent<Transform>({});
    e.AddComponent<Position>({ Vector3(1400, 900, 0) });
    e.AddComponent<Rotation>({});
    e.AddComponent<Scale>({ Vector3(100, 100, 0) });
    e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_SpeedBar_Accent_01.png)") });
    e.AddComponent<ZIndex>({ 1 });

    e = scene->CreateEntity("Speed slot 1");
    e.AddComponent<Transform>({});
    e.AddComponent<Position>({ Vector3(600, 900, 0) });
    e.AddComponent<Rotation>({});
    e.AddComponent<Scale>({ Vector3(100, 100, 0) });
    e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_SpeedBar_AllyProfile_Empty.png)") });
    e.AddComponent<ZIndex>({ 1 });
    e.AddComponent<SpeedBarSlot>({});
    e.GetComponent<SpeedBarSlot>()->slot_number = 1;
    e.GetComponent<SpeedBarSlot>()->character = 1;

    e = scene->CreateEntity("Speed slot 2");
    e.AddComponent<Transform>({});
    e.AddComponent<Position>({ Vector3(700, 900, 0) });
    e.AddComponent<Rotation>({});
    e.AddComponent<Scale>({ Vector3(100, 100, 0) });
    e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_SpeedBar_AllyProfile_Empty.png)") });
    e.AddComponent<ZIndex>({ 1 });
    e.AddComponent<SpeedBarSlot>({});
    e.GetComponent<SpeedBarSlot>()->slot_number = 2;
    e.GetComponent<SpeedBarSlot>()->character = 2;

    e = scene->CreateEntity("Speed slot 3");
    e.AddComponent<Transform>({});
    e.AddComponent<Position>({ Vector3(800, 900, 0) });
    e.AddComponent<Rotation>({});
    e.AddComponent<Scale>({ Vector3(100, 100, 0) });
    e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_SpeedBar_AllyProfile_Empty.png)") });
    e.AddComponent<ZIndex>({ 1 });
    e.AddComponent<SpeedBarSlot>({});
    e.GetComponent<SpeedBarSlot>()->slot_number = 3;
    e.GetComponent<SpeedBarSlot>()->character = 3;

    e = scene->CreateEntity("Speed slot 4");
    e.AddComponent<Transform>({});
    e.AddComponent<Position>({ Vector3(900, 900, 0) });
    e.AddComponent<Rotation>({});
    e.AddComponent<Scale>({ Vector3(100, 100, 0) });
    e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_SpeedBar_AllyProfile_Empty.png)") });
    e.AddComponent<ZIndex>({ 1 });
    e.AddComponent<SpeedBarSlot>({});
    e.GetComponent<SpeedBarSlot>()->slot_number = 4;
    e.GetComponent<SpeedBarSlot>()->character = 4;

    e = scene->CreateEntity("Speed slot 5");
    e.AddComponent<Transform>({});
    e.AddComponent<Position>({ Vector3(1000, 900, 0) });
    e.AddComponent<Rotation>({});
    e.AddComponent<Scale>({ Vector3(100, 100, 0) });
    e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_SpeedBar_AllyProfile_Empty.png)") });
    e.AddComponent<ZIndex>({ 1 });
    e.AddComponent<SpeedBarSlot>({});
    e.GetComponent<SpeedBarSlot>()->slot_number = 5;
    e.GetComponent<SpeedBarSlot>()->character = 3;

    e = scene->CreateEntity("Speed slot 6");
    e.AddComponent<Transform>({});
    e.AddComponent<Position>({ Vector3(1100, 900, 0) });
    e.AddComponent<Rotation>({});
    e.AddComponent<Scale>({ Vector3(100, 100, 0) });
    e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_SpeedBar_AllyProfile_Empty.png)") });
    e.AddComponent<ZIndex>({ 1 });
    e.AddComponent<SpeedBarSlot>({});
    e.GetComponent<SpeedBarSlot>()->slot_number = 6;
    e.GetComponent<SpeedBarSlot>()->character = 4;

    e = scene->CreateEntity("Speed slot 7");
    e.AddComponent<Transform>({});
    e.AddComponent<Position>({ Vector3(1200, 900, 0) });
    e.AddComponent<Rotation>({});
    e.AddComponent<Scale>({ Vector3(100, 100, 0) });
    e.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_SpeedBar_AllyProfile_Empty.png)") });
    e.AddComponent<ZIndex>({ 1 });
    e.AddComponent<SpeedBarSlot>({});
    e.GetComponent<SpeedBarSlot>()->slot_number = 7;
    e.GetComponent<SpeedBarSlot>()->character = 5;

  #pragma endregion

#else
// load from flxscene
#endif
#pragma endregion

#pragma region Load Battle Data

    // load the battle
    Internal_ParseBattle(R"(/data/demo.flxbattle)");

    // init non-loaded values
    for (auto& character : battle.drifter_slots) battle.speed_bar.push_back(&character);
    for (auto& character : battle.enemy_slots) battle.speed_bar.push_back(&character);

    for (auto character : battle.speed_bar)
    {
      character->current_health = character->health;
      character->current_speed = character->speed;
    }

    // create entities for the characters using the battle data
    // each drifter will have drifter, health, speed, and 3 moves
    // each enemy will have enemy, health, speed, and 3 moves
    // position is based on the slot number
    // for (int i = 0; i < battle.drifter_slots.size(); i++)
    //{
    //  // get the location based on the slot
    //  Vector3 position = {};
    //  for (FlexECS::Entity& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Position, CharacterSlot>())
    //  {
    //    auto& characterSlot = *entity.GetComponent<CharacterSlot>();
    //    if (characterSlot.slot_number == i)
    //    {
    //      position = entity.GetComponent<Position>()->position;
    //      break;
    //    }
    //  }
    //
    //
    //}

#pragma endregion

#pragma region Load Hardcoded Animator

    // each loaded character will need one of these
    // the animator will be updated in the update loop


#pragma endregion
  }

  void BattleLayer::OnDetach()
  {
  }

  void BattleLayer::Update()
  {
    FlexECS::Scene::GetActiveScene()->GetEntityByName("Mockup").GetComponent<Transform>()->is_active =
      Input::GetKey(GLFW_KEY_SPACE);

#pragma region Debug

    // print the speed bar
    if (Input::GetKeyDown(GLFW_KEY_D))
    {
      Log::Info("Speed Bar:");
      for (auto character : battle.speed_bar)
        Log::Debug(character->name + " - " + std::to_string(character->current_speed));
    }

#pragma endregion

#pragma region Animation Tester

    // when a button is pressed, change the animation state of the
    if (Input::GetKeyDown(GLFW_KEY_U))
    {
      // get the entity
      auto entity = FlexECS::Scene::GetActiveScene()->GetEntityByName("Drifter Slot 1");
      // get the animator component
      auto& animator = *entity.GetComponent<Animator>();

      // essentially update the animator for each animation
      animator.spritesheet_handle =
        FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Attack_Anim_Sheet.flxspritesheet)");
      animator.should_play = true;
      animator.is_looping = false;
      animator.return_to_default = true;
      animator.frame_time = 0.f;
      animator.current_frame = 0;
    }

#pragma endregion

#pragma region Speed Bar Increment

    // increment the speed bar
    // this happens after clicking a move
    if (Input::GetKeyDown(GLFW_KEY_G))
    {
      Log::Info("Speed Bar Increment");

      // add the speed to the first character + a random value
      battle.speed_bar[0]->current_speed += battle.speed_bar[0]->speed + Range<int>(20, 30).Get();

      Log::Debug("New speed: " + std::to_string(battle.speed_bar[0]->current_speed));
    }

#pragma endregion

#pragma region Speed Bar Sorting

    // sort the speed bar slots
    // 0 is the fastest so use < instead of >
    std::sort(
      battle.speed_bar.begin(), battle.speed_bar.end(),
      [](const _Character* a, const _Character* b)
      {
        return a->current_speed < b->current_speed;
      }
    );

#pragma endregion

#pragma region Speed Bar Resolution

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
  }

} // namespace Game
