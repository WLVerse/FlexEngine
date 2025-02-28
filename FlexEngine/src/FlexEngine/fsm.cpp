// fsm.cpp
//
// Implements a finite state machine template.
//
// AUTHORS
// [100%] Yew Chong (yewchong.k\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2025 DigiPen, All rights reserved.

#include "fsm.h"
#include "flexlogger.h"

void FiniteStateMachine::AddState(std::string const state_name,
                                  std::function<void()> exit_function,
                                  std::function<void()> update_function,
                                  std::function<void()> enter_function)
{
  if (fsm_states.find(state_name) != fsm_states.end())
  {
    // State already exists
    FlexEngine::Log::Warning("State already exists with name: " + state_name + ", aborting AddState!");
    return;
  }
  
  // Since we have no default constructor, we must emplace to avoid creating a temporary object
  fsm_states.emplace(state_name, State(state_name, exit_function, update_function, enter_function));
  
  if (current_state == "") current_state = state_name; // Default sets a current_state
}

void FiniteStateMachine::RemoveState(std::string state_name)
{
  if (fsm_states.find(state_name) == fsm_states.end())
  {
    // State does not exist
    FlexEngine::Log::Warning("State does not exist with name: " + state_name + ". Removal cancelled.");
    return;
  }
  
  fsm_states.erase(state_name);
}

void FiniteStateMachine::AddTransition(std::string const src_state, FiniteStateMachine::exitConditions exitCondition)
{
  fsm_states.at(src_state).transitions.push_back(exitCondition);
}

void FiniteStateMachine::Start()
{
  fsm_states.at(current_state).on_enter();
}

void FiniteStateMachine::Update()
{
  if (current_state == "") return; // Guard empty finite state machines being called.

  fsm_states.at(current_state).on_update();
  
  // Check for any exit conditions in the current state
  for (auto& transition : fsm_states.at(current_state).transitions)
  {
    if (transition.first())
    {
      ChangeState(transition.second);
    }
  }
}

void FiniteStateMachine::Exit()
{
  fsm_states.at(current_state).on_exit();
}

void FiniteStateMachine::ChangeState(std::string target_state)
{
  if (fsm_states.find(target_state) == fsm_states.end())
  {
    FlexEngine::Log::Warning("State does not exist with name: " + target_state);
    return;
  }
  
  fsm_states.at(current_state).on_exit();
  current_state = target_state;
  fsm_states.at(current_state).on_enter();
}
