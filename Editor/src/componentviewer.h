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
// Copyright (c) 2025 DigiPen, All rights reserved.
**************************************************************************/

#pragma once
#include "FlexEngine.h"
#include "editorgui.h"

namespace Editor
{
	// class PrefabView
	// { FLX_REFL_SERIALIZABLE
	// };

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
		static void RegisterComponent(std::string name, ComponentViewer display, ComponentViewer adder, ComponentViewer remover);

		/*!***************************************************************************
		* @brief
		* Checks if a component has a viewer/adder/remover defined for it.
		* @return
		* true if has, false otherwise
		******************************************************************************/
		static bool ViewerExists(std::string);
		static bool AdderExists(std::string);
		static bool RemoverExists(std::string);

		/*!***************************************************************************
		* @brief
		* Calls the viewer function for the component. This displays the details
		* of the component to the Inspector Panel in the editor.
		******************************************************************************/
		static void ViewComponent(std::string, FlexEngine::FlexECS::Entity);

		static void AddComponent(std::string, FlexEngine::FlexECS::Entity);
		static void RemoveComponent(std::string, FlexEngine::FlexECS::Entity);


		static std::vector<std::string> GetComponentList();

	private:
		static std::unordered_map<std::string, ComponentViewer> m_component_viewer;
		static std::unordered_map<std::string, ComponentViewer> m_component_adder;
		static std::unordered_map<std::string, ComponentViewer> m_component_remover;
	};


	/*!************************************************************************
	1) Use COMPONENT_VIEWER_START(ComponentName) to automatically create the function
	2) Use COMPONENT_VIEWER_xxxxx(member) with the appropriate type for every member inside the 
	component you want to display
	3) Use COMPONENT_VIEWER_END(ComponentName) to close the function (i know the define parameter is unused, for now D:)
	4) Register the function to the registry using REGISTER_COMPONENT_VIEWER(ComponentName)
	4a) Use REGISTER_COMPONENT_VIEWER_FUNCTIONS(TYPE, ENABLE_ADD, ENABLE_REMOVE) to control 
			whether adding or removing the component is allowed (default both allowed)

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
	#define COMPONENT_ENABLE_ADD (1)
	#define COMPONENT_DISABLE_ADD (0)
	#define COMPONENT_ENABLE_REMOVE (1)
	#define COMPONENT_DISABLE_REMOVE (0)




	#define COMPONENT_VIEWER_START(TYPE) \
void COMPONENT_ADDER_##TYPE(FlexEngine::FlexECS::Entity entity) \
{ \
	if(entity.HasComponent<TYPE>()) return; \
	entity.AddComponent<TYPE>({}); \
} \
\
void COMPONENT_REMOVER_##TYPE(FlexEngine::FlexECS::Entity entity) \
{ \
	if(!entity.HasComponent<TYPE>()) return; \
	entity.RemoveComponent<TYPE>(); \
} \
\
void COMPONENT_VIEWER_##TYPE(FlexEngine::FlexECS::Entity entity) \
{ \
	UNREFERENCED_PARAMETER(entity);\
  using T = TYPE;


// Using Manual means that Component add and remove functionality won't be
// automatically generated. So you can write your own in the case that special functionality is needed.
// > Make sure that the naming convention is correct for your personalized function.
	#define COMPONENT_VIEWER_START_MANUAL(TYPE) \
void COMPONENT_VIEWER_##TYPE(FlexEngine::FlexECS::Entity entity) \
{ \
  using T = TYPE;


	#define COMPONENT_VIEWER_END(name) \
}

	#pragma region Component View Functions
	//////////////////////////////////////////////
	//Viewers for each variable in the component//
	//////////////////////////////////////////////

	#define COMPONENT_VIEWER_DRAG_VECTOR2(name) \
  EditorGUI::DragFloat2(entity.GetComponent<T>()->name, #name);

	#define COMPONENT_VIEWER_DRAG_VECTOR3(name) \
  EditorGUI::DragFloat3(entity.GetComponent<T>()->name, #name); 

	#define COMPONENT_VIEWER_DRAG_INT(name) \
  EditorGUI::DragInt(entity.GetComponent<T>()->name, #name);

	#define COMPONENT_VIEWER_INT1(name) \
  EditorGUI::Int1(entity.GetComponent<T>()->name, #name);

	#define COMPONENT_VIEWER_DRAG_GLUINT(name) \
  EditorGUI::DragGLuint(entity.GetComponent<T>()->name, #name); 

	#define COMPONENT_VIEWER_FLOAT1(name) \
  EditorGUI::Float1(entity.GetComponent<T>()->name, #name);

	#define COMPONENT_VIEWER_DRAG_FLOAT(name) \
  EditorGUI::DragFloat1(entity.GetComponent<T>()->name, #name);

	#define COMPONENT_VIEWER_CHECKBOX(name) \
  EditorGUI::CreateCheckbox(entity.GetComponent<T>()->name, #name); 

	#define COMPONENT_VIEWER_ENTITY_REFERENCE(name) \
  EditorGUI::EntityReference(entity.GetComponent<T>()->name, #name); 

	#define COMPONENT_VIEWER_COLOR3(name) \
  EditorGUI::Color3(entity.GetComponent<T>()->name, #name); 

	#define COMPONENT_VIEWER_EDITABLE_STRING(name) \
	std::string& str_##name = FLX_STRING_GET(entity.GetComponent<T>()->name); \
	EditorGUI::EditableTextField(str_##name, #name);

	#define COMPONENT_VIEWER_SHADER_PATH(name) \
	std::string& path_##name = FLX_STRING_GET(entity.GetComponent<T>()->name); \
	EditorGUI::ShaderPath(path_##name);

	#define COMPONENT_VIEWER_TEXTURE_PATH(name) \
	std::string& path_##name = FLX_STRING_GET(entity.GetComponent<T>()->name); \
	EditorGUI::TexturePath(path_##name);

	#define COMPONENT_VIEWER_SPRITESHEET_PATH(name) \
	std::string& path_##name = FLX_STRING_GET(entity.GetComponent<T>()->name); \
	EditorGUI::SpritesheetPath(path_##name);

	#define COMPONENT_VIEWER_AUDIO_PATH(name) \
	std::string& path_##name = FLX_STRING_GET(entity.GetComponent<T>()->name); \
	EditorGUI::AudioPath(path_##name);

	#define COMPONENT_VIEWER_FONT_PATH(name) \
	std::string& path_##name = FLX_STRING_GET(entity.GetComponent<T>()->name); \
	EditorGUI::FontPath(path_##name);

	#define COMPONENT_VIEWER_VIDEO_PATH(name) \
	std::string& path_##name = FLX_STRING_GET(entity.GetComponent<T>()->name); \
	EditorGUI::VideoPath(path_##name);

	#define COMPONENT_VIEWER_STRING(name) \
	std::string& str_##name = FLX_STRING_GET(entity.GetComponent<T>()->name); \
	EditorGUI::TextField(str_##name);

	#define COMPONENT_VIEWER_BOOL(name) \
	EditorGUI::Checkbox(entity.GetComponent<T>()->name, #name);

	#define COMPONENT_VIEWER_MAT44(name) \
	EditorGUI::Mat44(entity.GetComponent<T>()->name, #name);

	#define COMPONENT_VIEWER_TEXT_ALIGNMENT(name) \
	EditorGUI::TextAlignment(entity.GetComponent<T>()->name);

#pragma endregion


	#define REGISTER_COMPONENT_VIEWER(TYPE) \
	ComponentViewRegistry::RegisterComponent(#TYPE, COMPONENT_VIEWER_##TYPE, COMPONENT_ADDER_##TYPE, COMPONENT_REMOVER_##TYPE);


	#define REGISTER_COMPONENT_VIEWER_FUNCTIONS(TYPE, ENABLE_ADD, ENABLE_REMOVE) \
	ComponentViewer viewer_function##TYPE  = COMPONENT_VIEWER_##TYPE; \
	ComponentViewer adder_function##TYPE	 = (ENABLE_ADD) ? COMPONENT_ADDER_##TYPE : nullptr; \
	ComponentViewer remover_function##TYPE = (ENABLE_REMOVE) ? COMPONENT_REMOVER_##TYPE : nullptr; \
	ComponentViewRegistry::RegisterComponent(#TYPE, viewer_function##TYPE, adder_function##TYPE, remover_function##TYPE);

}