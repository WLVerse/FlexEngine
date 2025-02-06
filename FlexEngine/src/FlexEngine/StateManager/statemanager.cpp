// WLVERSE [https://wlverse.web.app]
// statemanager.cpp
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

#include "statemanager.h"

namespace FlexEngine
{
    
  //FLX_STATEMANAGER_REGISTER_IMPL(Application);

}