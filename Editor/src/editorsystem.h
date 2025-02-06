/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// editor.h
//
// Each system (selection, commands) will inherit from this
//
// AUTHORS
// [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
**************************************************************************/

#pragma once
#include <FlexEngine.h>

namespace Editor
{
	class EditorSystem
	{
	public:
		/************************** Virtual functions to implement *************************/
		virtual void Init() = 0;
		virtual void Update() = 0;
		virtual void Shutdown() = 0;
	};
}