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
class ImFont;

namespace FrameExtractor
{
	class ViewportPanel;
	class ToolsPanel;
	class ConsolePanel;
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
	private:
		// panels
		ViewportPanel* mViewportPanel;
		ToolsPanel* mToolsPanel;
		ConsolePanel* mConsolePanel;
		static float mFontSize;
	};
}

#endif
