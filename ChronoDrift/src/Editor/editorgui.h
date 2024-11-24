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
#include "imguipayloads.h"

#include "imgui_internal.h"
namespace ChronoDrift
{

	/* =======================================================================================================================
	Brief:
	As of now, most of the functions are only intended to be used within the inspector panel.
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

	class EditorGUI
	{
	public:
		static void DragFloat1(float& data, std::string label1 = "",
			float width = 65.0f, float drag_speed = 0.1f);

		static void DragFloat2(FlexEngine::Vector2& data, std::string title = "", 
			//std::string label1 = "X", std::string label2 = "Y",
			float width = 65.0f, float drag_speed = 0.1f);

		static void DragFloat3(FlexEngine::Vector3& data, std::string title = "", 
			//std::string label1 = "X", std::string label2 = "Y", std::string label3 = "Z",
			float width = 65.0f, float drag_speed = 0.1f);

		static void DragInt(int& data, std::string label1 = "",
			float width = 65.0f, float drag_speed = 0.1f);

		static void DragGLuint(GLuint& data, std::string label1 = "",
			float width = 65.0f, float drag_speed = 0.1f);

		static void CreateCheckbox(bool& value, std::string title = "");

		/*!***************************************************************************
		* @brief
		* Displays reference to an entity in the scene.
		* Supports drag and drop of an entity from the hierarchy panel to the field.
		******************************************************************************/
		static void EntityReference(FlexEngine::FlexECS::Entity& data, std::string title = "Entity");
	
		static void ShaderPath(std::string& path, std::string title = "Shader");

		static void TexturePath(std::string& path, std::string title = "Sprite");

    static void AudioPath(std::string& path, std::string title = "Audio");

		static void Color3(FlexEngine::Vector3& data, std::string title = "color");

		static void EditableTextField(std::string& data, std::string title = "");

		static void TextField(const std::string& data);

		static void Checkbox(bool& data, std::string title = "");

		static void Mat44(FlexEngine::Matrix4x4& data, std::string title = "");



		/*!***************************************************************************
		* @brief payloads
		******************************************************************************/
		static bool StartPayload(PayloadTags tag, const void* data, size_t data_size, std::string tooltip);
		static void EndPayload();

		template <typename T>
		static const T* StartPayloadReceiver(PayloadTags tag);

		template <typename T>
		static const T* StartWindowPayloadReceiver(PayloadTags tag);

		static void EndPayloadReceiver();

		/*!***************************************************************************
		* @brief Gizmos
		******************************************************************************/
		static void GizmoTranslateRight(float* p_x_axis_change, const ImVec2& origin, bool* hovering);
		static void GizmoTranslateUp(float* p_y_axis_change, const ImVec2& origin, bool* hovering);
		static void GizmoTranslateXY(float* p_x_axis_change, float* p_y_axis_change, const ImVec2& origin, bool* hovering);

		//static void Gizmo_Right_Arrow(float* p_x_axis_change, const ImVec2& origin, bool* hovering);


		/*!***************************************************************************
		* @brief
		* Similar to ImGui::StartFrame() and ImGui::EndFrame(), 
		* just call these two once at start and end of frame.
		******************************************************************************/
		static void StartFrame();
		static void EndFrame();

		static int PushID();
		static void PopID();
	private:

		static int m_id;
	};


	/*
	Template functions
	*/
	template<typename T>
	const T* EditorGUI::StartPayloadReceiver(PayloadTags tag)
	{
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GetPayloadTagString(tag));

			if (payload) return static_cast<T*>(payload->Data);
			else return nullptr;
		}
		return nullptr;

	}

	//I dont know how imgui works very well yet
	//https://github.com/ocornut/imgui/issues/5539
	//copy paste for now
	template <typename T>
	const T* EditorGUI::StartWindowPayloadReceiver(PayloadTags tag)
	{
		using namespace ImGui;
		ImRect inner_rect = GetCurrentWindow()->InnerRect;
		if (BeginDragDropTargetCustom(inner_rect, GetID("##WindowBgArea")))
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GetPayloadTagString(tag), ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
			{
				if (payload->IsPreview())
				{
					ImDrawList* draw_list = ImGui::GetForegroundDrawList();
					draw_list->AddRectFilled(inner_rect.Min, inner_rect.Max, GetColorU32(ImGuiCol_DragDropTarget, 0.05f));
					draw_list->AddRect(inner_rect.Min, inner_rect.Max, GetColorU32(ImGuiCol_DragDropTarget), 0.0f, 0, 2.0f);
				}
				if (payload->IsDelivery())
				{
					return static_cast<T*>(payload->Data);
				}
			}
			return nullptr;
		}
		return nullptr;
	}
}