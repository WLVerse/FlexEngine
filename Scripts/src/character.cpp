#include <FlexEngine.h>
using namespace FlexEngine;

class CharacterScript : public IScript
{
public:
    CharacterScript() { ScriptRegistry::RegisterScript(this); }
    std::string GetName() const override { return "CharacterTest"; }

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
   

    void Awake() override
    {
        Log::Debug("CharacterScript: Awake");

        //find character_name, skill_one_name, skill_two_name, skill_three_name
        character_name = FLX_STRING_GET(*self.GetComponent<EntityName>());
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

    void Start() override
    {
        Log::Debug("CharacterScript: Start");

        character_name_text.GetComponent<Text>()->text = FLX_STRING_NEW(character_name);
        character_hp_text.GetComponent<Text>()->text = FLX_STRING_NEW("HP: " + std::to_string(base_hp) + "/" + std::to_string(base_hp) + " | " + "bSpd: " + std::to_string(base_speed));
        skill_one_text.GetComponent<Text>()->text = FLX_STRING_NEW(skill_one_name);
        skill_two_text.GetComponent<Text>()->text = FLX_STRING_NEW(skill_two_name);
        skill_three_text.GetComponent<Text>()->text = FLX_STRING_NEW(skill_three_name);

        ToggleSkill();
    }

    void Update() override
    {
        //Log::Debug("CharacterScript: Update");
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

    /* GetMove(std::string skill_one_name, std::string name)
    {
        Asset::FlxData& move = FLX_ASSET_GET(Asset::FlxData, "/data/" + name + ".flxdata");
        Move result;
        skill_one_name = move.GetString("name", "");
        result.description = move.GetString("description", "");
        result.cost = move.GetInt("speed_cost", 0);
        result.is_target_enemy = move.GetBool("is_target_enemy", false);
        result.target_type = break_down_numerical_string[move.GetString("target_type", "")];

        std::stringstream ss(move.GetString("functions", ""));
        std::array<std::string, 3> tokens;

        while (std::getline(ss, tokens[0], ',')) {
            std::getline(ss, tokens[1], ',');
            std::getline(ss, tokens[2], ',');
            for (std::string& token : tokens) {
                for (std::string::iterator i = token.begin(); i != token.end(); i++) {
                    if (*i == ' ') {
                        token.erase(i);
                        i = token.begin();
                    }
                }
            }
            int status_duration = std::stoi(tokens[1]);
            int move_value = std::stoi(tokens[2]);
            if (status_duration == 0 || move_value == 0) {
                MoveExecution read_move;
                read_move.move_function = s_move_function_registry[tokens[0]];
                read_move.value = (move_value == 0) ? status_duration : move_value;
                result.move_function_container.push_back(read_move);
            }
            else {
                StatusEffectApplication status_effect_move;
                status_effect_move.duration = status_duration;
                status_effect_move.value = move_value;
                status_effect_move.effect_function = s_status_function_registry[tokens[0]];
                result.sea_function_container.push_back(status_effect_move);
            }
        }
        result.move_value = move.GetInt("move_value", 0);
        result.move_function = s_move_function_registry[move.GetString("move_function", "")];
        result.effect_value = move.GetInt("effect_value", 0);
        result.effect_duration = move.GetInt("effect_duration", 0);
        if (move.GetString("effect_function", "") != "") {
            result.effect_function = s_status_function_registry[move.GetString("effect_function", "")];
        }

        return result;
    } */

    int TakeDamage(int incoming_damage)
    {
        if (invuln_buff_duration > 0)
            incoming_damage = 0;
        else
        {
            current_hp -= incoming_damage;
            character_hp_text.GetComponent<Text>()->text = FLX_STRING_NEW("HP: " + std::to_string(current_hp) + "/" + std::to_string(base_hp) + " | " + "bSpd: " + std::to_string(base_speed));
        }
            return incoming_damage;
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

    void UpdateEffect()
    {
        if (attack_buff_duration > 0)
        {
            attack_buff.GetComponent<Transform>()->is_active = true;
        }
        else
        {
            attack_buff.GetComponent<Transform>()->is_active = false;
        }
        if (attack_debuff_duration > 0)
        {
            attack_debuff.GetComponent<Transform>()->is_active = true;
        }
        else
        {
            attack_debuff.GetComponent<Transform>()->is_active = false;
        }
        if (invuln_buff_duration > 0)
        {
            invuln_buff.GetComponent<Transform>()->is_active = true;
        }
        else
        {
            invuln_buff.GetComponent<Transform>()->is_active = false;
        }
        if (stun_debuff_duration > 0)
        {
            stun_debuff.GetComponent<Transform>()->is_active = true;
        }
        else
        {
            stun_debuff.GetComponent<Transform>()->is_active = false;
        }
    }

    void Stop() override
    {
        Log::Debug("CharacterScript: Stop");
    }
};

// Static instance to ensure registration
static CharacterScript CharacterTest;
