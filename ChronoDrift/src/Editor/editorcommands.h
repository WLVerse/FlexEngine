#pragma once

#include "Editor/editor.h"
#include <FlexECS/datastructures.h>
#include <stack>
#include <any>

namespace ChronoDrift
{
	class EditorSystem
	{
	public:
		/************************** Virtual functions to implement *************************/
		virtual void Init() = 0;
		virtual void Update() = 0;
		virtual void Shutdown() = 0;
	};

	struct Command
	{
	public:
		virtual void Do(void) = 0;
		virtual void Undo(void) = 0;
	};

	struct UpdateComponentCommand : Command
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

	class EditorCommands : EditorSystem
	{
	public:
		void Init();
		void Update();
		void Shutdown();

		void Undo(void);
		void Redo(void);

	private:
		std::stack<Command> m_undo_list;
		std::stack<Command> m_redo_list;

	};

}