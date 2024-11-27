#pragma once

#include "Editor/editorsystem.h"
#include <FlexECS/datastructures.h>
#include <stack>
#include <any>

namespace ChronoDrift
{
	struct Command
	{
	public:
    virtual ~Command() = default;
		virtual void Do(void) = 0;
		virtual void Undo(void) = 0;
	};

	struct UpdateComponentCommand : public Command
	{
		UpdateComponentCommand(FlexEngine::FlexECS::Entity target, FlexEngine::FlexECS::ComponentID component_name, const void* old_value, const void* new_value, size_t size);

		void Do() override;
		void Undo() override;

		FlexEngine::FlexECS::Entity m_target;
		FlexEngine::FlexECS::ComponentID m_component_id;
		std::unique_ptr<char[]> m_old_value;
		std::unique_ptr<char[]> m_new_value;
		size_t m_size;
	};

	class EditorCommands : public EditorSystem
	{
	public:
		void Init() override;
		void Update() override;
		void Shutdown() override;

		void UpdateComponent(FlexEngine::FlexECS::Entity target, FlexEngine::FlexECS::ComponentID component_name, const void* old_value, const void* new_value, size_t size);

		void Undo(void);
		void Redo(void);

	private:
		std::stack<std::unique_ptr<Command>> m_undo_list;
		std::stack<std::unique_ptr<Command>> m_redo_list;
	};


}