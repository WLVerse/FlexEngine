/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// componentviewer.h
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

#pragma once
#include "FlexEngine.h"
#include "editorgui.h"

namespace ChronoShift
{
	//Each component should provide it's own display functionality for the editor,
	//via a function pointer. Check the macros at the bottom to make it easier.
	using ComponentViewer = std::function<void(FlexEngine::FlexECS::Entity)>;

	class ComponentViewRegistry
	{
	public:
		
		/*!***************************************************************************
		* @brief
		* Adds the function to the m_component_viewer map.
		******************************************************************************/
		static void RegisterComponent(std::string, ComponentViewer);

		/*!***************************************************************************
		* @brief
		* Checks if a component has a viewer defined for it.
		* @return
		* true if has, false otherwise
		******************************************************************************/
		static bool ViewerExists(std::string);

		/*!***************************************************************************
		* @brief
		* Calls the viewer function for the component. This displays the details
		* of the component to the Inspector Panel in the editor.
		******************************************************************************/
		static void ViewComponent(std::string, FlexEngine::FlexECS::Entity);

	private:
		static std::unordered_map<std::string, ComponentViewer> m_component_viewer;

	};


	/*!************************************************************************
	1) Use COMPONENT_VIEWER_START(ComponentName) to automatically create the function
	2) Use COMPONENT_VIEWER_xxxxx(member) with the appropriate type for every member inside the 
	component you want to display
	3) Use COMPONENT_VIEWER_END(ComponentName) to close the function (i know the define parameter is unused, for now D:)
	4) Register the function using REGISTER_COMPONENT_VIEWER(ComponentName)


	//Sample Usage in cpp file:
	
	COMPONENT_VIEWER_START(Position)
		COMPONENT_VIEWER_DRAG_FLOAT2(position)
	COMPONENT_VIEWER_END(Position)
	
	COMPONENT_VIEWER_START(Sprite)
		COMPONENT_VIEWER_COLOR3(color)
		COMPONENT_VIEWER_COLOR3(color_to_add)
		COMPONENT_VIEWER_COLOR3(color_to_multiply)
	COMPONENT_VIEWER_END(Sprite)

	void RegisterRenderingComponents()	//call this when you attach the layer.
	{
		REGISTER_COMPONENT_VIEWER(Position);
		REGISTER_COMPONENT_VIEWER(Sprite);
	}
	**************************************************************************/

	#define COMPONENT_VIEWER_START(TYPE) \
void COMPONENT_VIEWER_##TYPE(FlexEngine::FlexECS::Entity entity) \
{ \
  using T = TYPE;
	
	#define COMPONENT_VIEWER_DRAG_VECTOR2(name) \
  EditorUI::DragFloat2(entity.GetComponent<T>()->name, #name);

	#define COMPONENT_VIEWER_DRAG_VECTOR3(name) \
  EditorUI::DragFloat3(entity.GetComponent<T>()->name, #name); 

	#define COMPONENT_VIEWER_DRAG_INT(name) \
  EditorUI::DragInt(entity.GetComponent<T>()->name, #name); 

	#define COMPONENT_VIEWER_ENTITY_REFERENCE(name) \
  EditorUI::EntityReference(entity.GetComponent<T>()->name, #name); 

	#define COMPONENT_VIEWER_COLOR3(name) \
  EditorUI::Color3(entity.GetComponent<T>()->name, #name); 

	#define COMPONENT_VIEWER_EDITABLE_STRING(name) \
	std::string& str = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<T>()->name); \
	EditorUI::EditableTextField(str, #name);

	#define COMPONENT_VIEWER_STRING(name) \
	std::string& str = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<T>()->name); \
	EditorUI::TextField(str);

	#define COMPONENT_VIEWER_END(name) \
}

	#define REGISTER_COMPONENT_VIEWER(TYPE) \
	ComponentViewRegistry::RegisterComponent(#TYPE, COMPONENT_VIEWER_##TYPE);

}