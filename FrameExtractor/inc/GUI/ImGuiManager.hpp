/******************************************************************************/
/*!
\file       ImGuiManager.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 10, 2025
\brief      Declares the ImGui Manager class which manages the overall ImGui
			interface and its components.
 ******************************************************************************/

#ifndef ImGuiManager_HPP
#define ImGuiManager_HPP
 // Project includes
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


	/*!***********************************************************************
		\brief
			Structure that holds the editor preferences for the ImGui interface.
			This includes appearance settings, autosave settings, etc.
	*************************************************************************/
	struct EditorPreferences
	{

		/*!***********************************************************************
			\brief
				Structure that holds the appearance preferences for the ImGui interface.
		*************************************************************************/
		struct AppearancePreferences
		{
			uint32_t BackGroundColor = 0x25213100;	
			uint32_t TextColor = 0xF4F1DE00;
			uint32_t MainColor = 0xDA115E00;
			uint32_t MainAccentColor = 0x79235900;
			uint32_t HighlightColor = 0xC7EF0000;
			float FontSize = 1.f;
		};

		/*!***********************************************************************
			\brief
				Structure that holds the general preferences for the ImGui interface.
				This includes autosave settings.
		*************************************************************************/
		struct GeneralPreferences
		{
			uint32_t AutosaveInterval = 5; // in minutes
			bool UseAutosave = true;
		};
		GeneralPreferences mGeneral;
		AppearancePreferences mAppearance;
	};

	/*!***********************************************************************
		\brief
			Class that manages the ImGui interface and its components.
			It initializes, updates, and renders the ImGui interface.
	*************************************************************************/
	class ImGuiManager
	{
	public:

		/*!***********************************************************************
			\brief
				Default constructor for ImGuiManager.
		*************************************************************************/
		ImGuiManager();

		/*!***********************************************************************
			\brief
				Default destructor for ImGuiManager.
		*************************************************************************/
		~ImGuiManager();

		/*!***********************************************************************
			\brief
				Initializes the ImGui interface and its components.
		*************************************************************************/
		void Init();

		/*!***********************************************************************
			\brief
				Shuts down the ImGui interface and frees resources.
		*************************************************************************/
		void Shutdown();

		/*!***********************************************************************
			\brief
				Updates the ImGui interface and its components.
			\param dt
				The delta time since the last update.
		*************************************************************************/
		void Update(float dt);

		/*!***********************************************************************
			\brief
				Renders the ImGui interface and its components.
		*************************************************************************/
		void Render();

		static float styleMultiplier;		//<- Multiplier for scaling UI elements based on window size
		static ImFont* BoldFont;			//<- Font for bold text
		static ImFont* LightFont;			//<- Font for light text
		static ImFont* RegularFont;			//<- Font for regular text
		static ImFont* FrameRateFont;		//<- Font for displaying frame rates


		/*!***********************************************************************
			\brief
				Callback function for quitting the application.
		*************************************************************************/
		static void QuitCallback();
	private:

		/*!***********************************************************************
			\brief
				Saves the editor preferences to a file.
		*************************************************************************/
		void SavePreferences();

		/*!***********************************************************************
			\brief
				Loads the editor preferences from a file.
		*************************************************************************/
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