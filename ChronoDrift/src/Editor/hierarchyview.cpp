/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// hierarchyview.cpp
//
// Hierarchy Panel for the editor.
//
// AUTHORS
// [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
//   - Main Author
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#include "editor.h"
#include "hierarchyview.h"
#include "Components/rendering.h"
#include "windowsizes.h"


using namespace FlexEngine;
using EntityName = FlexEngine::FlexECS::Scene::StringIndex;

namespace ChronoDrift
{
	void HierarchyView::Init()
	{
	}
	void HierarchyView::Update()
	{
	}

	void HierarchyView::EditorUI()
	{
		auto scene = FlexECS::Scene::GetActiveScene();



		FunctionQueue delete_queue;

		FlexECS::Entity entity_to_delete = FlexECS::Entity::Null;

		SETHIERARCHYWINDOW
		ImGui::Begin("Scene Hierarchy");
		//size_t entity_count = scene->entity_index.size();
		//std::string entity_count_text = "Entity Count:  " + std::to_string(entity_count);
		//ImGui::Text(entity_count_text.c_str());
		
		//Drag a sprite from assets to window to create entity with the sprite.
		if (auto image = EditorGUI::StartWindowPayloadReceiver<const char>(PayloadTags::IMAGE))
		{
			//calculate position to place, at center
			CameraManager& cam_manager = Editor::GetInstance().GetCamManager();
			Vector3 position = cam_manager.GetCameraData(cam_manager.GetEditorCamera())->position;
			float width = static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetWidth());
			float height = static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetHeight());
			position.x += width / 2;
			position.y += height / 2;

			std::string image_key(image);
			std::filesystem::path path = image_key;

			FlexECS::Entity new_entity = scene->CreateEntity(path.filename().string());
			new_entity.AddComponent<IsActive>({true});
			new_entity.AddComponent<Position>({ {position.x, position.y} });
			new_entity.AddComponent<Rotation>({});
			new_entity.AddComponent<Scale>({ Vector2::One * 100.0f });
			new_entity.AddComponent<Transform>({});
			new_entity.AddComponent<ZIndex>({});
			new_entity.AddComponent<Sprite>({ FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(image_key) });
			new_entity.AddComponent<Shader>({ FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(R"(\shaders\texture)") });
			EditorGUI::EndPayloadReceiver();
		}


		//Right click menu (create entity)
		if (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup("RightClickMenu");
		}
		if (ImGui::BeginPopup("RightClickMenu"))
		{
			if (ImGui::MenuItem("Create Entity"))
			{
				//Add default components
				FlexECS::Entity new_entity = FlexECS::Scene::CreateEntity();
				new_entity.AddComponent<IsActive>({});
				new_entity.AddComponent<Position>({});
				new_entity.AddComponent<Rotation>({});
				new_entity.AddComponent<Scale>({});
				new_entity.AddComponent<Transform>({});
				new_entity.AddComponent<ZIndex>({});
			}
			ImGui::EndPopup();
		}

		int imgui_id = 0;
		for (auto& [id, record] : scene->entity_index)
		{
			ImGui::PushID(imgui_id++);
			FlexECS::Entity entity(id);

			std::string name = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>());
			ImGuiTreeNodeFlags node_flags =
				ImGuiTreeNodeFlags_DefaultOpen |
				ImGuiTreeNodeFlags_FramePadding |
				ImGuiTreeNodeFlags_OpenOnArrow |
				ImGuiTreeNodeFlags_SpanAvailWidth;

			bool is_selected = (entity == Editor::GetInstance().GetSelectedEntity());
			if (is_selected)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}

			bool is_open = ImGui::TreeNodeEx(name.c_str(), node_flags, name.c_str());
			if (is_open)
			{
				if (ImGui::BeginDragDropSource())
				{
					EditorGUI::StartPayload(PayloadTags::ENTITY, &entity.Get(), sizeof(FlexECS::EntityID), name.c_str());
					EditorGUI::EndPayload();
				}
				ImGui::TreePop();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left) && !ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				Editor::GetInstance().SelectEntity(entity);
			}


			if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
			{
				ImGui::OpenPopup("EntityOptions");
				Editor::GetInstance().SelectEntity(entity);
			}
			if (ImGui::BeginPopup("EntityOptions"))
			{
				if (ImGui::MenuItem("Duplicate Entity"))
				{
					FlexECS::EntityID new_EntityID = scene->CloneEntity(entity);
					if (entity.HasComponent<Camera>())
						Editor::GetInstance().GetCamManager().AddCameraEntity(new_EntityID, entity.GetComponent<Camera>()->camera);
				}
				if (ImGui::MenuItem("Destroy Entity"))
				{
					delete_queue.Insert({ [scene, entity]() {scene->DestroyEntity(entity); }, "", 0 });
					if (entity.HasComponent<Camera>())
						Editor::GetInstance().GetCamManager().RemoveCameraEntity(entity.Get());
					Editor::GetInstance().SelectEntity(FlexECS::Entity::Null);
				}
				ImGui::EndPopup();
			}

			ImGui::PopID();
		}


		//Track Clicks when not inside tree node
		//Deselect focused entity
		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0))
		{
			Editor::GetInstance().SelectEntity(FlexECS::Entity::Null);  // Deselect when clicking in empty space
		}




		ImGui::End();

		delete_queue.Flush();
	}

	void HierarchyView::Shutdown()
	{
	}
}

