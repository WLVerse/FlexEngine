#include "editorcommands.h"

using namespace FlexEngine::FlexECS;

namespace ChronoDrift
{
	UpdateComponentCommand::UpdateComponentCommand(FlexECS::Entity target, FlexECS::ComponentID component_id, const void* old_value, const void* new_value, size_t component_size)
		: m_target(target), m_component_id(component_id), m_old_value(new char[component_size]), m_new_value(new char[component_size]), m_size(component_size)
	{
		memcpy(m_old_value.get(), old_value, m_size);
		memcpy(m_new_value.get(), new_value, m_size);
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

	void UpdateComponentCommand::Do()
	{
		std::cout << "doing the test!\n";

		EntityRecord& entity_record = ENTITY_INDEX[m_target];
		Archetype& archetype = *entity_record.archetype;

		ArchetypeMap& archetype_map = COMPONENT_INDEX[m_component_id];

		// get the component data
		ArchetypeRecord& archetype_record = archetype_map[archetype.id];
		ComponentData<void> component_data = archetype.archetype_table[archetype_record.column][entity_record.row];
		void* data = Internal_GetComponentData(component_data).second;
		Position* out_component = reinterpret_cast<Position*>(data);
		//std::cout << "hi!!!! current Position data: " << out_component->position.x << ", " << out_component->position.y << '\n';

		memcpy(data, m_new_value.get(), m_size);
		//std::cout << "MemCOPY ATTACK!: " << out_component->position.x << ", " << out_component->position.y << "\n";
	}

	void UpdateComponentCommand::Undo()
	{
	}



}