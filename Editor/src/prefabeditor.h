/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// prefabeditor.h
//
// Brief
//
// AUTHORS
// [100%] Ho Jin Jie Donovan (h.jinjiedonovan\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#pragma once
#include "FlexEngine.h"
#include "editorpanel.h"


namespace Editor
{

	class PrefabEditor : public EditorPanel
	{
	public:
		void Init();
		void Update();
		void EditorUI();
		void Shutdown();

	private:
		std::vector<std::pair<std::string, std::variant<std::string, bool, int>>> prefab_keys;
		std::string file_name;
	};

}