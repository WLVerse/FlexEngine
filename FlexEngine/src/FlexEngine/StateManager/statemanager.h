// WLVERSE [https://wlverse.web.app]
// statemanager.h
//
// Uses macros to easily create StateManager classes. 
// These macros can be used in other files as well. 
// 
// Usage: 
// 
// FLX_STATEMANAGER_REGISTER_DECL(Application); 
// This will create a class called ApplicationStateManager. 
// Remember to run the FLX_STATEMANAGER_REGISTER_IMPL macro in a cpp file. 
// 
// FLX_STATEMANAGER_REGISTER_IMPL(Application); 
// This will create the implementation of the ApplicationStateManager class. 
// This implementation keeps everything in scope, do not rely on restart 
// to fully reset the state. That is the responsibility of the state itself. 
// 
// The state manager implementation itself uses the Template Method Pattern. 
// It is a static class that doesn't need explicit instantiation. 
// Create states using the State class and set them using the SetState method. 
// 
// Usage: ApplicationStateManager::SetState<YourState>(); 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include "istate.h"

#include <memory>

// Uses macros to easily create StateManager classes.
// These macros can be used in other files as well.
#pragma region Macros

#pragma region Declaration
// Creates a StateManager class with the given name.
// Usage: FLX_STATEMANAGER_REGISTER_DECL(Application);
// This will create a class called ApplicationStateManager.
// Remember to run the FLX_STATEMANAGER_REGISTER_IMPL macro in a cpp file.
#define FLX_STATEMANAGER_REGISTER_DECL(NAME)                                     \
  class __FLX_API NAME##StateManager                                             \
  {                                                                              \
    static std::unique_ptr<State> m_state_current;                               \
    static std::unique_ptr<State> m_state_next;                                  \
                                                                                 \
  public:                                                                        \
    /* Usage example : NAME##StateManager::SetState<GameState>(); */             \
    template <typename T>                                                        \
    static void SetState()                                                       \
    {                                                                            \
      SetState(std::make_unique<T>());                                           \
    }                                                                            \
                                                                                 \
    /* For best practice, use the SetState template method instead               \
     * Usage: NAME##StateManager::SetState(std::make_unique<GameState>());       \
     */                                                                          \
    static void SetState(std::unique_ptr<State> state);                          \
                                                                                 \
    static void Restart();                                                       \
    static void UpdateManager();                                                 \
    static void Update();                                                        \
    static void Exit();                                                          \
  }
#pragma endregion

#pragma region Implementation
// Implementation of the StateManager class.
// Usage: FLX_STATEMANAGER_REGISTER_IMPL(Application);
// This will create the implementation of the ApplicationStateManager class.
// This implementation keeps everything in scope, do not rely on restart to fully reset the state.
// That is the responsibility of the state itself.
#define FLX_STATEMANAGER_REGISTER_IMPL(NAME)                            \
  void NAME##StateManager::SetState(std::unique_ptr<State> state)       \
  {                                                                     \
    /* no state */                                                      \
    if (!state) return;                                                 \
                                                                        \
    /* same state */                                                    \
    if (state == m_state_current || state == m_state_next) return;      \
                                                                        \
    /* set the next state */                                            \
    m_state_next.swap(state);                                           \
  }                                                                     \
                                                                        \
  void NAME##StateManager::Update()                                     \
  {                                                                     \
    /* no state */                                                      \
    if (!m_state_current) return;                                       \
                                                                        \
    m_state_current->Update();                                          \
  }                                                                     \
                                                                        \
  void NAME##StateManager::UpdateManager()                              \
  {                                                                     \
    /* no state */                                                      \
    if (!m_state_next) return;                                          \
                                                                        \
    /* same state */                                                    \
    if (m_state_next == m_state_current) return;                        \
                                                                        \
    /* set the next state */                                            \
    Exit();                                                             \
    m_state_current.swap(m_state_next);                                 \
    m_state_current->OnEnter();                                         \
  }                                                                     \
                                                                        \
  void NAME##StateManager::Restart()                                    \
  {                                                                     \
    /* no state */                                                      \
    if (!m_state_current) return;                                       \
                                                                        \
    /* EXTREMELY IMPORTANT NOTE!                                        \
     * this implementation keeps everything in scope                    \
     * do not rely on restart to fully reset the state                  \
     * that is the responsibility of the state itself                   \
     */                                                                 \
    m_state_current->OnExit();                                          \
    m_state_current->OnEnter();                                         \
  }                                                                     \
                                                                        \
  void NAME##StateManager::Exit()                                       \
  {                                                                     \
    if (m_state_current) m_state_current->OnExit();                     \
    m_state_current.reset();                                            \
  }                                                                     \
  /* static member initialization */                                    \
  std::unique_ptr<State> NAME##StateManager::m_state_current = nullptr; \
  std::unique_ptr<State> NAME##StateManager::m_state_next = nullptr

#pragma endregion

#pragma endregion

namespace FlexEngine
{

  //FLX_STATEMANAGER_REGISTER_DECL(Application);

}