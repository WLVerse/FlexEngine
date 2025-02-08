#pragma once
#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game
{
  enum Targets
  {
    single_target_ally = 1,
    next_fastest_ally = 2,
    all_allies = 3,
    single_target_enemy = 4,
    next_fastest_enemy = 5,
    all_enemies = 6
  };

  class Character
  {
  public:
    class Move
    {
    public:
      std::string name = "";
      std::string description = "";
      int speed = 0;
      int damage = 0;
      std::vector<std::tuple<std::string, int, int>> effect;
    };

    bool is_player = true;

    std::string character_name = "";
    int base_speed = 5;
    int current_speed = 0;
    int base_hp = 10;
    int current_hp = 0;
    Move skill_one, skill_two, skill_three;
    Move pending_skill;
    int attack_buff_duration = 0, attack_debuff_duration = 0, invuln_buff_duration = 0, stun_debuff_duration = 0;

    Vector2 char_position;
    Vector2 name_offset;
    Vector2 hp_offset;
    Vector2 button_offset;
    Vector2 description_offset;
    Vector2 buff_offset;

    FlexECS::Entity character_sprite;
    FlexECS::Entity character_name_text;
    FlexECS::Entity character_hp_bar;
    FlexECS::Entity character_hp_text;

    FlexECS::Entity skill_one_button, skill_two_button, skill_three_button;
    FlexECS::Entity chrono_gear_button;
    FlexECS::Entity skill_one_text, skill_two_text, skill_three_text;
    FlexECS::Entity chrono_gear_text;
    FlexECS::Entity skill_enemy_button;

    FlexECS::Entity skill_border, skill_text;
    FlexECS::Entity button_text;

    FlexECS::Entity attack_buff, attack_debuff, invuln_buff, stun_debuff;
    // FlexECS::Entity Audio;
    // FlexECS::Entity Animation;

    void Awake();

    void Start();

    void Update();

    void Find(FlexECS::Entity& obj, std::string obj_name);

    void GetMove(std::string move_name, Move move_num);

    int TakeDamage(int incoming_damage);

    void ToggleSkill();

    void ToggleSkillText();

    void ChooseMoveOne();

    void UpdateEffect();
  };
} // namespace Game
