#include "selectionsystem.h"

void Editor::SelectionSystem::Init()
{
}

void Editor::SelectionSystem::Update()
{
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

void Editor::SelectionSystem::DeleteSelectedEntities()
{
}
