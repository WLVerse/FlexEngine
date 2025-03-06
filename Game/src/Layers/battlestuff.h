/*#pragma once
#include <vector>
#include <string>
#include <FlexEngine.h>

namespace Game
{
    #pragma region Battle Data

    struct Move
    {
        std::string name = "";
        int speed = 0;

        std::vector<std::string> effect;
        std::vector<int> value;
        std::vector<std::string> target;

        std::string description = "";
    };

    struct Character
    {
        std::string name = "";

        int character_id = 0; // 1-5, determines the animations and sprites which are hardcoded
        int health = 0;
        int speed = 0;

        Move move_one = {};
        Move move_two = {};
        Move move_three = {};

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

    struct Battle
    {
        std::vector<Character*> drifters_and_enemies = {}; 
        std::vector<Character*> speed_bar = {};

        FlexECS::Entity projected_character;

        std::array<Vector3, 7> sprite_slot_positions = {};
        std::array<Vector3, 7> healthbar_slot_positions = {};

        // std::array<Vector3, 7> speedbar_slot_positions; // needed for animated speed bar

        // used during move selection and resolution
        Character* current_character = nullptr;
        Move* current_move = nullptr;
        Character* initial_target = nullptr;
        int move_num = 0;
        int target_num = 0; // 0-4, pointing out which enemy slot is the target

        // game state
        bool is_player_turn = true;
        float disable_input_timer = 0.f;
        bool prev_state = is_player_turn; // used to cache the previous state, but also can be set to false even when the player takes a turn and still their turn next

        bool is_paused = false;
        bool is_end = false;

        bool start_of_turn = false;
        bool move_select = false;
        bool move_resolution = false;
        bool end_of_turn = false;

        bool change_phase = false;
        // Return to original position
        Character* previous_character = nullptr;
    };
}*/