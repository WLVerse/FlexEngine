#include <FlexEngine.h>
#include <character.h>
using namespace FlexEngine;

class Battle
{
public:
    //std::vector<FlexECS::Entity> original_characters;
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

    bool move_selected = false;

    Vector3 barPos[3] = { { 483,470,0 }, { 483,400,0 }, { 483,330,0 } };

    static int target_num;

    static int boss_move;
    
    float delay_timer;

    void Start()
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
        renko_icon = scene->GetEntityByName("Renko Icon");
        if (renko_icon)
        {
            Log::Info("Found " + FLX_STRING_GET(*renko_icon.GetComponent<EntityName>()));
        }
        else
        {
            Log::Error("Entity not found");
        }

        grace_portrait = scene->GetEntityByName("Grace Portrait");
        if (grace_portrait)
        {
            Log::Info("Found " + FLX_STRING_GET(*grace_portrait.GetComponent<EntityName>()));
        }
        else
        {
            Log::Error("Entity not found");
        }
        grace_icon = scene->GetEntityByName("Grace Icon");
        if (grace_icon)
        {
            Log::Info("Found " + FLX_STRING_GET(*grace_icon.GetComponent<EntityName>()));
        }
        else
        {
            Log::Error("Entity not found");
        }

        jack_portrait = scene->GetEntityByName("Jack Portrait");
        if (jack_portrait)
        {
            Log::Info("Found " + FLX_STRING_GET(*jack_portrait.GetComponent<EntityName>()));
        }
        else
        {
            Log::Error("Entity not found");
        }
        jack_icon = scene->GetEntityByName("Jack Icon");
        if (jack_icon)
        {
            Log::Info("Found " + FLX_STRING_GET(*jack_icon.GetComponent<EntityName>()));
        }
        else
        {
            Log::Error("Entity not found");
        }
        characters.clear();
        ally_characters.clear();
        enemy_characters.clear();
        for (FlexECS::Entity& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Character>())
        {
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
    }

    void Update()
    {
        if (current_character.GetComponent<Character>()->is_player && current_character.GetComponent<Character>()->skill_one_button.GetComponent<Transform>()->is_active)
        {
            if (Input::GetKeyDown(GLFW_KEY_1))
            {
                current_character.GetComponent<Character>()->pending_skill = current_character.GetComponent<Character>()->skill_one;
                if (std::get<2>(current_character.GetComponent<Character>()->pending_skill.effect[0]) <= 3)
                {
                    initial_target = ally_characters[0];
                }
                else
                {
                    initial_target = enemy_characters[0];
                }
                current_character.GetComponent<Character>()->skill_text.GetComponent<Text>()->text = FLX_STRING_NEW(current_character.GetComponent<Character>()->skill_one.description);
                target_num = 0;
                move_selected = true;
                std::string message = current_character.GetComponent<Character>()->character_name + " will use " + current_character.GetComponent<Character>()->skill_one.name + " targeting " + initial_target.GetComponent<Character>()->character_name;
                Log::Info(message);
            }
            else if (Input::GetKeyDown(GLFW_KEY_2))
            {
                current_character.GetComponent<Character>()->pending_skill = current_character.GetComponent<Character>()->skill_two;
                if (std::get<2>(current_character.GetComponent<Character>()->pending_skill.effect[0]) <= 3)
                {
                    initial_target = ally_characters[0];
                }
                else
                {
                    initial_target = enemy_characters[0];
                }
                current_character.GetComponent<Character>()->skill_text.GetComponent<Text>()->text = FLX_STRING_NEW(current_character.GetComponent<Character>()->skill_two.description);
                target_num = 0;
                move_selected = true;
                std::string message = current_character.GetComponent<Character>()->character_name + " will use " + current_character.GetComponent<Character>()->skill_two.name + " targeting " + initial_target.GetComponent<Character>()->character_name;
                Log::Info(message);
            }
            else if (Input::GetKeyDown(GLFW_KEY_3))
            {
                current_character.GetComponent<Character>()->pending_skill = current_character.GetComponent<Character>()->skill_three;
                if (std::get<2>(current_character.GetComponent<Character>()->pending_skill.effect[0]) <= 3)
                {
                    initial_target = ally_characters[0];
                }
                else
                {
                    initial_target = enemy_characters[0];
                }
                current_character.GetComponent<Character>()->skill_text.GetComponent<Text>()->text = FLX_STRING_NEW(current_character.GetComponent<Character>()->skill_three.description);
                target_num = 0;
                move_selected = true;
                std::string message = current_character.GetComponent<Character>()->character_name + " will use " + current_character.GetComponent<Character>()->skill_three.name + " targeting " + initial_target.GetComponent<Character>()->character_name;
                Log::Info(message);
            }

            if (Input::GetKeyDown(GLFW_KEY_Q))
            {
                target_num--;
                if (std::get<2>(current_character.GetComponent<Character>()->pending_skill.effect[0]) <= 3)
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
                std::string message = current_character.GetComponent<Character>()->character_name + " will use " + current_character.GetComponent<Character>()->skill_three.name + " targeting " + initial_target.GetComponent<Character>()->character_name;
                Log::Info(message);
            }
            else if (Input::GetKeyDown(GLFW_KEY_E))
            {
                target_num++;
                if (std::get<2>(current_character.GetComponent<Character>()->pending_skill.effect[0]) <= 3)
                {
                    if (target_num >= 0 && target_num < ally_characters.size())
                    {
                        initial_target = ally_characters[target_num];
                    }
                    else
                    {
                        target_num = ally_characters.size() - 1;
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
                        target_num = enemy_characters.size() - 1;
                        initial_target = enemy_characters[target_num];
                    }
                }
                std::string message = current_character.GetComponent<Character>()->character_name + " will use " + current_character.GetComponent<Character>()->skill_three.name + " targeting " + initial_target.GetComponent<Character>()->character_name;
                Log::Info(message);
            }

            if (Input::GetKeyDown(GLFW_KEY_SPACE) && move_selected)
            {
                MoveResolution();
            }
        }

        if (delay_timer > 0.0f)
        {
            delay_timer -= deltaTime;
        }
    }

