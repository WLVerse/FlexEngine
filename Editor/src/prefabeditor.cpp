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

		// Save/Load Prefabs
		if (ImGui::Button("Create Prefab")) {
			// Data: flxfile.data
			/*Asset::FlxData flex("data/flxer.flxdata");
			flex.SetBool("test", true);
			flex.SetString("tester", "Hello2 World!");
			flex.SetInt("testint", 1243);
			flex.Save();*/
		}

		if (ImGui::Button("Load Prefab")) {
			prefab_keys.clear();
			FileList files = FileList::Browse(
				"Open Prefab",
				Path::current("prefabs"), "default.flxdata",
				L"FlexScene Files (*.flxdata)\0" L"*.flxdata\0",
				//L"All Files (*.*)\0" L"*.*\0",
				false,
				false
			);

			// cancel if no files were selected
			if (files.empty()) Log::Error("No file was selected");
			else {
				// open the prefab
				File& file = File::Open(files[0]);

				std::istringstream input_stream(file.Read());
				std::string temp;
				file_name = file.path.filename().string();
				Asset::FlxData prefab(file_name);

				while (std::getline(input_stream, temp)) {
					size_t start_pos = temp.find('\"');
					if (start_pos != std::string::npos) {
						size_t end_pos = temp.find('\"', start_pos + 1);
						std::string key_name = temp.substr(start_pos + 1, end_pos - start_pos - 1);

						if (temp.find('\"', end_pos + 1) != std::string::npos) {
							// String Data Type
							prefab_keys.push_back(std::make_pair(key_name, prefab.GetString(key_name, "")));
						}
						else {
							size_t colon_pos = temp.find(':', end_pos + 1);
							if (temp.substr(colon_pos + 2, 4) == "true" || temp.substr(colon_pos + 2, 5) == "false") {
								// Boolean Data Type
								prefab_keys.push_back(std::make_pair(key_name, prefab.GetBool(key_name, false)));
							}
							else {
								// Integer Data Type
								prefab_keys.push_back(std::make_pair(key_name, prefab.GetInt(key_name, 0)));
							}
						}
					}
				}
			}
		}
		
		if (!prefab_keys.empty()) {
			for (int i = 1; i < prefab_keys.size(); i++) {
				ImGui::Text(prefab_keys[i].first.c_str());
				ImGui::SameLine(150);

				// Push a unique ID for each item to avoid conflicts
				ImGui::PushID(i);

				if (std::get_if<std::string>(&prefab_keys[i].second)) {
					char text[256];
					strncpy_s(text, std::get<std::string>(prefab_keys[i].second).c_str(), sizeof(text) - 1);
					text[sizeof(text) - 1] = '\0';
					
					if (ImGui::InputText("##hidden_label", text, sizeof(text))) {
						// The text has been changed by the user
						// You can handle the new text here
						prefab_keys[i].second = std::string(text);
					}
				}
				else if (std::get_if<int>(&prefab_keys[i].second)) {
					if (ImGui::InputInt("##hidden_label", &std::get<int>(prefab_keys[i].second))) {
						// The number has been changed by the user
						// Handle the updated integer here
					}
				}
				else if (std::get_if<bool>(&prefab_keys[i].second)) {
					if (ImGui::Checkbox("##hidden_label", &std::get<bool>(prefab_keys[i].second))) {
						// The checkbox value has been changed
						// Handle the updated state here
					}
				}
				ImGui::PopID();
			}
		}

		if (ImGui::Button("Save Prefab")) {
			if (file_name == "") {
				Log::Error("No file selected or created");
			}
			else {
				Asset::FlxData prefab_to_save(file_name);
				for (int i = 1; i < prefab_keys.size(); i++) {
					if (std::get_if<std::string>(&prefab_keys[i].second)) {
						prefab_to_save.SetString(prefab_keys[i].first, std::get<std::string>(prefab_keys[i].second));
					}
					else if (std::get_if<int>(&prefab_keys[i].second)) {
						prefab_to_save.SetInt(prefab_keys[i].first, std::get<int>(prefab_keys[i].second));
					}
					else if (std::get_if<bool>(&prefab_keys[i].second)) {
						prefab_to_save.SetBool(prefab_keys[i].first, std::get<bool>(prefab_keys[i].second));
					}
				}
				prefab_to_save.Save();
				Log::Info(file_name + " prefab file has been updated.");
			}
		}

		ImGui::End();
	}
}