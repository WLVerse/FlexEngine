/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// selectionsystem.cpp
//
// Centralized location where selecting/deleting entities is managed in
// the level editor.
//
// AUTHORS
// [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
//   - Main Author
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#include "selectionsystem.h"

using namespace FlexEngine;

void Editor::SelectionSystem::Init()
{
}

void Editor::SelectionSystem::Update()
{
	if (!m_entities_to_delete.empty())
	{
		HandleEntityDelete();
		m_entities_to_delete.clear();
		ClearSelection();
	}
}

void Editor::SelectionSystem::Shutdown()
{
}

const std::unordered_set<FlexEngine::FlexECS::EntityID>& Editor::SelectionSystem::GetSelectedEntities()
{
	return m_selected_entities;
}

void Editor::SelectionSystem::SelectEntity(FlexEngine::FlexECS::EntityID id)
{
	ClearSelection();
	m_selected_entities.insert(id);
}

void Editor::SelectionSystem::AddSelectedEntity(FlexEngine::FlexECS::EntityID id)
{
	if (m_selected_entities.find(id) != m_selected_entities.end())
	{
		DeselectEntity(id);
	}
	else
	{
		m_selected_entities.insert(id);
	}
}

void Editor::SelectionSystem::DeselectEntity(FlexEngine::FlexECS::EntityID id)
{
	m_selected_entities.erase(id);
}

void Editor::SelectionSystem::ClearSelection()
{
	m_selected_entities.clear();
}

void Editor::SelectionSystem::DeleteEntity(FlexEngine::FlexECS::EntityID id)
{
	m_entities_to_delete.insert(id);
}

void Editor::SelectionSystem::DeleteSelectedEntities()
{
	for (auto entity : m_selected_entities)
	{
		m_entities_to_delete.insert(entity);
	}
}

void Editor::SelectionSystem::HandleEntityDelete()
{
	for (auto entity : m_entities_to_delete)
	{
		FindChildrenToDelete(entity);
	}

	auto scene = FlexECS::Scene::GetActiveScene();
	for (auto entity : m_entities_to_delete)
	{
		if (entity == CameraManager::GetMainGameCameraID())
		{
			CameraManager::RemoveMainCamera();
		}
		scene->DestroyEntity(entity);
	}
}

void Editor::SelectionSystem::FindChildrenToDelete(FlexEngine::FlexECS::EntityID id)
{
	FlexECS::Entity parent = id;
	auto scene = FlexECS::Scene::GetActiveScene();
	for (auto& entity : scene->CachedQuery<Parent>())
	{
		auto pid = entity.GetComponent<Parent>()->parent;
		if (pid == parent)
		{
			m_entities_to_delete.insert(entity);
			FindChildrenToDelete(entity);
		}
	}
}
