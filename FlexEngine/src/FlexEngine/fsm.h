// fsm.h
//
// Implements a finite state machine template. It consists of a few main components
// - Stateful behavior
// - Supports state start and exit
// - Supports custom state flow functions
// - State addition and removal
// 
// Usage:
// - Create a new FiniteStateMachine object
// - Call AddState to add a new state
// - Call AddTransition to add a new transition to a state, specified by a condition, start and end
// 
// Example:
// 
// To setup...
// FiniteStateMachine fsm;
// 
// fsm.AddState("StateOne", [] { Log::Info("Exit"); }, [] { Log::Info("Update"); }, [] { Log::Info("Enter"); });
// fsm.AddState("StateTwo", [] { Log::Info("Exit2"); }, [] { Log::Info("Update2"); }, [] { Log::Info("Enter2"); });
// fsm.AddTransition("StateOne", { [] { return Input::GetKeyDown(GLFW_KEY_K); }, "StateTwo" });
// fsm.AddTransition("StateTwo", { [] { return Input::GetKeyDown(GLFW_KEY_L); }, "StateOne" });
// 
// To run FSM, make sure this is constantly called...
// fsm.Update();
// 
//
// AUTHORS
// [100%] Yew Chong (yewchong.k\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2025 DigiPen, All rights reserved.

#include <functional>
#include <string>
#include <unordered_map>
#include "flx_api.h"

class __FLX_API FiniteStateMachine
{
  using exitConditions = std::pair<std::function<bool()>, std::string>; // A bool function and a target state

  struct __FLX_API State
  {
    // OnEnter State
    std::function<void()> on_enter;
    
    // OnUpdate State
    std::function<void()> on_update;

    // OnExit State
    std::function<void()> on_exit;

    // State Name
    std::string state_name;

    // Transition 
    std::vector<exitConditions> transitions;

    // A name must minimally be defined to avoid overlapping states and hard to track bugs
    State(std::string name,
          std::function<void()> exit_function = []{},
          std::function<void()> update_function = []{}, // Default to no op
          std::function<void()> enter_function = []{}) : state_name(name), on_exit(exit_function), on_update(update_function), on_enter(enter_function)
    {

    }
  };

  std::unordered_map<std::string, State> fsm_states;
  std::string current_state;

public:
  void AddState(std::string const state_name,
                std::function<void()> exit_function = [] {},
                std::function<void()> update_function = [] {},
                std::function<void()> enter_function = [] {});

  // Attaches an exit condition to a state.
  void AddTransition(std::string const src_state, FiniteStateMachine::exitConditions exitCondition);

  void RemoveState(std::string state_name);

  void Start();
  void Update();
  void Exit();
  void ChangeState(std::string state_name);
};