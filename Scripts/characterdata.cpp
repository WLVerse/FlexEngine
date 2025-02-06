#include <FlexEngine.h>
using namespace FlexEngine;

class CharacterData
{
public:

    bool player = true;

    std::string character_name = "";
    int base_speed = 5;
    int current_speed = 0;
    int base_hp = 10;
    int current_hp = 0;
    std::string skill_one_name = "", skill_two_name = "", skill_three_name = "";
    std::string skill_one_description = "", skill_two_description = "", skill_three_description = "";
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

    void Load()
    {
        Log::Debug("CharacterScript: Awake");

        //find character_name, skill_one_name, skill_two_name, skill_three_name
        character_name = FLX_STRING_GET(*self.GetComponent<EntityName>());
        if (character_name != "Renko" || character_name != "Grace" || character_name != "Ash")
        {
            player = false;
        }
        //for (FlexECS::Entity& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, Script>())
        //{
            //if ()
            //Transform& transform = *entity.GetComponent<Transform>();
        //}
        Find(character_sprite, character_name);
        /* character_sprite = scene->CreateEntity("Renko");
        entity.AddComponent<Position>({
          Vector3{ char_position.x, char_position.y, 0 }
        });
        entity.AddComponent<Rotation>({
          Vector3{ 0, -180, 180 }
        });
        entity.AddComponent<Scale>({
          { 40, 93, 1 }
        });
        entity.AddComponent<Transform>({});
        // entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW(R"(CharacterInfo)") });
        entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/chrono_drift_renko.png)"), -1 });
        //entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Renko_Idle_Attack_Anim_Sheet.flxspritesheet)"),
        //true, 0.f }); entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
        }*/
        //Vector3{ char_position.x + button_offset.x, char_position.y + button_offset.y, 0 } // first button is built of the position of character + button offset
        //Vector3{ char_position.x + button_offset.x + 10, char_position.y + button_offset.y + 5, 0 } //buttons & text are built off position of the first button
        Find(character_name_text, character_name + " Name");
        Find(character_hp_bar, character_name + " HP Bar");
        Find(character_hp_text, character_name + " HP Text");
        if (player)
        {
            Find(skill_one_button, character_name + " S1 Button");
            Find(skill_two_button, character_name + " S2 Button");
            Find(skill_three_button, character_name + " S3 Button");
            Find(skill_one_text, character_name + " S1 Text");
            Find(skill_two_text, character_name + " S2 Text");
            Find(skill_three_text, character_name + " S3 Text");
        }
        else
        {
            Find(skill_enemy_button, character_name + " Skill Button");
        }
        Find(skill_border, character_name + " Skill Border");
        Find(skill_text, character_name + " Skill Text");

        Find(attack_buff, character_name + " Attack Buff");
        Find(attack_debuff, character_name + " Attack Debuff");
        Find(invuln_buff, character_name + " Invuln Buff");
        Find(stun_debuff, character_name + " Stun Debuff");
    }

    void OnStart()
    {
        Log::Debug("CharacterScript: Start");

        character_name_text.GetComponent<Text>()->text = FLX_STRING_NEW(character_name);
        character_hp_text.GetComponent<Text>()->text = FLX_STRING_NEW("HP: " + std::to_string(base_hp) + "/" + std::to_string(base_hp) + " | " + "bSpd: " + std::to_string(base_speed));
        skill_one_text.GetComponent<Text>()->text = FLX_STRING_NEW(skill_one_name);
        skill_two_text.GetComponent<Text>()->text = FLX_STRING_NEW(skill_two_name);
        skill_three_text.GetComponent<Text>()->text = FLX_STRING_NEW(skill_three_name);

        ToggleSkill();
    }

    void Find(FlexECS::Entity& obj, std::string obj_name)
    {
        auto scene = FlexECS::Scene::GetActiveScene();
        obj = scene->GetEntityByName(obj_name);
        if (obj)
        {
            Log::Info("Found " + FLX_STRING_GET(*obj.GetComponent<EntityName>()));
        }
        else
        {
            Log::Error("Entity not found");
        }
    }

    void ToggleSkill()
    {
        if (skill_border.GetComponent<Transform>()->is_active)
        {
            skill_one_button.GetComponent<Transform>()->is_active = false;
            skill_two_button.GetComponent<Transform>()->is_active = false;
            skill_three_button.GetComponent<Transform>()->is_active = false;
            skill_one_text.GetComponent<Transform>()->is_active = false;
            skill_two_text.GetComponent<Transform>()->is_active = false;
            skill_three_text.GetComponent<Transform>()->is_active = false;
            skill_border.GetComponent<Transform>()->is_active = false;
            skill_text.GetComponent<Transform>()->is_active = false;
        }
        else
        {
            skill_one_button.GetComponent<Transform>()->is_active = true;
            skill_two_button.GetComponent<Transform>()->is_active = true;
            skill_three_button.GetComponent<Transform>()->is_active = true;
            skill_one_text.GetComponent<Transform>()->is_active = true;
            skill_two_text.GetComponent<Transform>()->is_active = true;
            skill_three_text.GetComponent<Transform>()->is_active = true;
            skill_border.GetComponent<Transform>()->is_active = true;
            skill_text.GetComponent<Transform>()->is_active = true;
        }
    }

    void ToggleSkillText()
    {
        skill_text.GetComponent<Text>()->text = FLX_STRING_NEW(skill_one_description);
        if (skill_border.GetComponent<Transform>()->is_active)
        {
            skill_border.GetComponent<Transform>()->is_active = false;
            skill_text.GetComponent<Transform>()->is_active = false;
        }
        else
        {
            skill_border.GetComponent<Transform>()->is_active = true;
            skill_text.GetComponent<Transform>()->is_active = true;
        }
    }
};