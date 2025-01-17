#include "editor.h"
#include "assetbrowser.h"
#include "editorgui.h"
#include <filesystem>

namespace Editor
{
	void AssetBrowser::Init()
	{
		LoadAllDirectories();
	}

	void AssetBrowser::Update()
	{
		if (m_should_delete_file)
		{
			if (std::filesystem::exists(m_file_to_delete))
			{
				std::filesystem::remove(m_file_to_delete);
				m_should_delete_file = false;
				m_file_to_delete.clear();
				LoadAllDirectories();
			}
		}
	}
	void AssetBrowser::Shutdown()
	{
		m_root_folder.subfolders.clear();
		m_directories.clear();
	}

	void AssetBrowser::LoadAllDirectories()
	{
		m_root_folder.files.clear(); 
		m_root_folder.subfolders.clear();
		m_font_paths.clear();
		m_directories.clear();

		for (const auto& entry : std::filesystem::recursive_directory_iterator(m_root_directory))
		{
			AddToDirectoryStructure(entry);
		}

		//Populate root folder with directories
		for (auto& [folder_path, folder] : m_directories)
		{
			if (folder_path.parent_path() == "")
			{
				m_root_folder.subfolders[folder_path] = &folder;
			}
		}
	}

	void AssetBrowser::AddToDirectoryStructure(const std::filesystem::path& entry)
	{
		auto relative_path = std::filesystem::relative(entry, m_root_directory);
		auto parent_path = relative_path.parent_path();

		if (relative_path.extension().string() == ".ttf")
		{
			m_font_paths.push_back("\\" + relative_path.string());
		}

		Folder* current_folder = nullptr;
		//check which folder this entry is within
		for (auto& [folder_name, folder] : m_directories)
		{
			if (parent_path == folder.path)
			{
				current_folder = &folder;
				break;
			}
		}

		if (std::filesystem::is_directory(entry))
		{
			//Add to directory list if havent yet
			if (m_directories.find(relative_path) == m_directories.end())
			{
				m_directories[relative_path] = Folder(relative_path);
				m_directories[relative_path].name = relative_path.filename().string();

			}

			//Add to the parent.subfolder if its a is directory
			if (m_directories.find(parent_path) != m_directories.end())
			{
				m_directories[parent_path].subfolders[relative_path] = &m_directories[relative_path];
			}
		}
		else
		{
			//Check if top level file
			if (parent_path.string() == "")
			{
				m_root_folder.files.push_back(relative_path);
			}
			else
			{
				// Add file to folder normally
				m_directories[parent_path].files.push_back(relative_path);
			}
		}
	}

	void AssetBrowser::RenderFolderDropdown(Folder& folder)
	{
		ImGuiTreeNodeFlags node_flags = (folder.subfolders.size() < 1) ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_OpenOnArrow;

		// Highlight the selected folder
		if (m_selected_folder == &folder)
			node_flags |= ImGuiTreeNodeFlags_Selected;

		// Render folder node
		if (ImGui::TreeNodeEx((folder.name + "##" + folder.path.string()).c_str(), node_flags))
		{
			// Check if the folder is clicked
			if (ImGui::IsItemClicked())
			{
				m_selected_folder = &folder; // Set the selected folder
			}

			// Render subfolders recursively
			for (auto& [subfolder_path, subfolder] : folder.subfolders)
			{
				RenderFolderDropdown(*subfolder);
			}

			ImGui::TreePop();
		}
	}

	void AssetBrowser::DeleteFilePopup(std::filesystem::path& file)
	{
		//Deleting files
		if (ImGui::BeginPopupContextItem(file.filename().string().c_str()))
		{
			if (ImGui::MenuItem("Delete"))
			{
				if (std::filesystem::exists(m_root_directory / file))
				{
					m_should_delete_file = true;
					m_file_to_delete = m_root_directory / file;
				}
			}
			ImGui::EndPopup();
		}
	}