    void BossSelect()
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
    }

    void StartOfTurn()
    {
        SpeedSort();
        int value = characters[0].GetComponent<Character>()->current_speed;
        for (unsigned i = 0; i < characters.size(); i++) {
                characters[i].GetComponent<Character>()->current_speed -= value;
        }
        current_character = characters[0];
        MoveSelect();
        std::string message = current_character.GetComponent<Character>()->character_name + "'s turn!";
        Log::Info(message);
    }

    void MoveSelect()
    {
        if (current_character.GetComponent<Character>()->is_player)
        {
            current_character.GetComponent<Character>()->ToggleSkill();
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
    void MoveResolution()
    {
        if (current_character.GetComponent<Character>()->is_player)
        {
            MoveExecution();
            current_character.GetComponent<Character>()->ToggleSkill();
            EndOfTurn();
        }
        else
        {
            //random gen, but for this build boss uses 1->2->3->3
            BossSelect();
            EndOfTurn();
        }
    }

    void EndOfTurn()
    {
        for (unsigned i = 0; i < characters.size(); i++) {
            if (characters[i].GetComponent<Character>()->current_hp <= 0)
            {
                characters.erase(characters[i]);
            }
        }

        StartOfTurn();
    }

    void SpeedSort()
    {
        for (unsigned i = 0; i < characters.size(); i++) {
            for (unsigned j = i + 1; j < characters.size(); j++)
            {
                if (characters[i].GetComponent<Character>()->current_speed < characters[j].GetComponent<Character>()->current_speed)
                {
                    std::swap(characters[i], characters[j]);
                }
            }
        }
        for (unsigned i = 0; i < ally_characters.size(); i++) {
            for (unsigned j = i + 1; j < ally_characters.size(); j++)
            {
                if (ally_characters[i].GetComponent<Character>()->current_speed < ally_characters[j].GetComponent<Character>()->current_speed)
                {
                    std::swap(ally_characters[i], ally_characters[j]);
                }
            }
        }
        for (unsigned i = 0; i < enemy_characters.size(); i++) {
            for (unsigned j = i + 1; j < enemy_characters.size(); j++)
            {
                if (enemy_characters[i].GetComponent<Character>()->current_speed < enemy_characters[j].GetComponent<Character>()->current_speed)
                {
                    std::swap(enemy_characters[i], enemy_characters[j]);
                }
            }
        }
        for (unsigned i = 0; i < characters.size(); i++) {
            if (characters[i].GetComponent<Character>()->character_name == "Renko")
            {
                renko_portrait.GetComponent<Position>()->position = barPos[i];
                renko_icon.GetComponent<Position>()->position = barPos[i];
            }
            else if (characters[i].GetComponent<Character>()->character_name == "Grace")
            {
                grace_portrait.GetComponent<Position>()->position = barPos[i];
                grace_icon.GetComponent<Position>()->position = barPos[i];
            }
            else if (characters[i].GetComponent<Character>()->character_name == "Jack")
            {
                jack_portrait.GetComponent<Position>()->position = barPos[i];
                jack_icon.GetComponent<Position>()->position = barPos[i];
            }
        }
    }

    void MoveExecution()
    {
        //random gen, but for this build boss uses 1->2->3->3
        for (unsigned i = 0; i < current_character.GetComponent<Character>()->pending_skill.effect.size(); i++) {
            current_targets.clear();

            switch (std::get<2>(current_character.GetComponent<Character>()->pending_skill.effect[i]))
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
                for (unsigned i = 0; i < ally_characters.size(); i++) {
                    if (current_character != ally_characters[i])
                    {
                        current_targets.push_back(ally_characters[i]);
                    }
                    current_targets.push_back(initial_target);
                }
                break;
            case 3:
                for (unsigned i = 0; i < ally_characters.size(); i++) {
                    current_targets.push_back(initial_target);
                }
                break;
            case 4:
                current_targets.push_back(initial_target);
                break;
            case 5:
                for (unsigned i = 0; i < enemy_characters.size(); i++) {
                    current_targets.push_back(initial_target);
                }
                break;
            case 6:
                for (unsigned i = 0; i < enemy_characters.size(); i++) {
                    current_targets.push_back(initial_target);
                }
                break;
            }

            if (current_targets.empty())
            {
                continue;
            }

            if (std::get<0>(current_character.GetComponent<Character>()->pending_skill.effect[i]) == "DAMAGE")
            {
                int damage = std::get<1>(current_character.GetComponent<Character>()->pending_skill.effect[i]);
                int totaldamage = 0;
                std::string message = current_character.GetComponent<Character>()->character_name + " attacked ";
                if (current_character.GetComponent<Character>()->attack_buff_duration > 0)
                {
                    damage += std::get<1>(current_character.GetComponent<Character>()->pending_skill.effect[i]);
                }
                if (current_character.GetComponent<Character>()->attack_debuff_duration > 0)
                {
                    damage -= std::get<1>(current_character.GetComponent<Character>()->pending_skill.effect[i]);
                }
                for (unsigned i = 0; i < current_targets.size(); i++) {

                    current_targets[i].GetComponent<Character>()->TakeDamage(damage);
                    current_targets[i].GetComponent<Character>()->UpdateEffect();
                    message += current_targets[i].GetComponent<Character>()->character_name + " ";
                    if (i + 1 != current_targets.size())
                    {
                        message += "and ";
                    }
                }
                message += "with " + current_character.GetComponent<Character>()->pending_skill.name + " for a total of " + std::to_string(totaldamage) + " damage!";
                Log::Info(message);
            }
            else if (std::get<0>(current_character.GetComponent<Character>()->pending_skill.effect[i]) == "ATTACK_BUFF")
            {
                int duration = std::get<1>(current_character.GetComponent<Character>()->pending_skill.effect[i]);
                for (unsigned i = 0; i < current_targets.size(); i++) {

                    current_targets[i].GetComponent<Character>()->attack_buff_duration += duration;
                    current_targets[i].GetComponent<Character>()->UpdateEffect();
                }
            }
            else if (std::get<0>(current_character.GetComponent<Character>()->pending_skill.effect[i]) == "ATTACK_DEBUFF")
            {
                int duration = std::get<1>(current_character.GetComponent<Character>()->pending_skill.effect[i]);
                for (unsigned i = 0; i < current_targets.size(); i++) {

                    current_targets[i].GetComponent<Character>()->attack_debuff_duration += duration;
                    current_targets[i].GetComponent<Character>()->UpdateEffect();
                }
            }
            else if (std::get<0>(current_character.GetComponent<Character>()->pending_skill.effect[i]) == "INVULN_BUFF")
            {
                int duration = std::get<1>(current_character.GetComponent<Character>()->pending_skill.effect[i]);
                for (unsigned i = 0; i < current_targets.size(); i++) {

                    current_targets[i].GetComponent<Character>()->invuln_buff_duration += duration;
                    current_targets[i].GetComponent<Character>()->UpdateEffect();
                }
            }
            else if (std::get<0>(current_character.GetComponent<Character>()->pending_skill.effect[i]) == "STUN_DEBUFF")
            {
                int duration = std::get<1>(current_character.GetComponent<Character>()->pending_skill.effect[i]);
                for (unsigned i = 0; i < current_targets.size(); i++) {

                    current_targets[i].GetComponent<Character>()->stun_debuff_duration += duration;
                    current_targets[i].GetComponent<Character>()->UpdateEffect();
                }
            }
            current_character.GetComponent<Character>()->current_speed = current_character.GetComponent<Character>()->base_speed + current_character.GetComponent<Character>()->pending_skill.speed;
        }
    }
};