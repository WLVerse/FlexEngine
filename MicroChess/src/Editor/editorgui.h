/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// editorgui.cpp
//
// This is the Encapsulation of ImGUI functions to be of ease for other programmers to use.
// Lemme know if you need anything, and I'll try to create something easy to use!
//
//
// AUTHORS
// [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
//   - Main Author
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#pragma once
#include "FlexEngine.h"
#include "editor.h"

namespace ChronoShift
{

	/* =======================================================================================================================
	Brief:
	As of now, all the functions are only intended to be used within the inspector panel.
	Will work on making a more general wrapper.

	Regarding function parameters:
	- The first parameter is always the data to be displayed.
	if you see a const, then it will not be editable. Otherwise,
	you have to assume that the value can be changed from the inspector.

	- If you see std::string title, 
	it refers to the title/header to give the component member.
	e.g:
	DragFloat2(data, "Position") will display
	Position
	X[1.11] Y[2.22]
	======================================================================================================================= */

	class EditorUI
	{
	public:
		static void DragFloat2(FlexEngine::Vector2& data, std::string title = "", 
			//std::string label1 = "X", std::string label2 = "Y",
			float width = 65.0f, float drag_speed = 0.1f);

		static void DragFloat3(FlexEngine::Vector3& data, std::string title = "", 
			//std::string label1 = "X", std::string label2 = "Y", std::string label3 = "Z",
			float width = 65.0f, float drag_speed = 0.1f);

		static void DragInt(int& data, std::string label1 = "",
			float width = 65.0f, float drag_speed = 0.1f);

		/*!***************************************************************************
		* @brief
		* Displays reference to an entity in the scene.
		* Supports drag and drop of an entity from the hierarchy panel to the field.
		******************************************************************************/
		static void EntityReference(FlexEngine::FlexECS::Entity& data, std::string title = "Entity");
	
		static void Color3(FlexEngine::Vector3& data, std::string title = "color");

		static void EditableTextField(std::string& data, std::string title = "");

		static void TextField(const std::string& data);



		/*!***************************************************************************
		* @brief
		* Similar to ImGui::StartFrame() and ImGui::EndFrame(), 
		* just call these two once at start and end of frame.
		******************************************************************************/
		static void StartFrame();
		static void EndFrame();

	private:
		static int PushID();
		static void PopID();

		static int m_id;
	};
}