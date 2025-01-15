/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// componentviewer.cpp
//
// This file contains a class that helps to display 
// component data inside the Inspector Panel in the editor.
//
// AUTHORS
// [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
//   - Main Author
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/


#include "componentviewer.h"
#include "editor.h"


namespace Editor
{
	// COMPONENT_VIEWER_START(PrefabView)
	// COMPONENT_VIEWER_END(PrefabView)

	std::unordered_map<std::string, ComponentViewer> ComponentViewRegistry::m_component_viewer;
	std::unordered_map<std::string, ComponentViewer> ComponentViewRegistry::m_component_adder;
	std::unordered_map<std::string, ComponentViewer> ComponentViewRegistry::m_component_remover;

	void ComponentViewRegistry::RegisterComponent(std::string name, ComponentViewer display, ComponentViewer adder, ComponentViewer remover)
	{
		if(display != nullptr) m_component_viewer[name] = display;
		if(adder != nullptr) m_component_adder[name] = adder;
		if (remover != nullptr) m_component_remover[name] = remover;
	}

	bool ComponentViewRegistry::ViewerExists(std::string name)
	{
		if (auto search = m_component_viewer.find(name); search == m_component_viewer.end())
			return false;
		else return true;
	}

	bool ComponentViewRegistry::AdderExists(std::string name)
	{
		if (auto search = m_component_adder.find(name); search == m_component_adder.end())
			return false;
		else return true;
	}

	bool ComponentViewRegistry::RemoverExists(std::string name)
	{
		if (auto search = m_component_remover.find(name); search == m_component_remover.end())
			return false;
		else return true;
	}

	void ComponentViewRegistry::ViewComponent(std::string name, FlexECS::Entity entity)
	{
		if (auto search = m_component_viewer.find(name); search == m_component_viewer.end())
			return;

		m_component_viewer[name](entity);
	}


	void ComponentViewRegistry::AddComponent(std::string name, FlexEngine::FlexECS::Entity entity)
	{
		if (auto search = m_component_adder.find(name); search == m_component_adder.end())
			return;

		m_component_adder[name](entity);
	}

	void ComponentViewRegistry::RemoveComponent(std::string name, FlexEngine::FlexECS::Entity entity)
	{
		if (auto search = m_component_remover.find(name); search == m_component_remover.end())
			return;

		m_component_remover[name](entity);
	}


	std::vector<std::string> ComponentViewRegistry::GetComponentList()
	{
		std::vector<std::string> components;
		for (const auto& component : m_component_adder)
		{
			components.push_back(component.first);
		}
		return components;
	}
}