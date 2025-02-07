// 
// Audio implementation in the layer
//
// AUTHORS
// [100%] Yew Chong (yewchong.k\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2025 DigiPen, All rights reserved.

#include "Layers.h"
#include "battlelayer.h"
#include "../../Scripts/src/character.h"

#include <thread>

namespace Editor
{
    std::vector<FlexECS::Entity> characters;
    std::vector<FlexECS::Entity> ally_characters;
    std::vector<FlexECS::Entity> enemy_characters;
    std::vector<void (*)()> effects;

    FlexECS::Entity win_screen;
    FlexECS::Entity lose_screen;
    FlexECS::Entity speed_bar;

    FlexECS::Entity renko_portrait;
    FlexECS::Entity renko_icon;
    FlexECS::Entity grace_portrait;
    FlexECS::Entity grace_icon;
    FlexECS::Entity jack_portrait;
    FlexECS::Entity jack_icon;

    FlexECS::Entity current_character;
    FlexECS::Entity initial_target;
    std::vector<FlexECS::Entity> current_targets;

    //Character Renko;
    //Character Grace;
    //Character Jack;

    bool move_selected = false;

    Vector3 barPos[3] = { { 483,470,0 }, { 483,400,0 }, { 483,330,0 } };

    static int target_num;

    static int boss_move;

    float delay_timer;

    void BattleLayer::BossSelect()
    {
        boss_move++;

        if (boss_move == 1)
        {
            current_character.GetComponent<Character>()->pending_skill = current_character.GetComponent<Character>()->skill_one;
        }
        else if (boss_move == 2)
        {
            current_character.GetComponent<Character>()->pending_skill = current_character.GetComponent<Character>()->skill_two;
        }
        else
        {
            current_character.GetComponent<Character>()->pending_skill = current_character.GetComponent<Character>()->skill_three;
        }
        current_character.GetComponent<Character>()->skill_text.GetComponent<Text>()->text = current_character.GetComponent<Character>()->pending_skill.description;
    }

    void BattleLayer::StartOfTurn()
    {
        SpeedSort();
        int value = characters[0].GetComponent<Character>()->current_speed;
        for (unsigned i = 0; i < characters.size(); i++) {
            characters[i].GetComponent<Character>()->current_speed -= value;
            Log::Info(FLX_STRING_GET(characters[i].GetComponent<Character>()->character_name) + std::to_string(characters[i].GetComponent<Character>()->current_speed));
            if (characters[i].GetComponent<Character>()->attack_buff_duration > 0)
            {
                characters[i].GetComponent<Character>()->attack_buff_duration--;
            }
            if (characters[i].GetComponent<Character>()->attack_debuff_duration > 0)
            {
                characters[i].GetComponent<Character>()->attack_debuff_duration--;
            }
            if (characters[i].GetComponent<Character>()->invuln_buff_duration > 0)
            {
                characters[i].GetComponent<Character>()->invuln_buff_duration--;
            }
            if (characters[i].GetComponent<Character>()->stun_debuff_duration > 0)
            {
                characters[i].GetComponent<Character>()->stun_debuff_duration--;
            }
            UpdateEffect(*characters[i].GetComponent<Character>());
        }
        current_character = characters[0];
        MoveSelect();
        std::string message = FLX_STRING_GET(current_character.GetComponent<Character>()->character_name) + "'s turn!";
        Log::Info(message);
    }

    void BattleLayer::MoveSelect()
    {
        if (current_character.GetComponent<Character>()->is_player)
        {
            ToggleSkill(*current_character.GetComponent<Character>());
            move_selected = false;
        }
        else
        {
            //random target, but doesn't matter for this boss fight.
            initial_target = ally_characters[0];
            MoveExecution();
            MoveResolution();
        }
    }
    void BattleLayer::MoveResolution()
    {
        if (current_character.GetComponent<Character>()->is_player)
        {
            MoveExecution();
            ToggleSkill(*current_character.GetComponent<Character>());
            EndOfTurn();
        }
        else
        {
            //random gen, but for this build boss uses 1->2->3->3
            BossSelect();
            EndOfTurn();
        }
    }

