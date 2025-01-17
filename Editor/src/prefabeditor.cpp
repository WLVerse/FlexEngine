/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// prefabeditor.cpp
//
// Brief
//
// AUTHORS
// [100%] Ho Jin Jie Donovan (h.jinjiedonovan\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#include "editor.h"
#include "prefabeditor.h"

using namespace FlexEngine;
using EntityName = FlexEngine::FlexECS::Scene::StringIndex;

namespace Editor
{
	void PrefabEditor::Init()
	{}
	void PrefabEditor::Update()
	{}
	void PrefabEditor::Shutdown()
	{}

	void PrefabEditor::EditorUI()
	{
		ImGui::Begin("Prefab Editor");


		ImGui::End();
	}
}