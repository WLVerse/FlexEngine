/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// gameview.h
//
// Game Panel for the editor.
//
// AUTHORS
// [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
**************************************************************************/

#pragma once
#include "editorpanel.h"

namespace Editor
{

	class GameView : public EditorPanel
	{
	public:
		void Init();
		void Update();
		void EditorUI();
		void Shutdown();
	
	private:
		void CalculateViewportPosition();	//internal function to determine position to place viewport
		void SetMousePosition();


		ImVec2 m_viewport_size;
		ImVec2 m_viewport_position;
		ImVec2 m_viewport_screen_position;

		FlexEngine::Vector2 m_mouse_pos;
	};
}
