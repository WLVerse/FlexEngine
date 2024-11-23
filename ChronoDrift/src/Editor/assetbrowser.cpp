#include "editor.h"
#include "assetbrowser.h"
#include <filesystem>

namespace ChronoDrift
{
	void AssetBrowser::Init()
	{
		LoadAllDirectories();
	}

	void AssetBrowser::Update()
	{
	}
	void AssetBrowser::Shutdown()
	{
		m_root_folder.subfolders.clear();
		m_directories.clear();
	}

	void AssetBrowser::LoadAllDirectories()
	{
		m_directories.clear();

		for (const auto& entry : std::filesystem::recursive_directory_iterator(m_root_directory))
		{
			AddToDirectoryStructure(entry);
		}
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
					
					if (FLX_EXTENSIONS_CHECK_SAFETY("image", file.extension().string()))
					{
						EditorGUI::StartPayload(PayloadTags::IMAGE, payload.c_str(), payload.size() + 1, file.filename().string().c_str());
						EditorGUI::EndPayload();
					}
					else if (FLX_EXTENSIONS_CHECK_SAFETY("shader", file.extension().string()))
					{
						payload = payload.substr(0, payload.find_last_of('.')); //to fit the AssetKey::Shader format
						EditorGUI::StartPayload(PayloadTags::SHADER, payload.c_str(), payload.size() + 1, file.filename().string().c_str());
						EditorGUI::EndPayload();
					}
					else if (FLX_EXTENSIONS_CHECK_SAFETY("audio", file.extension().string()))
					{
						EditorGUI::StartPayload(PayloadTags::AUDIO, payload.c_str(), payload.size() + 1, file.filename().string().c_str());
						EditorGUI::EndPayload();
					}
					else if (FLX_EXTENSIONS_CHECK_SAFETY("flx", file.extension().string()))
					{
						EditorGUI::StartPayload(PayloadTags::PREFAB, payload.c_str(), payload.size() + 1, file.filename().string().c_str());
						EditorGUI::EndPayload();
					}
					else
					{
						//Find rocky if you want your filetype supported
						ImGui::Text("Unsupported file type: %s", file.extension().string().c_str());
						EditorGUI::EndPayload();
					}
				}
			}
			ImGui::TreePop();
		}
	}

	void AssetBrowser::EditorUI()
	{
		ImGui::Begin("Asset Browser");

		for (auto& [path, folder] : m_root_folder.subfolders)
		{
			RenderFolder(*folder);
		}

		for (const auto& file : m_root_folder.files)
		{
			ImGui::Selectable(file.filename().string().c_str());
		}
		ImGui::End();
	}
}




//void AssetBrowser::RenderFolder(Folder& folder)
//{
//	if (ImGui::TreeNodeEx(folder.name.c_str()))
//	{
//		// Render directories first
//		for (auto& [subfolder_path, subfolder] : folder.subfolders)
//		{
//			RenderFolder(*subfolder);
//		}
//
//		// Render the rest of the files in the folder
//		for (auto& file : folder.files)
//		{
//			bool renaming = m_is_renaming && m_selected_file == file;
//			if (ImGui::IsKeyPressed(ImGuiKey_F2) && m_selected_file == file)
//			{
//				m_is_renaming = true;
//			}
//			if (renaming)
//			{
//				static char renameBuffer[256];
//				strncpy_s(renameBuffer, file.filename().string().c_str(), sizeof(renameBuffer));
//
//				if (ImGui::InputText("##RenameAsset", renameBuffer, sizeof(renameBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
//				{
//					// Rename confirmed
//					std::filesystem::path newPath = file.parent_path() / renameBuffer;
//					std::cout << "Old path: " << file.string() << "   new path: " << newPath.string() << "\n";
//
//					LoadAllDirectories();
//					m_is_renaming = false;
//				}
//			}
//			else
//			{
//				//Render the file name
//				ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
//				bool is_selected = (m_selected_file == file);
//				if (is_selected) node_flags |= ImGuiTreeNodeFlags_Selected;
//
//				ImGui::TreeNodeEx(file.filename().string().c_str(), node_flags);
//				if (ImGui::IsItemClicked())
//				{
//					m_selected_file = file;
//				}
//
//				//Drag and drop for sprite component
//				if (ImGui::BeginDragDropSource())
//				{
//					std::string payload(file.string());
//					payload.insert(0, "\\");	//to fit the AssetKey format
//
//					if (FLX_EXTENSIONS_CHECK_SAFETY("image", file.extension().string()))
//					{
//						ImGui::SetDragDropPayload("IMAGE_PATH", payload.c_str(), payload.size() + 1);	//+ 1 for null terminating
//						ImGui::Text(file.filename().string().c_str()); // Show the name during drag
//						ImGui::EndDragDropSource();
//					}
//				}
//			}
//		}
//		ImGui::TreePop();
//	}
//}