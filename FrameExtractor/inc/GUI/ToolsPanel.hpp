/******************************************************************************
/*!
\file       ToolsPanel.hpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 10, 2024
\brief      Declares the Tools Panel class which has all the tools.

 /******************************************************************************/

#ifndef ToolsPanel_HPP
#define ToolsPanel_HPP
#include <string>
#include <imgui.h>
#include <GUI/IPanel.hpp>
namespace FrameExtractor
{
	class ExplorerPanel;

	class ToolsPanel : public IPanel
	{
	public:
		ToolsPanel(ExplorerPanel* ex);
		~ToolsPanel() override;
		virtual void OnImGuiRender(float dt) override;
		virtual const char* GetName() const override;
		virtual void OnAttach() override;
	private:
		std::vector<std::filesystem::path> videosInProject;
		ExplorerPanel* ExPanel;

	};

}

#endif
