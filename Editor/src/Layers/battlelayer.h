// WLVERSE [https://wlverse.web.app]
// audiolayer.h
// 
// Audio layer for the editor.
//
// AUTHORS
// [100%] Yew Chong (yewchong.k\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Editor
{
    class BattleLayer : public FlexEngine::Layer
    {
    public:
        BattleLayer() : Layer("Battle Layer") {}
        ~BattleLayer() = default;

        void BossSelect();
        void StartOfTurn();
        void MoveSelect();
        void MoveResolution();
        void EndOfTurn();
        void SpeedSort();
        void MoveExecution();
        void Find(FlexECS::Entity& obj, std::string obj_name);
        void GetMove(std::string move_name, Character::Move &move_num);
        int TakeDamage(Character& cha, int incoming_damage);
        void ToggleSkill(Character& cha);
        void ToggleSkillText(Character& cha);
        void UpdateEffect(Character& cha);
        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void Update() override;
    };

}
