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
#include <Core/Command.hpp>
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
        if (CommandHistory::isDirty())
        {
            ImGui::Begin("Project*###ProjectWindow");
        }
        else
        {
            ImGui::Begin("Project###ProjectWindow");
        }

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

                videosInProject.insert(droppedItem);
                
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
                VpPanel->ClearVideo();
            }
            // Close the popup
            ImGui::EndPopup();
        }

        ImGui::SetCursorScreenPos(ImVec2(buttonPos.x, buttonPos.y));
        auto regionAvail = ImGui::GetContentRegionAvail();
        float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
        int deletionTrack = 0;
        int distance = 0;
        std::filesystem::path deletionMark{};
        for (const auto& entry : videosInProject)
        {
            std::string fileName = entry.filename().string();
            std::string filePath = entry.string();

            if (ImGui::Button(("-##ProjectList" + filePath).c_str(), {lineHeight, lineHeight}))
            {
                deletionMark = entry;
                distance = deletionTrack;
            }

            ImGui::SameLine();
            auto vpPath = VpPanel->GetVideoPath();
            if (VpPanel->GetVideoPath() == entry)
                ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Header));
            else
                ImGui::PushStyleColor(ImGuiCol_Button, {});
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_FrameBgHovered));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_FrameBgActive));
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, { 0,0, });
            ImGui::Button((fileName + " [" + std::to_string(ExPanel->GetCache(entry).mMaxFrames) + "]").c_str(), {regionAvail.x - lineHeight - ImGui::GetStyle().FramePadding.y * 2.0f, lineHeight});
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(3);

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
            if (ImGui::BeginDragDropSource())
            {
                ImGui::SetDragDropPayload("ITEM_NAME", filePath.c_str(), filePath.size() + 1);
                ImGui::EndDragDropSource();
            }
            deletionTrack++;
        }
        if (!deletionMark.empty())
        {
            if (std::filesystem::absolute(VpPanel->GetVideoPath()) == std::filesystem::absolute(deletionMark))
            {
                if (distance == 0)
                {
                    if (videosInProject.size() > 1)
                        VpPanel->SetVideo(*(++videosInProject.begin()));

                }
                else
                {
                    auto it = videosInProject.begin();
                    std::advance(it, distance - 1);
                    VpPanel->SetVideo(*it);
                }
            }
            videosInProject.erase(deletionMark);
            if (videosInProject.empty())
            {
                VpPanel->ClearVideo();
            }
        }
        ImGui::EndChild();
        ImGui::End();
    }

    const char* ProjectPanel::GetName() const
    {
        return "Project";
    }
    
    static void AddDirectoryRecursive(const std::filesystem::directory_entry& currEntry, std::unordered_set<std::filesystem::path>& videosInProject)
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
            videosInProject.insert(currEntry.path());
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
