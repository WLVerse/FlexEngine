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
#include "editorgui.h"

using namespace FlexEngine;
namespace ChronoShift
{
	int EditorUI::m_id = 0;

	void EditorUI::DragFloat2(Vector2& data, std::string title,
		//std::string label1, std::string label2, 
		float width, float drag_speed)
	{
		if (title != "")
		{
			TextField(title);
		}
		PushID();
		ImGui::PushItemWidth(width);
		ImGui::Text("X"); ImGui::SameLine();
		ImGui::DragFloat("", &data.x, drag_speed);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		PopID();
		PushID();
		ImGui::Text("Y"); ImGui::SameLine();
		ImGui::PushItemWidth(width);
		ImGui::DragFloat("", &data.y, drag_speed);
		ImGui::PopItemWidth();
		PopID();
	}

	void EditorUI::DragFloat3(Vector3& data, std::string title,
		//std::string label1, std::string label2, std::string label3,
		float width, float drag_speed)
	{
		if (title != "")
		{
			TextField(title);
		}
		PushID();
		ImGui::PushItemWidth(width);
		ImGui::Text("X"); ImGui::SameLine();
		ImGui::DragFloat("", &data.x, drag_speed);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		PopID();
		PushID();
		ImGui::Text("Y"); ImGui::SameLine();
		ImGui::PushItemWidth(width);
		ImGui::DragFloat("", &data.y, drag_speed);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		PopID();
		PushID();
		ImGui::Text("Z"); ImGui::SameLine();
		ImGui::PushItemWidth(width);
		ImGui::DragFloat("", &data.z, drag_speed);
		ImGui::PopItemWidth();
		PopID();
	}

	void EditorUI::DragInt(int& data, std::string title, float width, float drag_speed)
	{
		PushID();
		ImGui::PushItemWidth(width);
		ImGui::Text(title.c_str()); ImGui::SameLine();
		ImGui::DragInt("", &data, drag_speed);
		ImGui::PopItemWidth();
		PopID();
	}

	void EditorUI::EntityReference(FlexECS::Entity& entity, std::string title)
	{
		PushID();
		std::string entity_name = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<FlexEngine::FlexECS::Scene::StringIndex>());

		ImGui::BeginGroup();
		ImGui::Text(title.c_str()); ImGui::SameLine();
		if (ImGui::Button(entity_name.c_str()))
		{
			Editor::GetInstance()->SelectEntity(entity);
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_DRAG"))
			{
				// Assuming the payload is the Entity ID (you can customize this)
				FlexECS::Entity dropped_entity = *(FlexECS::EntityID*)payload->Data;
				entity = dropped_entity;
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::EndGroup();
		PopID();
	}

	void EditorUI::Color3(Vector3& data, std::string title)
	{
		PushID();
		ImGui::ColorEdit3(title.c_str(), data.data);
		PopID();
	}

	void EditorUI::EditableTextField(std::string& data, std::string title)
	{
		PushID();
		char text_buffer[128];
		strncpy_s(text_buffer, data.c_str(), sizeof(text_buffer));
		text_buffer[sizeof(text_buffer) - 1] = '\0';
		if (ImGui::InputText(title.c_str(), text_buffer, IM_ARRAYSIZE(text_buffer)))
		{
			data = std::string(text_buffer);
		}
		PopID();
	}

	void EditorUI::TextField(const std::string& data)
	{
		PushID();
		ImGui::Text(data.c_str());
		PopID();
	}


	void EditorUI::StartFrame()
	{
		m_id = 0;
	}

	void EditorUI::EndFrame()
	{
		m_id = 0;
	}

	int EditorUI::PushID()
	{
		ImGui::PushID(m_id);
		return m_id++;
	}

	void EditorUI::PopID()
	{
		ImGui::PopID();
	}


}
