/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// prefabeditor.cpp
//
// Brief
//
// AUTHORS
// [100%] Ho Jin Jie Donovan (h.jinjiedonovan\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#include "editor.h"
#include "prefabeditor.h"
#include "flxdata.h"

using namespace FlexEngine;
using EntityName = FlexEngine::FlexECS::Scene::StringIndex;

namespace Editor
{
	void PrefabEditor::Init()
	{}
	void PrefabEditor::Update()
	{}
	void PrefabEditor::Shutdown()
	{}

	void PrefabEditor::EditorUI()
	{
		ImGui::Begin("Prefab Editor");

		// Create New Prefab
		if (ImGui::Button("Create Prefab")) {
			file_name = "flxer.flxdata";
			prefab_keys.clear();
			prefab_keys.push_back(std::make_tuple("test", "test", true));
			prefab_keys.push_back(std::make_tuple("tester", "tester", std::string("Hello2 World!")));
			prefab_keys.push_back(std::make_tuple("testint", "testint", 1243));
		}
		ImGui::SameLine(175);

		// Load Existing Prefab
		if (ImGui::Button("Load Prefab")) {
			prefab_keys.clear();
			FileList files = FileList::Browse(
				"Open Prefab",
				Path::current(), "default.flxdata",
				L"FlexScene Files (*.flxdata)\0" L"*.flxdata\0",
				//L"All Files (*.*)\0" L"*.*\0",
				false,
				false
			);
			
			// cancel if no files were selected
			if (files.empty()) {
				Log::Error("No file was selected");
				std::cout << Path::current().string() << std::endl;
			}
			else {
				// open the prefab
				File& file = File::Open(files[0]);
				
				// read the file details
				std::istringstream input_stream(file.Read());
				std::string temp;
				file_name = file.path.filename().string();
				// Access the prefab
				Asset::FlxData prefab(file_name);

				while (std::getline(input_stream, temp)) {
					size_t start_pos = temp.find('\"');
					if (start_pos != std::string::npos) {
						size_t end_pos = temp.find('\"', start_pos + 1);
						std::string key_name = temp.substr(start_pos + 1, end_pos - start_pos - 1);
						// Skip unneeded line
						if (key_name == "flxdata") continue;

						if (temp.find('\"', end_pos + 1) != std::string::npos) {
							// String Data Type
							prefab_keys.push_back(std::make_tuple(key_name,key_name, prefab.GetString(key_name, "")));
						}
						else {
							size_t colon_pos = temp.find(':', end_pos + 1);
							if (temp.substr(colon_pos + 2, 4) == "true" || temp.substr(colon_pos + 2, 5) == "false") {
								// Boolean Data Type
								prefab_keys.push_back(std::make_tuple(key_name, key_name, prefab.GetBool(key_name, false)));
							}
							else {
								// Integer Data Type
								prefab_keys.push_back(std::make_tuple(key_name, key_name, prefab.GetInt(key_name, 0)));
							}
						}
					}
				}
			}
		}
		// Prefab Editor Imgui editable fields
		if (!prefab_keys.empty()) {
			for (int i = 0; i < prefab_keys.size(); i++) {
				char field_name[128];
				strncpy_s(field_name, std::get<1>(prefab_keys[i]).c_str(), sizeof(field_name) - 1);
				field_name[sizeof(field_name) - 1] = '\0';

				ImGui::PushID(i);
				ImGui::PushItemWidth(175); // Set the width to 175 pixels
				if (ImGui::InputText("##hidden_label", field_name, sizeof(field_name))) {
					// The text has been changed by the user
					// You can handle the new text here
					std::get<1>(prefab_keys[i]) = std::string(field_name);
				}
				ImGui::PopItemWidth(); // Restore the default width
				ImGui::PopID();

				ImGui::SameLine(200);

				// Push a unique ID for each item to avoid conflicts
				ImGui::PushID(static_cast<int>(prefab_keys.size() + i));

				if (std::get_if<std::string>(&std::get<2>(prefab_keys[i]))) {
					char text[256];
					strncpy_s(text, std::get<std::string>(std::get<2>(prefab_keys[i])).c_str(), sizeof(text) - 1);
					text[sizeof(text) - 1] = '\0';
					
					if (ImGui::InputText("##hidden_label", text, sizeof(text))) {
						// The text has been changed by the user
						// You can handle the new text here
						std::get<2>(prefab_keys[i]) = std::string(text);
					}
				}
				else if (std::get_if<int>(&std::get<2>(prefab_keys[i]))) {
					if (ImGui::InputInt("##hidden_label", &std::get<int>(std::get<2>(prefab_keys[i])))) {
						// The number has been changed by the user
						// Handle the updated integer here
					}
				}
				else if (std::get_if<bool>(&std::get<2>(prefab_keys[i]))) {
					if (ImGui::Checkbox("##hidden_label", &std::get<bool>(std::get<2>(prefab_keys[i])))) {
						// The checkbox value has been changed
						// Handle the updated state here
					}
				}
				ImGui::PopID();
			}
			// To add new fields for the prefab
			if (ImGui::BeginMenu("Add field")) {
				if (ImGui::MenuItem("String")) {
					prefab_keys.push_back(std::make_tuple("tester","tester", std::string("Hello2 World!")));
				}
				if (ImGui::MenuItem("Integer")) {
					prefab_keys.push_back(std::make_tuple("testint","testint", 1243));
				}
				if (ImGui::MenuItem("Boolean")) {
					prefab_keys.push_back(std::make_tuple("test","test", true));
				}
				ImGui::EndMenu();
			}
		}

		if (ImGui::Button("Save Prefab")) {
			if (file_name == "") {
				Log::Error("No file selected or created");
			}
			else {
				Asset::FlxData prefab_to_save(file_name);
				for (int i = 0; i < prefab_keys.size(); i++) {
					// If key name is changed, delete old key and create new key
					if (std::get<1>(prefab_keys[i]) != std::get<0>(prefab_keys[i])) {
						prefab_to_save.DeleteKey(std::get<0>(prefab_keys[i]));
					}
					if (std::get_if<std::string>(&std::get<2>(prefab_keys[i]))) {
						prefab_to_save.SetString(std::get<1>(prefab_keys[i]), std::get<std::string>(std::get<2>(prefab_keys[i])));
					}
					else if (std::get_if<int>(&std::get<2>(prefab_keys[i]))) {
						prefab_to_save.SetInt(std::get<1>(prefab_keys[i]), std::get<int>(std::get<2>(prefab_keys[i])));
					}
					else if (std::get_if<bool>(&std::get<2>(prefab_keys[i]))) {
						prefab_to_save.SetBool(std::get<1>(prefab_keys[i]), std::get<bool>(std::get<2>(prefab_keys[i])));
					}
				}
				prefab_to_save.Save();
			}
		}
		ImGui::End();
	}
}