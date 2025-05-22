/******************************************************************************
/*!
\file       ImGuiManager.hpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 10, 2024
\brief      Declares the ImGui Manager class which manages the overall ImGui
			interface and its components.

 /******************************************************************************/

#ifndef ImGuiManager_HPP
#define ImGuiManager_HPP

#include <Core/Project.hpp>
struct ImFont;

namespace FrameExtractor
{
	// Forward Declarations
	class ViewportPanel;
	class ToolsPanel;
	class ProjectPanel;
	class ConsolePanel;
	class ExplorerPanel;

	struct EditorPreferences
	{
		
		struct AppearancePreferences
		{
			uint32_t BackGroundColor = 0x25213100;
			uint32_t TextColor = 0xF4F1DE00;
			uint32_t MainColor = 0xDA115E00;
			uint32_t MainAccentColor = 0x79235900;
			uint32_t HighlightColor = 0xC7EF0000;
			float FontSize = 1.f;
		};
		struct GeneralPreferences
		{
			uint32_t AutosaveInterval = 5; // in minutes
			bool UseAutosave = true;
		};
		GeneralPreferences mGeneral;
		AppearancePreferences mAppearance;
	};

	class ImGuiManager
	{
	public:
		// constructor
		ImGuiManager();
		// destructor
		~ImGuiManager();

		// initialize ImGui
		void Init();
		// shutdown ImGui
		void Shutdown();
		// update ImGui
		void Update(float dt);
		// render ImGui
		void Render();
		static float styleMultiplier;
		static ImFont* BoldFont;
		static ImFont* LightFont;
		static ImFont* RegularFont;
		static ImFont* FrameRateFont;

		static void QuitCallback();
	private:

		void SavePreferences();
		void LoadPreferences();
		// panels
		ViewportPanel* mViewportPanel;
		ProjectPanel* mProjectPanel;
		ConsolePanel* mConsolePanel;
		ExplorerPanel* mExplorerPanel;
		ToolsPanel* mToolsPanel;

		Project mProject;
		EditorPreferences mPreferences;
		float mTimer = 0.f;
	};
}

#endif
