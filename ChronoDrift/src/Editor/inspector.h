/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// inspector.h
//
// Inspector Panel (properties viewer) for the editor.
//
// AUTHORS
// [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
//   - Main Author
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#pragma once
#include "FlexEngine.h"
#include "editorpanel.h"


namespace ChronoDrift
{

	class Inspector : public EditorPanel
	{
	public:
		void Init();
		void Update();
		void EditorUI();
		void Shutdown();

	private:
		char m_search_query[64] = "";
		bool m_focus_search_bar{ false };
	};

}