	void AssetBrowser::RenderFolder(Folder& folder)
	{
		if (ImGui::TreeNodeEx(folder.name.c_str()))
		{
			// Render directories first
			for (auto& [subfolder_path, subfolder] : folder.subfolders)
			{
				RenderFolder(*subfolder);
			}

			// Render the rest of the files in the folder
			for (auto& file : folder.files)
			{
				//Render the file name
				ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
				bool is_selected = (m_selected_file == file);
				if (is_selected) node_flags |= ImGuiTreeNodeFlags_Selected;

				ImGui::TreeNodeEx(file.filename().string().c_str(), node_flags);
				if (ImGui::IsItemClicked())
				{
					m_selected_file = file;
				}

				//Drag and drop assets
				if (ImGui::BeginDragDropSource())
				{
					std::string payload(file.string());
					payload.insert(0, "\\");	//to fit the AssetKey format
					std::string extension = file.extension().string();
					//hardcode for now
					if (extension == ".jpg" || extension == ".jpeg" || extension == ".png")
					{
						EditorGUI::StartPayload(PayloadTags::IMAGE, payload.c_str(), payload.size() + 1, file.filename().string().c_str());
						EditorGUI::EndPayload();
					}
					else if (extension == ".glsl" || extension == ".hlsl" || extension == ".frag" || extension == ".vert")
					{
						payload = payload.substr(0, payload.find_last_of('.')); //to fit the AssetKey::Shader format
						EditorGUI::StartPayload(PayloadTags::SHADER, payload.c_str(), payload.size() + 1, file.filename().string().c_str());
						EditorGUI::EndPayload();
					}
					else if (extension == ".mp3" || extension == ".wav" || extension == ".ogg" || extension == ".flac")
					{
						EditorGUI::StartPayload(PayloadTags::AUDIO, payload.c_str(), payload.size() + 1, file.filename().string().c_str());
						EditorGUI::EndPayload();
					}
					else if (extension == ".flxprefab")
					{
						EditorGUI::StartPayload(PayloadTags::PREFAB, payload.c_str(), payload.size() + 1, file.filename().string().c_str());
						EditorGUI::EndPayload();
					}
					else if (extension == ".ttf")
					{
						EditorGUI::StartPayload(PayloadTags::FONT, payload.c_str(), payload.size() + 1, file.filename().string().c_str());
						EditorGUI::EndPayload();
					}
					else
					{
						//Find rocky if you want your filetype supported
						ImGui::Text("Unsupported file type: %s", extension.c_str());
						EditorGUI::EndPayload();
					}
				}

				DeleteFilePopup(file);
			}
			ImGui::TreePop();
		}
	}

	void AssetBrowser::EditorUI()
	{
		ImGui::Begin("Asset Browser");

		if (ImGui::Button("Refresh"))
		{
			AssetManager::Unload();
			AssetManager::Load();
			LoadAllDirectories();
		}

		ImGui::SameLine();

		if (ImGui::Button("Add File"))
		{
			ImGui::OpenPopup("AddFilePopup");
			memset(m_text_input, 0, sizeof(m_text_input));
			m_selected_folder = &m_root_folder;
		}

		if (ImGui::BeginPopup("AddFilePopup"))
		{
			ImGui::Text("Select Folder");

			for (auto& [subfolder_path, subfolder] : m_root_folder.subfolders)
			{
				RenderFolderDropdown(*subfolder);
			}

			ImGui::Separator();
			ImGui::InputText("File Name", m_text_input, IM_ARRAYSIZE(m_text_input));

			if (ImGui::Button("Create"))
			{
				if (std::strlen(m_text_input) > 0)
				{
					if (m_selected_folder == nullptr) m_selected_folder = &m_root_folder;
					std::filesystem::path new_file_path = m_root_directory / m_selected_folder->path /m_text_input;
					std::ofstream new_file(new_file_path);
					new_file.close();

					//AssetManager::Unload();
					//AssetManager::Load();
					LoadAllDirectories();
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		for (auto& [path, folder] : m_root_folder.subfolders)
		{
			RenderFolder(*folder);
		}

		for (auto& file : m_root_folder.files)
		{
			ImGui::Selectable(file.filename().string().c_str());
			DeleteFilePopup(file);
		}
		ImGui::End();
	}
}