/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// hierarchyview.h
//
// Hierarchy Panel for the editor.
//
// AUTHORS
// [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
//   - Main Author
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#pragma once
#include "editorpanel.h"

namespace Editor
{

	/*!***************************************************************************
	* @brief
	* Call in Update() function of EditorLayer().
	******************************************************************************/
	class HierarchyView : public EditorPanel
	{
	public:
		void Init();
		void Update();
		void EditorUI();
		void Shutdown();
	};


}