    void BattleLayer::EndOfTurn()
    {
        for (unsigned i = 0; i < characters.size(); i++) {
            if (characters[i].GetComponent<Character>()->current_hp <= 0)
            {
                /*for (unsigned j = 0; j < ally_characters.size(); j++) {
                    if (characters[i] == ally_characters[j])
                    {
                        characters.erase(ally_characters.begin() + j);
                    }
                }
                for (unsigned j = 0; j < enemy_characters.size(); j++) {
                    if (characters[i] == enemy_characters[j])
                    {
                        characters.erase(enemy_characters.begin() + j);
                    }
                }*/
                if (FLX_STRING_GET(characters[i].GetComponent<Character>()->character_name) == "Renko")
                {
                    renko_portrait.GetComponent<Transform>()->is_active = false;
                    renko_icon.GetComponent<Transform>()->is_active = false;
                }
                else if (FLX_STRING_GET(characters[i].GetComponent<Character>()->character_name) == "Grace")
                {
                    grace_portrait.GetComponent<Transform>()->is_active = false;
                    grace_icon.GetComponent<Transform>()->is_active = false;
                }
                else if (FLX_STRING_GET(characters[i].GetComponent<Character>()->character_name) == "Jack")
                {
                    jack_portrait.GetComponent<Transform>()->is_active = false;
                    jack_icon.GetComponent<Transform>()->is_active = false;
                }
                std::string message = characters[i].GetComponent<Character>()->character_name + " has fallen!";
                Log::Info(message);
                //characters.erase(characters.begin() + i);
            }
        }
        if (!renko_portrait.GetComponent<Transform>()->is_active)
        {
            lose_screen.GetComponent<Transform>()->is_active = true;
            std::string message = "You lose!";
            Log::Info(message);
        }
        else if (!jack_portrait.GetComponent<Transform>()->is_active)
        {
            win_screen.GetComponent<Transform>()->is_active = true;
            std::string message = "You win!";
            Log::Info(message);
        }
        else
        {
            StartOfTurn();
        }
    }

    void BattleLayer::SpeedSort()
    {
        for (unsigned i = 0; i < characters.size(); i++) {
            for (unsigned j = i + 1; j < characters.size(); j++)
            {
                if (characters[i].GetComponent<Character>()->current_speed > characters[j].GetComponent<Character>()->current_speed)
                {
                    std::swap(characters[i], characters[j]);
                }
            }
        }
        for (unsigned i = 0; i < ally_characters.size(); i++) {
            for (unsigned j = i + 1; j < ally_characters.size(); j++)
            {
                if (ally_characters[i].GetComponent<Character>()->current_speed > ally_characters[j].GetComponent<Character>()->current_speed)
                {
                    std::swap(ally_characters[i], ally_characters[j]);
                }
            }
        }
        for (unsigned i = 0; i < enemy_characters.size(); i++) {
            for (unsigned j = i + 1; j < enemy_characters.size(); j++)
            {
                if (enemy_characters[i].GetComponent<Character>()->current_speed > enemy_characters[j].GetComponent<Character>()->current_speed)
                {
                    std::swap(enemy_characters[i], enemy_characters[j]);
                }
            }
        }
        for (unsigned i = 0; i < characters.size(); i++) {
            Log::Info(FLX_STRING_GET(characters[i].GetComponent<Character>()->character_name) + std::to_string(characters[i].GetComponent<Character>()->current_speed));
            if (FLX_STRING_GET(characters[i].GetComponent<Character>()->character_name) == "Renko")
            {
                renko_portrait.GetComponent<Position>()->position = barPos[i];
                renko_icon.GetComponent<Position>()->position = barPos[i];
            }
            else if (FLX_STRING_GET(characters[i].GetComponent<Character>()->character_name) == "Grace")
            {
                grace_portrait.GetComponent<Position>()->position = barPos[i];
                grace_icon.GetComponent<Position>()->position = barPos[i];
            }
            else if (FLX_STRING_GET(characters[i].GetComponent<Character>()->character_name) == "Jack")
            {
                jack_portrait.GetComponent<Position>()->position = barPos[i];
                jack_icon.GetComponent<Position>()->position = barPos[i];
            }
        }
    }

