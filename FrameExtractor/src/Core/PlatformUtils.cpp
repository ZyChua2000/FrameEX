/******************************************************************************
/*!
\file       PlatformUtils.cpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 16, 2024
\brief      Defines the Platform specific utilities

 /******************************************************************************/


#include <FrameExtractorPCH.hpp>
#include <Core/PlatformUtils.hpp>
#include <Core/ApplicationManager.hpp>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
namespace FrameExtractor
{
	// Windows Implementation
	std::filesystem::path OpenFileDialog(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[MAX_PATH] = { 0 };
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)ApplicationManager::GetInstance()->GetWindowManager()->GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		CHAR szInitialDir[MAX_PATH];
		GetCurrentDirectoryA(MAX_PATH, szInitialDir);
		ofn.lpstrInitialDir = szInitialDir;
		if (GetOpenFileNameA(&ofn))
		{
			return ofn.lpstrFile;
		}
		return {};
	}

	std::filesystem::path SaveFileDialog(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[MAX_PATH] = { 0 };
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)ApplicationManager::GetInstance()->GetWindowManager()->GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		CHAR szInitialDir[MAX_PATH];
		GetCurrentDirectoryA(MAX_PATH, szInitialDir);
		ofn.lpstrInitialDir = szInitialDir;
		if (GetSaveFileNameA(&ofn))
		{
			return ofn.lpstrFile;
		}
		return {};
	}
	void CopyToClipboard(std::string text)
	{
		// Open the clipboard
		OpenClipboard(nullptr);
		// Empty the clipboard
		EmptyClipboard();

		// Allocate a global memory object for the text
		HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);


		// Lock the handle and copy the text to the buffer
		char* pGlobal = static_cast<char*>(GlobalLock(hGlobal));
		memcpy(pGlobal, text.c_str(), text.size() + 1);
		GlobalUnlock(hGlobal);

		// Place the handle on the clipboard
		SetClipboardData(CF_TEXT, hGlobal);

		// Close the clipboard
		CloseClipboard();
	}
}


