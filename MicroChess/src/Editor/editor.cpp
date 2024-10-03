/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// editor.cpp
//
// This file contains the implementation of the Editor class.
//
// AUTHORS
// [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
//   - Main Author
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#pragma once
#include "editor.h"


namespace ChronoShift
{
	Editor* Editor::GetInstance()
	{
		static bool initialized = false;
		static Editor instance{};
		if (!initialized) instance.Init();

		return &instance;
	}

	void Editor::Init()
	{

	}

	//ImGui startframe endframe already called in States::Window
	void Editor::Update()
	{
		EditorUI::StartFrame();
		
		DisplaySceneHierarchy();
		DisplayInspector();
		
		EditorUI::EndFrame();
	}
	void Editor::Shutdown()
	{
	}

	void Editor::SelectEntity(FlexECS::Entity entity)
	{
		m_selected_entity = entity;
	}

	FlexECS::Entity Editor::GetSelectedEntity()
	{
		return m_selected_entity;
	}

}

