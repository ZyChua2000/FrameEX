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
	class ToolsPanel : public IPanel
	{
	public:
		ToolsPanel(const std::string& name, ImVec2& size, ImVec2& pos);
		~ToolsPanel() override;
		virtual void OnImGuiRender(float dt) override;
		virtual const char* GetName() const override;
	private:
		std::string mName;
		ImVec2 mViewportSize = ImVec2(0, 0);
		ImVec2 mViewportPos = ImVec2(0, 0);
	};

}

#endif
