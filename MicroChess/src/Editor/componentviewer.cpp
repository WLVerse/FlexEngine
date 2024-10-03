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


namespace ChronoShift
{
	std::unordered_map<std::string, ComponentViewer> ComponentViewRegistry::m_component_viewer;

	void ComponentViewRegistry::RegisterComponent(std::string name, ComponentViewer display)
	{
		m_component_viewer[name] = display;
	}

	bool ComponentViewRegistry::ViewerExists(std::string name)
	{
		if (auto search = m_component_viewer.find(name); search == m_component_viewer.end())
			return false;
		else return true;
	}

	void ComponentViewRegistry::ViewComponent(std::string name, FlexECS::Entity entity)
	{
		if (auto search = m_component_viewer.find(name); search == m_component_viewer.end())
			return;

		m_component_viewer[name](entity);
	}
}