    void BattleLayer::MoveExecution()
    {
        //random gen, but for this build boss uses 1->2->3->3
        //for (unsigned i = 0; i < current_character.GetComponent<Character>()->pending_skill.effect_name.size(); i++) {
            current_targets.clear();

            switch ((current_character.GetComponent<Character>()->pending_skill.target))
            {
                //single_target_ally = 1,
                    //next_fastest_ally = 2,
                    //all_allies = 3,
                    //single_target_enemy = 4,
                    //next_fastest_enemy = 5,
                    //all_enemies = 6
            //case Targets::single_target_ally:
            case 1:
                current_targets.push_back(initial_target);
                break;
            case 2:
                for (unsigned j = 0; j < ally_characters.size(); j++) {
                    if (current_character != ally_characters[j])
                    {
                        current_targets.push_back(ally_characters[j]);
                    }
                    current_targets.push_back(initial_target);
                }
                break;
            case 3:
                for (unsigned j = 0; j < ally_characters.size(); j++) {
                    current_targets.push_back(ally_characters[j]);
                }
                break;
            case 4:
                current_targets.push_back(initial_target);
                break;
            case 5:
                for (unsigned j = 0; j < enemy_characters.size(); j++) {
                    current_targets.push_back(initial_target);
                }
                break;
            case 6:
                for (unsigned j = 0; j < enemy_characters.size(); j++) {
                    current_targets.push_back(enemy_characters[j]);
                }
                break;
            }

            if (current_targets.empty())
            {
                return;
            }

            if (FLX_STRING_GET(current_character.GetComponent<Character>()->pending_skill.effect_name) == "DAMAGE")
            {
                int damage = (current_character.GetComponent<Character>()->pending_skill.damage_duration);
                int totaldamage = 0;
                std::string message = FLX_STRING_GET(current_character.GetComponent<Character>()->character_name) + " attacked ";
                if (current_character.GetComponent<Character>()->attack_buff_duration > 0)
                {
                    damage += (current_character.GetComponent<Character>()->pending_skill.damage_duration) / 2;
                }
                if (current_character.GetComponent<Character>()->attack_debuff_duration > 0)
                {
                    damage -= (current_character.GetComponent<Character>()->pending_skill.damage_duration) / 2;
                }
                for (unsigned j = 0; j < current_targets.size(); j++) {

                    totaldamage += TakeDamage(*current_targets[j].GetComponent<Character>(), damage);
                    message += FLX_STRING_GET(current_targets[j].GetComponent<Character>()->character_name) + " ";
                    if (j + 1 != current_targets.size())
                    {
                        message += "and ";
                    }
                }
                message += "with " + FLX_STRING_GET(current_character.GetComponent<Character>()->pending_skill.name) + " for a total of " + std::to_string(totaldamage) + " damage!";
                Log::Info(message);
            }
            else if (FLX_STRING_GET(current_character.GetComponent<Character>()->pending_skill.effect_name) == "ATTACK_BUFF")
            {
                int duration = (current_character.GetComponent<Character>()->pending_skill.damage_duration);
                for (unsigned j = 0; j < current_targets.size(); j++) {

                    current_targets[j].GetComponent<Character>()->attack_buff_duration += duration;
                    UpdateEffect(*current_targets[j].GetComponent<Character>());
                }
            }
            else if (FLX_STRING_GET(current_character.GetComponent<Character>()->pending_skill.effect_name) == "ATTACK_DEBUFF")
            {
                int duration = (current_character.GetComponent<Character>()->pending_skill.damage_duration);
                for (unsigned j = 0; j < current_targets.size(); j++) {

                    current_targets[j].GetComponent<Character>()->attack_debuff_duration += duration;
                    UpdateEffect(*current_targets[j].GetComponent<Character>());
                }
            }
            else if (FLX_STRING_GET(current_character.GetComponent<Character>()->pending_skill.effect_name) == "INVULN_BUFF")
            {
                int duration = (current_character.GetComponent<Character>()->pending_skill.damage_duration);
                for (unsigned j = 0; j < current_targets.size(); j++) {

                    current_targets[j].GetComponent<Character>()->invuln_buff_duration += duration;
                    UpdateEffect(*current_targets[j].GetComponent<Character>());
                }
            }
            else if (FLX_STRING_GET(current_character.GetComponent<Character>()->pending_skill.effect_name) == "STUN_DEBUFF")
            {
                int duration = (current_character.GetComponent<Character>()->pending_skill.damage_duration);
                for (unsigned j = 0; j < current_targets.size(); j++) {

                    current_targets[j].GetComponent<Character>()->stun_debuff_duration += duration;
                    UpdateEffect(*current_targets[j].GetComponent<Character>());
                }
            }
            current_character.GetComponent<Character>()->current_speed = current_character.GetComponent<Character>()->base_speed + current_character.GetComponent<Character>()->pending_skill.speed;
        }
    //}

