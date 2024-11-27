#include "editorcommands.h"

using namespace FlexEngine::FlexECS;

namespace ChronoDrift
{
	using namespace FlexEngine;

	//The system itself
	void EditorCommands::Init()
	{
	}

	void EditorCommands::Update()
	{
		if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
		{
			if (ImGui::IsKeyPressed(ImGuiKey_Z))
			{
				std::cout << "Command Recieved to undo!\n";
				Undo();
			}
			if (ImGui::IsKeyPressed(ImGuiKey_Y))
			{
				std::cout << "Command Recieved to REDO!\n";
				Redo();
			}
		}
	}

	void EditorCommands::Shutdown()
	{
		std::cout << m_undo_list.size() << " size of my undo list\n";
		std::cout << m_undo_list.size() << " size of my redo list\n";
		std::cout << "clearing stuff lol\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

		// Clear undo list
		while (!m_undo_list.empty())
		{
			std::cout << "clearing stuff lol\n";
			m_undo_list.pop();
		}

		// Clear redo list
		while (!m_redo_list.empty())
		{
			std::cout << "popping stuff lol\n";
			m_redo_list.pop();
		}

		std::cout << m_undo_list.size() << " size of my undo list\n";
		std::cout << m_undo_list.size() << " size of my redo list\n";
	}

	void EditorCommands::UpdateComponent(FlexEngine::FlexECS::Entity target, FlexEngine::FlexECS::ComponentID component_name, const void* old_value, const void* new_value, size_t size)
	{
		std::cout << "Adding a new command to my m_undo_list!\n";
		auto command = std::make_unique<UpdateComponentCommand>(target, component_name, old_value, new_value, size);
		m_undo_list.push(std::move(command));
	}

	void EditorCommands::Undo(void)
	{
		std::cout << "Trying to undo!\n";
		if (!m_undo_list.empty())
		{
			std::cout << "Undo list not empty, lets REALLy try to undo now!\n";
			m_undo_list.top()->Undo();
			m_redo_list.push(std::move(m_undo_list.top()));
			m_undo_list.pop();
		}
	}

	void EditorCommands::Redo(void)
	{
		std::cout << "Trying to REDO!\n";
		if (!m_redo_list.empty())
		{
			std::cout << "REDO list not empty, lets REALLy try to REDO now!\n";
			m_redo_list.top()->Do();
			m_undo_list.push(std::move(m_redo_list.top()));
			m_redo_list.pop();
		}
	}








	UpdateComponentCommand::UpdateComponentCommand(FlexECS::Entity target, FlexECS::ComponentID component_id, const void* old_value, const void* new_value, size_t component_size)
		: m_target(target), m_component_id(component_id), m_old_value(new char[component_size]), m_new_value(new char[component_size]), m_size(component_size)
	{
		memcpy(m_old_value.get(), old_value, m_size);
		memcpy(m_new_value.get(), new_value, m_size);
	}

	UpdateComponentCommand::~UpdateComponentCommand()
	{
		std::cout << "Destroying the UPDATECOMPONENTCOMMAND command!\n";
	}

	void UpdateComponentCommand::Do()
	{
		//std::cout << "doing the test!\n";

		EntityRecord& entity_record = ENTITY_INDEX[m_target];
		Archetype& archetype = *entity_record.archetype;

		ArchetypeMap& archetype_map = COMPONENT_INDEX[m_component_id];

		// get the component data
		ArchetypeRecord& archetype_record = archetype_map[archetype.id];
		ComponentData<void> component_data = archetype.archetype_table[archetype_record.column][entity_record.row];
		void* data = Internal_GetComponentData(component_data).second;

		//std::cout << "hi!!!! current Position data: " << out_component->position.x << ", " << out_component->position.y << '\n';
		memcpy(data, m_new_value.get(), m_size);
		//std::cout << "MemCOPY ATTACK!: " << out_component->position.x << ", " << out_component->position.y << "\n";
	}

	void UpdateComponentCommand::Undo()
	{

		std::cout << "doing the Funny undo!\n";

		EntityRecord& entity_record = ENTITY_INDEX[m_target];
		Archetype& archetype = *entity_record.archetype;

		ArchetypeMap& archetype_map = COMPONENT_INDEX[m_component_id];

		// get the component data
		ArchetypeRecord& archetype_record = archetype_map[archetype.id];
		ComponentData<void> component_data = archetype.archetype_table[archetype_record.column][entity_record.row];
		void* data = Internal_GetComponentData(component_data).second;

		memcpy(data, m_old_value.get(), m_size);
	}

}



//__FLX_API ComponentData<void> Internal_CreateComponentData(std::size_t size, void* data)
//{
//	// Create a new data structure
//	void* ptr = new char[sizeof(std::size_t) + size];
//	if (!ptr)
//	{
//		FLX_ASSERT(false, "Failed to allocate memory for component data!");
//		return nullptr;
//	}

//	// Copy the size of the data
//	memcpy(ptr, &size, sizeof(std::size_t));

//	// Copy the data
//	memcpy(reinterpret_cast<std::size_t*>(ptr) + 1, data, size);

//	return ComponentData<void>(ptr, [](void* ptr) { delete[] reinterpret_cast<char*>(ptr); });
//}