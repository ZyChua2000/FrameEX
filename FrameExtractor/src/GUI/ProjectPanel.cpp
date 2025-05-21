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
#include <GUI/ViewportPanel.hpp>
#include <GUI/ImGuiManager.hpp>
namespace FrameExtractor
{
    ProjectPanel::ProjectPanel(ExplorerPanel* ex, ViewportPanel* vp, Project* project) : ExPanel(ex), VpPanel(vp), mProject(project)
    {}

    ProjectPanel::~ProjectPanel()
    {
    }

    void ProjectPanel::OnImGuiRender(float dt)
    {
        //ImGui::SetNextWindowSize(mViewportSize);
        //ImGui::SetNextWindowPos(mViewportPos);
        auto& videosInProject = mProject->mVideosInProject;
        ImGui::Begin("Project");

        ImVec2 windowSize = ImGui::GetContentRegionAvail();
        ImGui::BeginChild("ScrollableRegion", {}, ImGuiChildFlags_Border);

        ImVec2 buttonPos = ImGui::GetCursorScreenPos();
        // Create an invisible button that covers the entire child window area
        {
            auto contentTestSize = ImGui::GetContentRegionAvail();
            if (contentTestSize.x > 0 && contentTestSize.y > 0)
            {
                float scrollMaxY = ImGui::GetScrollMaxY();       // Maximum scroll position
                ImGui::Image((ImTextureID)Texture::GetInvisibleTexture()->GetTextureID(), {contentTestSize.x, contentTestSize.y + scrollMaxY});
                ImGui::SetItemAllowOverlap();
            }
        }
        

        if (ImGui::BeginDragDropTarget())
        {
            // Accept the dragged payload
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("IMPORTVIDEO")) {
                const char* droppedItem = static_cast<const char*>(payload->Data);
                videosInProject.push_back(droppedItem);
            }
            ImGui::EndDragDropTarget();
        }


        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1))  // 1 for right-click
        {
            // Open the popup when right-clicked on this button
            ImGui::OpenPopup("RightClickMenu##ProjectPanel");
        }
        if (ImGui::BeginPopup("RightClickMenu##ProjectPanel"))
        {
            // Menu items for the right-click menu
            if (ImGui::MenuItem("Clear Project Files##ProjectPanel"))
            {
                videosInProject.clear();
            }
            // Close the popup
            ImGui::EndPopup();
        }

        ImGui::SetCursorScreenPos(ImVec2(buttonPos.x, buttonPos.y));
        auto regionAvail = ImGui::GetContentRegionAvail();
        float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
        int deletionTrack = 0;
        int deletionMark = -1;
        for (const auto& entry : videosInProject)
        {
            std::string fileName = entry.filename().string();
            std::string filePath = entry.string();

            if (ImGui::Button(("-##ProjectList" + filePath).c_str(), {lineHeight, lineHeight}))
            {
                deletionMark = deletionTrack;
            }

            ImGui::SameLine();

            ImGui::Button((fileName + " [" + std::to_string(ExPanel->GetCache(entry).mMaxFrames) + "]").c_str(), {regionAvail.x - lineHeight - ImGui::GetStyle().FramePadding.y * 2.0f, lineHeight});
            if (ImGui::BeginDragDropSource())
            {
                ImGui::SetDragDropPayload("ITEM_NAME", filePath.c_str(), filePath.size() + 1);
                ImGui::EndDragDropSource();
            }

            if (ImGui::IsItemHovered() )  // 0 = left mouse button
            {
                if (ImGui::IsMouseDoubleClicked(0))
                // Double-click logic
                    VpPanel->SetVideo(entry);

                float printedThumbnailSize = (float)128 * ImGuiManager::styleMultiplier;

                ImGui::BeginTooltip();
                ImGui::Image((ImTextureID)ExPanel->GetCache(entry).mTexture->GetTextureID(), { printedThumbnailSize ,printedThumbnailSize });
                ImGui::EndTooltip();
            }
            deletionTrack++;
        }
        if (deletionMark != -1)
        {
            if (std::filesystem::absolute(VpPanel->GetVideoPath()) == std::filesystem::absolute(videosInProject[deletionMark]))
            {
                if (deletionMark == 0)
                {
                    if (videosInProject.size() > 1)
                        VpPanel->SetVideo(videosInProject[1]);

                }
                else
                {
                    VpPanel->SetVideo(videosInProject[deletionMark - 1]);
                }
            }
            videosInProject.erase(videosInProject.begin() + deletionMark);
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
        if (!mProject->IsProjectLoaded())
        {
			return;
		}
        for (auto& entry : std::filesystem::directory_iterator(mProject->GetAssetsDir()))
        {
            AddDirectoryRecursive(entry, mProject->mVideosInProject);
        }
    }
    void ProjectPanel::OnLoad()
    {
        for (auto& entry : std::filesystem::directory_iterator(mProject->GetAssetsDir()))
        {
            AddDirectoryRecursive(entry, mProject->mVideosInProject);
        }
    }
}
