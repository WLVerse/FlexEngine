/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// editor.h
//
// Each panel (scene, hierarchy, inspector, etc) will inherit from this
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
	class EditorPanel
	{
	public:
		/************************** Virtual functions to implement *************************/
		virtual void Init() = 0;
		virtual void Update() = 0;
		virtual void EditorUI() = 0;
		virtual void Shutdown() = 0;
	};
}