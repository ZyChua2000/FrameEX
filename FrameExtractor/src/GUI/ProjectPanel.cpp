/******************************************************************************
/*!
\file       ToolsPanel.cpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 13, 2024
\brief      Defines the Project Panel class which has an interface for all the
			videos involved in the project.

 /******************************************************************************/
#include "FrameExtractorPCH.hpp"
#include "GUI/ProjectPanel.hpp"
#include <Core/LoggerManager.hpp>
#include <GUI/ExplorerPanel.hpp>
namespace FrameExtractor
{
    ProjectPanel::ProjectPanel(ExplorerPanel* ex)
    {
        ExPanel = ex;
    }

    ProjectPanel::~ProjectPanel()
    {
    }

    void ProjectPanel::OnImGuiRender(float dt)
    {
        //ImGui::SetNextWindowSize(mViewportSize);
        //ImGui::SetNextWindowPos(mViewportPos);
        ImGui::Begin("Project");

        ImVec2 windowSize = ImGui::GetContentRegionAvail();
        ImGui::BeginChild("ScrollableRegion", ImVec2(windowSize.x, windowSize.y), true);

  

        for (const auto& entry : videosInProject)
        {
            std::string fileName = entry.filename().string();
            std::string filePath = entry.string();
            ImGui::Button(fileName.c_str());
            if (ImGui::BeginDragDropSource())
            {
                ImGui::SetDragDropPayload("ITEM_NAME", filePath.c_str(), filePath.size() + 1);
                ImGui::EndDragDropSource();
            }
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("IMPORTVIDEO")) {
                    const char* droppedItem = static_cast<const char*>(payload->Data);
                    videosInProject.push_back(droppedItem);
                }
            }
        }
        ImGui::EndChild();
        ImGui::End();
    }

    const char* ProjectPanel::GetName() const
    {
        return "Project";
    }
    
    static void AddDirectoryRecursive(const std::filesystem::directory_entry& currEntry, std::vector<std::filesystem::path>& videosInProject)
    {
        if (currEntry.is_directory())
        {
            for (auto& entry : std::filesystem::directory_iterator(currEntry))
            {
                AddDirectoryRecursive(entry, videosInProject);
            }
        }
        else if (currEntry.path().filename().extension() == ".mp4")
        {
            videosInProject.push_back(currEntry.path());
        }
        
    }

    void ProjectPanel::OnAttach()
    {
        auto rootPath = ExPanel->GetRootPath();
        for (auto& entry : std::filesystem::directory_iterator(rootPath))
        {
            AddDirectoryRecursive(entry, videosInProject);
        }
    }
}
