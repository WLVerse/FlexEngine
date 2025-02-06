/*#include <FlexEngine.h>
#include <character.h>
using namespace FlexEngine;

/*enum Targets
{
    single_target_ally = 1,
    next_fastest_ally = 2,
    all_allies = 3,
    single_target_enemy = 4,
    next_fastest_enemy = 5,
    all_enemies = 6
};

class Move
{
public:
    std::string name = "";
    std::string description = "";
    int speed = 0;
    int damage = 0;
    std::vector<std::tuple<std::string, int, int>> effect;
};

class Character
{
public:
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
    //FlexECS::Entity Audio;
   //FlexECS::Entity Animation;
   
    void Character::Awake()
    {
        Log::Debug("CharacterScript: Awake");

        //find character_name, skill_one_name, skill_two_name, skill_three_name
        //character_name = FLX_STRING_GET(*self.GetComponent<EntityName>());
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
        }
        //Vector3{ char_position.x + button_offset.x, char_position.y + button_offset.y, 0 } // first button is built of the position of character + button offset
        //Vector3{ char_position.x + button_offset.x + 10, char_position.y + button_offset.y + 5, 0 } //buttons & text are built off position of the first button

        if (character_name != "Renko" && character_name != "Grace" && character_name != "Ash")
        {
            is_player = false;
        }
        Find(character_name_text, character_name + " Name");
        Find(character_hp_bar, character_name + " HP Bar");
        Find(character_hp_text, character_name + " HP Text");
        if (is_player)
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

        for (int i = 1; i < 4; i++)
        {
            switch (i)
            {
            case 1:
                GetMove(character_name + std::to_string(i), skill_one);
                break;
            case 2:
                GetMove(character_name + std::to_string(i), skill_two);
                break;
            case 3:
                GetMove(character_name + std::to_string(i), skill_three);
                break;
            }
        }
        character_name_text.GetComponent<Text>()->text = FLX_STRING_NEW(character_name);
        skill_one_text.GetComponent<Text>()->text = FLX_STRING_NEW(skill_one.name);
        skill_two_text.GetComponent<Text>()->text = FLX_STRING_NEW(skill_two.name);
        skill_three_text.GetComponent<Text>()->text = FLX_STRING_NEW(skill_three.name);
        skill_text.GetComponent<Text>()->text = FLX_STRING_NEW(skill_one.description);
    }

    void Character::Start()
    {
        Log::Debug("CharacterScript: Start");

        current_hp = base_hp;
        current_speed = base_speed;
        character_hp_text.GetComponent<Text>()->text = FLX_STRING_NEW("HP: " + std::to_string(current_hp) + "/" + std::to_string(base_hp) + " | " + "bSpd: " + std::to_string(base_speed));
        attack_buff_duration = 0;
        attack_debuff_duration = 0;
        invuln_buff_duration = 0;
        stun_debuff_duration = 0;

        if (skill_border.GetComponent<Transform>()->is_active)
        {
            ToggleSkill();
        }
    }

    void Character::Update()
    {
        //Log::Debug("CharacterScript: Update");
        //if (skill_one_button.GetComponent<Button>()->on)
    }

    void Character::Find(FlexECS::Entity& obj, std::string obj_name)
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

    void Character::GetMove(std::string move_name, Move move_num)
    {
            std::string file_name = "assets/data/" + move_name + ".txt";
            //std::stringstream ss(FLX_STRING_NEW(file_name));
            std::ifstream ss(file_name);

            if (!ss)
            {
                Log::Info("File not found!");
                return;
            }

            std::string line = "";

            std::getline(ss, line);
            move_num.name = line;

            std::getline(ss, line);
            move_num.description = line;
            
            std::getline(ss, line);
            move_num.speed = std::stoi(line);

            while (std::getline(ss, line)) {
                std::tuple<std::string, int, int> current_effect;
                std::string current = "";
                int phase = 0;
                for (char& c : line) {
                    if (c == ',')
                    {
                        switch (phase)
                        {
                        case 0:
                            std::get<0>(current_effect) = current;
                            break;
                        case 1:
                            std::get<1>(current_effect) = std::stoi(current);
                            break;
                        case 2:
                            std::get<2>(current_effect) = std::stoi(current);
                            break;
                        }
                        current = "";
                        phase++;
                    }
                    else
                    {
                        current = current + c;
                    }
                }
                move_num.effect.push_back(current_effect);
            }
        move_num.damage = std::get<1>(move_num.effect[0]);
        Log::Info("Name: " + move_num.name);
        Log::Info("Info: " + move_num.description);
        Log::Info("DMG: " + std::to_string(move_num.damage));
        Log::Info("SPD: " + std::to_string(move_num.speed));
        for (unsigned i = 0; i < move_num.effect.size(); i++) {
            Log::Info("Effect: " + std::get<0>(move_num.effect[i]));
            Log::Info("DMG/Number of Turns: " + std::to_string(std::get<1>(move_num.effect[i])));

            std::string target = "";
            switch (std::get<2>(move_num.effect[i]))
            {
            case 1:
                target = "single_target_ally";
                break;
            case 2:
                target = "next_fastest_ally";
                break;
            case 3:
                target = "all_allies";
                break;
            case 4:
                target = "single_target_enemy";
                break;
            case 5:
                target = "next_fastest_enemy";
                break;
            case 6:
                target = "all_enemies";
                break;
            }
            Log::Info("Targets: " + target);
        }
    }

    int Character::TakeDamage(int incoming_damage)
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

    void Character::ToggleSkill()
    {
        if (skill_one_button.GetComponent<Transform>()->is_active)
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

    void Character::ToggleSkillText()
    {
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

    void Character::UpdateEffect()
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
//};
    */