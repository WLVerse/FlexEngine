#include "assetdropmanager.h"
#include <iostream>
#include <imgui.h>

void DropManager::Initialize(HWND hwnd)
{
	m_hwnd = hwnd;
}


ULONG DropManager::AddRef() { return 1; }
ULONG DropManager::Release() { return 0; }

// we handle drop targets, let others know
HRESULT DropManager::QueryInterface(REFIID riid, void** ppvObject)
{
	if (riid == IID_IDropTarget)
	{
		*ppvObject = this;	// or static_cast<IUnknown*> if preferred
		// AddRef() if doing things properly
		// but then you should probably handle IID_IUnknown as well;
		return S_OK;
	}

	*ppvObject = NULL;
	return E_NOINTERFACE;
};

// occurs when we drag files into our applications view
HRESULT DropManager::DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	// TODO: check whether we can handle this type of object at all and set *pdwEffect &= DROPEFFECT_NONE if not;

	// Simulate left mouse button down
	ImGuiIO& io = ImGui::GetIO();
	io.MouseDown[0] = true;
	// Update the mouse position based on the POINTL provided by drag-enter event
	io.MousePos = ImVec2(static_cast<float>(pt.x), static_cast<float>(pt.y));

	// trigger MouseDown for button 1 within ImGui
	*pdwEffect &= DROPEFFECT_COPY;
	return S_OK;
}

// occurs when we drag files out from our applications view
HRESULT DropManager::DragLeave() 
{
	return S_OK; 
}

// occurs when we drag the mouse over our applications view whilst carrying files (post Enter, pre Leave)
HRESULT DropManager::DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	// trigger MouseMove within ImGui, position is within pt.x and pt.y
	// grfKeyState contains flags for control, alt, shift etc

	// Update mouse position while dragging
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2(static_cast<float>(pt.x), static_cast<float>(pt.y));

	*pdwEffect &= DROPEFFECT_COPY;
	return S_OK;
}

// occurs when we release the mouse button to finish the drag-drop operation
HRESULT DropManager::Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{

	// trigger MouseUp for button 1 within ImGui
	// Simulate releasing the left mouse button
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2(static_cast<float>(pt.x), static_cast<float>(pt.y));
	io.MouseDown[0] = false;

	std::vector<std::string> file_paths;

	// grfKeyState contains flags for control, alt, shift etc

	// render the data into stgm using the data description in fmte
	FORMATETC fmte = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stgm;

	if (SUCCEEDED(pDataObj->GetData(&fmte, &stgm)))
	{
		HDROP hdrop = (HDROP)stgm.hGlobal; // or reinterpret_cast<HDROP> if preferred
		UINT file_count = DragQueryFile(hdrop, 0xFFFFFFFF, NULL, 0);

		// we can drag more than one file at the same time, so we have to loop here
		for (UINT i = 0; i < file_count; i++)
		{
			TCHAR szFile[MAX_PATH];
			UINT cch = DragQueryFile(hdrop, i, szFile, MAX_PATH);
			if (cch > 0 && cch < MAX_PATH)
			{
				// szFile contains the full path to the file, do something useful with it
				// i.e. add it to a vector or something
				std::wstring ws(szFile);
				std::string s(ws.begin(), ws.end());
				file_paths.push_back(s);
			}
		}

		// we have to release the data when we're done with it
		ReleaseStgMedium(&stgm);

		// notify our application somehow that we've finished dragging the files (provide the data somehow)
		if (m_file_drop_callback)
		{
			m_file_drop_callback(file_paths);
		}
	}


	*pdwEffect &= DROPEFFECT_COPY;
	return S_OK;
}

