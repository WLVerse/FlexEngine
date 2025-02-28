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

using namespace FlexEngine;

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
		void CreatePrefab();
		void LoadPrefab();
		void SavePrefab();
		void DeletePrefab();
		void PrefabDisplay();

		// store prefab key names and values
		// First variable: Original Key Name, Second Variable: New Key Name, Third Variable: Key Value
		std::vector<std::tuple<std::string, std::string, std::variant<std::string, bool, int>, bool>> prefab_keys;
		// prefab file name
		// First variable: Original File Name, Second Variable: New File Name
		std::pair<FileList, std::string> file_name;
	};

}