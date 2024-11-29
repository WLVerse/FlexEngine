#ifndef GAME
#include "imgui.h"

// Commented out for favor of copying imgui.ini, because the IMGUI code is way too messy to standardsize the docking IDs and its all ahrdcoded anyway.
#define SETINSPECTORWINDOW    //ImGui::SetNextWindowSize(ImVec2(285, 468), ImGuiCond_FirstUseEver); ImGui::SetNextWindowPos(ImVec2(996, 37), ImGuiCond_FirstUseEver); ImGui::SetNextWindowDockID(0x00000004, ImGuiCond_FirstUseEver);
#define SETSTATISTICSWINDOW   //ImGui::SetNextWindowSize(ImVec2(284, 468), ImGuiCond_FirstUseEver); ImGui::SetNextWindowPos(ImVec2(996, 37), ImGuiCond_FirstUseEver); ImGui::SetNextWindowDockID(0x00000004, ImGuiCond_FirstUseEver);
#define SETSCENEWINDOW        //ImGui::SetNextWindowSize(ImVec2(733, 468), ImGuiCond_FirstUseEver); ImGui::SetNextWindowPos(ImVec2(261, 37), ImGuiCond_FirstUseEver) ; ImGui::SetNextWindowDockID(0x00000002, ImGuiCond_FirstUseEver);
#define SETGAMEWINDOW         //ImGui::SetNextWindowSize(ImVec2(733, 468), ImGuiCond_FirstUseEver); ImGui::SetNextWindowPos(ImVec2(261, 37), ImGuiCond_FirstUseEver); ImGui::SetNextWindowDockID(0x00000002, ImGuiCond_FirstUseEver);
#define SETASSETBROWSERWINDOW //ImGui::SetNextWindowSize(ImVec2(1280, 243), ImGuiCond_FirstUseEver); ImGui::SetNextWindowPos(ImVec2(0, 507), ImGuiCond_FirstUseEver); ImGui::SetNextWindowDockID(0x00000007, ImGuiCond_FirstUseEver);
#define SETTOOLSWINDOW        //ImGui::SetNextWindowSize(ImVec2(1280, 243), ImGuiCond_FirstUseEver); \
                              //ImGui::SetNextWindowPos(ImVec2(0, 507), ImGuiCond_FirstUseEver ); ImGui::SetNextWindowDockID(0x00000007, ImGuiCond_FirstUseEver);
#define SETPROFILERWINDOW     //ImGui::SetNextWindowSize(ImVec2(1280, 243), ImGuiCond_FirstUseEver); ImGui::SetNextWindowPos(ImVec2(0, 507), ImGuiCond_FirstUseEver); ImGui::SetNextWindowDockID(0x00000007, ImGuiCond_FirstUseEver);
#define SETHIERARCHYWINDOW    //ImGui::SetNextWindowSize(ImVec2(259, 468), ImGuiCond_FirstUseEver); ImGui::SetNextWindowPos(ImVec2(0, 37), ImGuiCond_FirstUseEver); ImGui::SetNextWindowDockID(0x00000001, ImGuiCond_FirstUseEver);

// I wonder if these are needed, but Im not sure. I will leave them here for now.
//#define SETCURSORWINDOW ImGui::SetNextWindowSize(ImVec2(240, 66)); ImGui::SetNextWindowPos(ImVec2(60, 60));
//#define SETDEBUGWINDOW ImGui::SetNextWindowSize(ImVec2(400, 400)); ImGui::SetNextWindowPos(ImVec2(116, 84));
//#define SETPROPERTYEDITORWINDOW ImGui::SetNextWindowSize(ImVec2(430, 450)); ImGui::SetNextWindowPos(ImVec2(585, 156));

//
//[Table][0xD181190E, 2]
//Column 0  Weight = 1.0000
//Column 1  Weight = 1.0000
//
//[Docking][Data]
//DockSpace       ID = 0x8B93E3BD Window = 0xA787BDB4 Pos = 320, 202 Size = 1280, 713 Split = Y
//DockNode      ID = 0x00000005 Parent = 0x8B93E3BD SizeRef = 1280, 468 Split = X
//DockNode    ID = 0x00000003 Parent = 0x00000005 SizeRef = 994, 713 Split = X
//DockNode  ID = 0x00000001 Parent = 0x00000003 SizeRef = 259, 713 Selected = 0x2E9237F7
//DockNode  ID = 0x00000002 Parent = 0x00000003 SizeRef = 733, 713 CentralNode = 1 Selected = 0xE192E354
//DockNode    ID = 0x00000004 Parent = 0x00000005 SizeRef = 284, 713 Selected = 0xE7039252
//DockNode      ID = 0x00000007 Parent = 0x8B93E3BD SizeRef = 1280, 243 Selected = 0xD44407B5
#endif