    void BattleLayer::Find(FlexECS::Entity& obj, std::string obj_name)
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

    void BattleLayer::GetMove(std::string move_name, Character::Move& move_num)
    {
        std::string file_name = "assets/data/" + move_name + ".txt";
        std::stringstream ss(FLX_STRING_NEW(file_name));
        //std::ifstream ss(file_name);

        if (!ss)
        {
            Log::Info("File not found!");
            return;
        }
        Log::Info(file_name);
        std::string line = "";

        std::getline(ss, line);
        move_num.name = FLX_STRING_NEW(line);

        std::getline(ss, line);
        move_num.description = FLX_STRING_NEW(line);
        Log::Info(line);

        std::getline(ss, line);
        move_num.speed = std::stoi(line);

        int phase = 0;
        while (std::getline(ss, line)) {
            //Effect current_effect{};
            Log::Info(line);
            /*switch (phase)
            {
            case 0:
                move_num.effect_name = (FLX_STRING_NEW(line));
                break;
            case 1:
                move_num.damage_duration = std::stoi(line);
                break;
            case 2:
                move_num.target = std::stoi(line);
                break;
            }
            phase++;
            if (phase > 2)
            {
                phase = 0;
            }
            /*
            switch (phase)
            {
            case 0:
                ss >> line;
                {
                    move_num.effect_name.push_back(FLX_STRING_NEW(line));
                    ss >> current_num;
                    move_num.damage_duration.push_back(current_num);
                    ss >> current_num;
                    move_num.target.push_back(current_num);
                }
                //move_num.effect_name.push_back(FLX_STRING_NEW(line));
                break;
            case 1:
                move_num.damage_duration.push_back(std::atoi(line));
                break;
            case 2:
                move_num.target.push_back(std::atoi(line));
                break;
            }
            phase++;*/
            std::string current = "";
            int phase = 0;
            for (char& c : line) {
                if (c == ',')
                {
                    switch (phase)
                    {
                    case 0:
                        move_num.effect_name = (FLX_STRING_NEW(current));
                        break;
                    case 1:
                        move_num.damage_duration = (std::stoi(current));
                        break;
                    case 2:
                        move_num.target = (std::stoi(current));
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
            //.effect.push_back(current_effect);
        }
        Log::Info("Name: " + FLX_STRING_GET(move_num.name));
        Log::Info("Info: " + FLX_STRING_GET(move_num.description));
        Log::Info("SPD: " + std::to_string(move_num.speed));
        Log::Info("Effect: " + FLX_STRING_GET(move_num.effect_name));
        Log::Info("DMG/Number of Turns: " + std::to_string(move_num.damage_duration));
            std::string target = "";
            switch ((move_num.target))
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

    int BattleLayer::TakeDamage(Character& cha, int incoming_damage)
    {
        if (cha.invuln_buff_duration > 0)
            incoming_damage = 0;
        else
        {
            cha.current_hp -= incoming_damage;
            cha.character_hp_text.GetComponent<Text>()->text = FLX_STRING_NEW("HP: " + std::to_string(cha.current_hp) + "/" + std::to_string(cha.base_hp) + " | " + "bSpd: " + std::to_string(cha.base_speed));
            Log::Info(std::to_string(cha.current_hp));
        }
        return incoming_damage;
    }

    void BattleLayer::ToggleSkill(Character& cha)
    {
        if (cha.skill_one_button.GetComponent<Transform>()->is_active)
        {
            cha.skill_one_button.GetComponent<Transform>()->is_active = false;
            cha.skill_two_button.GetComponent<Transform>()->is_active = false;
            cha.skill_three_button.GetComponent<Transform>()->is_active = false;
            cha.skill_one_text.GetComponent<Transform>()->is_active = false;
            cha.skill_two_text.GetComponent<Transform>()->is_active = false;
            cha.skill_three_text.GetComponent<Transform>()->is_active = false;
            cha.skill_border.GetComponent<Transform>()->is_active = false;
            cha.skill_text.GetComponent<Transform>()->is_active = false;
        }
        else
        {
            cha.skill_one_button.GetComponent<Transform>()->is_active = true;
            cha.skill_two_button.GetComponent<Transform>()->is_active = true;
            cha.skill_three_button.GetComponent<Transform>()->is_active = true;
            cha.skill_one_text.GetComponent<Transform>()->is_active = true;
            cha.skill_two_text.GetComponent<Transform>()->is_active = true;
            cha.skill_three_text.GetComponent<Transform>()->is_active = true;
            cha.skill_border.GetComponent<Transform>()->is_active = true;
            cha.skill_text.GetComponent<Transform>()->is_active = true;
            ToggleSkillText(cha);
        }
    }

    void BattleLayer::ToggleSkillText(Character& cha)
    {
        if (cha.skill_border.GetComponent<Transform>()->is_active)
        {
            cha.skill_border.GetComponent<Transform>()->is_active = false;
            cha.skill_text.GetComponent<Transform>()->is_active = false;
        }
        else
        {
            cha.skill_border.GetComponent<Transform>()->is_active = true;
            cha.skill_text.GetComponent<Transform>()->is_active = true;
        }
    }

    void BattleLayer::UpdateEffect(Character& cha)
    {
        if (cha.attack_buff_duration > 0)
        {
            cha.attack_buff.GetComponent<Transform>()->is_active = true;
        }
        else
        {
            cha.attack_buff.GetComponent<Transform>()->is_active = false;
        }
        if (cha.attack_debuff_duration > 0)
        {
            cha.attack_debuff.GetComponent<Transform>()->is_active = true;
        }
        else
        {
            cha.attack_debuff.GetComponent<Transform>()->is_active = false;
        }
        if (cha.invuln_buff_duration > 0)
        {
            cha.invuln_buff.GetComponent<Transform>()->is_active = true;
        }
        else
        {
            cha.invuln_buff.GetComponent<Transform>()->is_active = false;
        }
        if (cha.stun_debuff_duration > 0)
        {
            cha.stun_debuff.GetComponent<Transform>()->is_active = true;
        }
        else
        {
            cha.stun_debuff.GetComponent<Transform>()->is_active = false;
        }
    }


    void BattleLayer::OnAttach()
    {
        auto scene = FlexECS::Scene::GetActiveScene();
        win_screen = scene->GetEntityByName("Win Screen");
        if (win_screen)
        {
            Log::Info("Found " + FLX_STRING_GET(*win_screen.GetComponent<EntityName>()));
        }
        else
        {
            Log::Error("Entity not found");
        }

        lose_screen = scene->GetEntityByName("Lose Screen");
        if (lose_screen)
        {
            Log::Info("Found " + FLX_STRING_GET(*win_screen.GetComponent<EntityName>()));
        }
        else
        {
            Log::Error("Entity not found");
        }

        speed_bar = scene->GetEntityByName("Speed Bar");
        if (speed_bar)
        {
            Log::Info("Found " + FLX_STRING_GET(*win_screen.GetComponent<EntityName>()));
        }
        else
        {
            Log::Error("Entity not found");
        }

        renko_portrait = scene->GetEntityByName("Renko Portrait");
        if (renko_portrait)
        {
            Log::Info("Found " + FLX_STRING_GET(*renko_portrait.GetComponent<EntityName>()));
        }
        else
        {
            Log::Error("Entity not found");
        }
        renko_portrait.GetComponent<Transform>()->is_active = true;

        renko_icon = scene->GetEntityByName("Renko Icon");
        if (renko_icon)
        {
            Log::Info("Found " + FLX_STRING_GET(*renko_icon.GetComponent<EntityName>()));
        }
        else
        {
            Log::Error("Entity not found");
        }
        renko_icon.GetComponent<Transform>()->is_active = true;

        grace_portrait = scene->GetEntityByName("Grace Portrait");
        if (grace_portrait)
        {
            Log::Info("Found " + FLX_STRING_GET(*grace_portrait.GetComponent<EntityName>()));
        }
        else
        {
            Log::Error("Entity not found");
        }
        grace_portrait.GetComponent<Transform>()->is_active = true;

        grace_icon = scene->GetEntityByName("Grace Icon");
        if (grace_icon)
        {
            Log::Info("Found " + FLX_STRING_GET(*grace_icon.GetComponent<EntityName>()));
        }
        else
        {
            Log::Error("Entity not found");
        }
        grace_icon.GetComponent<Transform>()->is_active = true;

        jack_portrait = scene->GetEntityByName("Jack Portrait");
        if (jack_portrait)
        {
            Log::Info("Found " + FLX_STRING_GET(*jack_portrait.GetComponent<EntityName>()));
        }
        else
        {
            Log::Error("Entity not found");
        }
        jack_portrait.GetComponent<Transform>()->is_active = true;

        jack_icon = scene->GetEntityByName("Jack Icon");
        if (jack_icon)
        {
            Log::Info("Found " + FLX_STRING_GET(*jack_icon.GetComponent<EntityName>()));
        }
        else
        {
            Log::Error("Entity not found");
        }
        jack_portrait.GetComponent<Transform>()->is_active = true;

        lose_screen.GetComponent<Transform>()->is_active = false;
        win_screen.GetComponent<Transform>()->is_active = false;
        characters.clear();
        ally_characters.clear();
        enemy_characters.clear();
        for (FlexECS::Entity& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Character>())
        {
            //entity.GetComponent<Character>()->character_name = (*entity.GetComponent<EntityName>());
            Find(entity.GetComponent<Character>()->character_sprite, FLX_STRING_GET(entity.GetComponent<Character>()->character_name));
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

            if (FLX_STRING_GET(entity.GetComponent<Character>()->character_name) != "Renko" && FLX_STRING_GET(entity.GetComponent<Character>()->character_name) != "Grace" && FLX_STRING_GET(entity.GetComponent<Character>()->character_name) != "Ash")
            {
                entity.GetComponent<Character>()->is_player = false;
            }
            Find(entity.GetComponent<Character>()->character_name_text, FLX_STRING_GET(entity.GetComponent<Character>()->character_name) + " Name");
            Find(entity.GetComponent<Character>()->character_hp_bar, FLX_STRING_GET(entity.GetComponent<Character>()->character_name) + " HP Bar");
            Find(entity.GetComponent<Character>()->character_hp_text, FLX_STRING_GET(entity.GetComponent<Character>()->character_name) + " HP Text");
            Find(entity.GetComponent<Character>()->skill_border, FLX_STRING_GET(entity.GetComponent<Character>()->character_name) + " Skill Border");
            Find(entity.GetComponent<Character>()->skill_text, FLX_STRING_GET(entity.GetComponent<Character>()->character_name) + " Skill Text");

            if (entity.GetComponent<Character>()->is_player)
            {
                Find(entity.GetComponent<Character>()->skill_one_button, FLX_STRING_GET(entity.GetComponent<Character>()->character_name) + " S1 Button");
                Find(entity.GetComponent<Character>()->skill_two_button, FLX_STRING_GET(entity.GetComponent<Character>()->character_name) + " S2 Button");
                Find(entity.GetComponent<Character>()->skill_three_button, FLX_STRING_GET(entity.GetComponent<Character>()->character_name) + " S3 Button");
                Find(entity.GetComponent<Character>()->skill_one_text, FLX_STRING_GET(entity.GetComponent<Character>()->character_name) + " S1 Text");
                Find(entity.GetComponent<Character>()->skill_two_text, FLX_STRING_GET(entity.GetComponent<Character>()->character_name) + " S2 Text");
                Find(entity.GetComponent<Character>()->skill_three_text, FLX_STRING_GET(entity.GetComponent<Character>()->character_name) + " S3 Text");
                entity.GetComponent<Character>()->skill_one_text.GetComponent<Text>()->text = entity.GetComponent<Character>()->skill_one.name;
                entity.GetComponent<Character>()->skill_two_text.GetComponent<Text>()->text = entity.GetComponent<Character>()->skill_two.name;
                entity.GetComponent<Character>()->skill_three_text.GetComponent<Text>()->text = entity.GetComponent<Character>()->skill_three.name;
                if (entity.GetComponent<Character>()->skill_one_button.GetComponent<Transform>()->is_active)
                {
                    ToggleSkill(*entity.GetComponent<Character>());
                }
            }
            else
            {
                Find(entity.GetComponent<Character>()->skill_enemy_button, FLX_STRING_GET(entity.GetComponent<Character>()->character_name) + " Skill Button");
            }

            Find(entity.GetComponent<Character>()->attack_buff, FLX_STRING_GET(entity.GetComponent<Character>()->character_name) + " Attack Buff");
            Find(entity.GetComponent<Character>()->attack_debuff, FLX_STRING_GET(entity.GetComponent<Character>()->character_name) + " Attack Debuff");
            Find(entity.GetComponent<Character>()->invuln_buff, FLX_STRING_GET(entity.GetComponent<Character>()->character_name) + " Invuln Buff");
            Find(entity.GetComponent<Character>()->stun_debuff, FLX_STRING_GET(entity.GetComponent<Character>()->character_name) + " Stun Debuff");

                /*for (int i = 1; i < 4; i++)
                {
                    switch (i)
                    {
                    case 1:
                        GetMove(FLX_STRING_GET(entity.GetComponent<Character>()->character_name) + std::to_string(i), entity.GetComponent<Character>()->skill_one);
                        break;
                    case 2:
                        GetMove(FLX_STRING_GET(entity.GetComponent<Character>()->character_name) + std::to_string(i), entity.GetComponent<Character>()->skill_two);
                        break;
                    case 3:
                        GetMove(FLX_STRING_GET(entity.GetComponent<Character>()->character_name) + std::to_string(i), entity.GetComponent<Character>()->skill_three);
                        break;
                    }
                }*/
            entity.GetComponent<Character>()->character_name_text.GetComponent<Text>()->text = entity.GetComponent<Character>()->character_name;
            entity.GetComponent<Character>()->skill_text.GetComponent<Text>()->text = entity.GetComponent<Character>()->skill_one.description;

            entity.GetComponent<Character>()->current_hp = entity.GetComponent<Character>()->base_hp;
            entity.GetComponent<Character>()->current_speed = entity.GetComponent<Character>()->base_speed;
            entity.GetComponent<Character>()->character_hp_text.GetComponent<Text>()->text = FLX_STRING_NEW("HP: " + std::to_string(entity.GetComponent<Character>()->current_hp) + "/" + std::to_string(entity.GetComponent<Character>()->base_hp) + " | " + "bSpd: " + std::to_string(entity.GetComponent<Character>()->base_speed));
            entity.GetComponent<Character>()->attack_buff_duration = 0;
            entity.GetComponent<Character>()->attack_debuff_duration = 0;
            entity.GetComponent<Character>()->invuln_buff_duration = 0;
            entity.GetComponent<Character>()->stun_debuff_duration = 0;

            if (!entity.GetComponent<Transform>()->is_active)
            {
                entity.GetComponent<Transform>()->is_active = true;
            }
            characters.push_back(entity);
            if (entity.GetComponent<Character>()->is_player)
            {
                ally_characters.push_back(entity);
            }
            else
            {
                enemy_characters.push_back(entity);
            }
        }

        for (unsigned i = 0; i < enemy_characters.size(); i++) {
            //random gen, but for this build boss uses 1->2->3->3
            current_character = enemy_characters[i];
            boss_move = 0;
            BossSelect();
        }

        StartOfTurn();
    }

    void BattleLayer::OnDetach()
    {

    }

    void BattleLayer::Update()
    {
        if (current_character.GetComponent<Character>()->is_player && current_character.GetComponent<Character>()->skill_one_button.GetComponent<Transform>()->is_active)
        {
            if (Input::GetKeyDown(GLFW_KEY_1))
            {
                current_character.GetComponent<Character>()->pending_skill = current_character.GetComponent<Character>()->skill_one;
                Log::Info("Move " + (current_character.GetComponent<Character>()->pending_skill.name));
                Log::Info("Move " + (current_character.GetComponent<Character>()->skill_one.name));
                if ((current_character.GetComponent<Character>()->pending_skill.target) <= 3)
                {
                    initial_target = ally_characters[0];
                }
                else
                {
                    initial_target = enemy_characters[0];
                }
                current_character.GetComponent<Character>()->skill_text.GetComponent<Text>()->text = (current_character.GetComponent<Character>()->skill_one.description);
                target_num = 0;
                move_selected = true;
                std::string message = current_character.GetComponent<Character>()->character_name + " will use " + FLX_STRING_GET(current_character.GetComponent<Character>()->skill_one.name) + " targeting " + FLX_STRING_GET(initial_target.GetComponent<Character>()->character_name);
                Log::Info(message);
            }
            else if (Input::GetKeyDown(GLFW_KEY_2))
            {
                current_character.GetComponent<Character>()->pending_skill = current_character.GetComponent<Character>()->skill_two;
                if ((current_character.GetComponent<Character>()->pending_skill.target) <= 3)
                {
                    initial_target = ally_characters[0];
                }
                else
                {
                    initial_target = enemy_characters[0];
                }
                current_character.GetComponent<Character>()->skill_text.GetComponent<Text>()->text = (current_character.GetComponent<Character>()->skill_two.description);
                target_num = 0;
                move_selected = true;
                std::string message = current_character.GetComponent<Character>()->character_name + " will use " + FLX_STRING_GET(current_character.GetComponent<Character>()->skill_two.name) + " targeting " + FLX_STRING_GET(initial_target.GetComponent<Character>()->character_name);
                Log::Info(message);
            }
            else if (Input::GetKeyDown(GLFW_KEY_3))
            {
                current_character.GetComponent<Character>()->pending_skill = current_character.GetComponent<Character>()->skill_three;
                if ((current_character.GetComponent<Character>()->pending_skill.target) <= 3)
                {
                    initial_target = ally_characters[0];
                }
                else
                {
                    initial_target = enemy_characters[0];
                }
                current_character.GetComponent<Character>()->skill_text.GetComponent<Text>()->text = (current_character.GetComponent<Character>()->skill_three.description);
                target_num = 0;
                move_selected = true;
                std::string message = current_character.GetComponent<Character>()->character_name + " will use " + FLX_STRING_GET(current_character.GetComponent<Character>()->skill_three.name) + " targeting " + FLX_STRING_GET(initial_target.GetComponent<Character>()->character_name);
                Log::Info(message);
            }

            if (Input::GetKeyDown(GLFW_KEY_Q))
            {
                target_num--;
                if ((current_character.GetComponent<Character>()->pending_skill.target) <= 3)
                {
                    if (target_num >= 0 && target_num < ally_characters.size())
                    {
                        initial_target = ally_characters[target_num];
                    }
                    else
                    {
                        target_num = 0;
                        initial_target = ally_characters[target_num];
                    }
                }
                else
                {
                    if (target_num >= 0 && target_num < enemy_characters.size())
                    {
                        initial_target = enemy_characters[target_num];
                    }
                    else
                    {
                        target_num = 0;
                        initial_target = enemy_characters[target_num];
                    }
                }
                std::string message = current_character.GetComponent<Character>()->character_name + " will use " + FLX_STRING_GET(current_character.GetComponent<Character>()->pending_skill.name) + " targeting " + FLX_STRING_GET(initial_target.GetComponent<Character>()->character_name);
                Log::Info(message);
            }
            else if (Input::GetKeyDown(GLFW_KEY_E))
            {
                target_num++;
                if ((current_character.GetComponent<Character>()->pending_skill.target) <= 3)
                {
                    if (target_num >= 0 && target_num < ally_characters.size())
                    {
                        initial_target = ally_characters[target_num];
                    }
                    else
                    {
                        target_num = int(ally_characters.size()) - 1;
                        initial_target = ally_characters[target_num];
                    }
                }
                else
                {
                    if (target_num >= 0 && target_num < enemy_characters.size())
                    {
                        initial_target = enemy_characters[target_num];
                    }
                    else
                    {
                        target_num = int(enemy_characters.size()) - 1;
                        initial_target = enemy_characters[target_num];
                    }
                }
                std::string message = current_character.GetComponent<Character>()->character_name + " will use " + FLX_STRING_GET(current_character.GetComponent<Character>()->pending_skill.name) + " targeting " + FLX_STRING_GET(initial_target.GetComponent<Character>()->character_name);
                Log::Info(message);
            }

            if (Input::GetKeyDown(GLFW_KEY_SPACE) && move_selected)
            {
                MoveResolution();
            }
        }

        if (delay_timer > 0.0f)
        {
            //delay_timer -= deltaTime;
        }

    }
}
