/******************************************************************************
/*!
\file       ToolsPanel.cpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 10, 2024
\brief      Defines the Tools Panel class which contains all the tools

 /******************************************************************************/
#include "FrameExtractorPCH.hpp"
#include "GUI/ToolsPanel.hpp"
#include <Core/LoggerManager.hpp>
namespace FrameExtractor
{
    ToolsPanel::ToolsPanel(const std::string& name, ImVec2& size, ImVec2& pos) :
        mName(name),
        mViewportSize(size),
        mViewportPos(pos)
    {

    }

    ToolsPanel::~ToolsPanel()
    {
    }

    void ToolsPanel::OnImGuiRender(float dt)
    {
        //ImGui::SetNextWindowSize(mViewportSize);
        //ImGui::SetNextWindowPos(mViewportPos);
        ImGui::Begin(mName.c_str());

        ImGui::End();
    }

    const char* ToolsPanel::GetName() const
    {
        return mName.c_str();
    }
}
