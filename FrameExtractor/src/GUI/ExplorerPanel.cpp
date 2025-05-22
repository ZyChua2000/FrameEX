/******************************************************************************
/*!
\file       ExplorerPanel.cpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 13, 2024
\brief      Defines the Explorer Panel class which creates the interface for a
			exploring files

 /******************************************************************************/

#include <FrameExtractorPCH.hpp>
#include <GUI/ExplorerPanel.hpp>
#include <GUI/ImGuiManager.hpp>
#include <GUI/GuiResourcesManager.hpp>
#include <Core/LoggerManager.hpp>
#include <imgui.h>
#include <Graphics/Video.hpp>
namespace FrameExtractor
{
	ExplorerPanel::ExplorerPanel(Project* project) : mProject(project)
	{


	}
	ExplorerPanel::~ExplorerPanel()
	{
	}
	void ExplorerPanel::OnImGuiRender(float dt)
	{
		ImGui::Begin("Content Browser");
		ImVec2 windowSize = ImGui::GetContentRegionAvail();
		float scrollableHeight = windowSize.y - 100; // Adjust for the fixed bottom row
		ImGui::BeginChild("ScrollableRegion", ImVec2(windowSize.x, scrollableHeight), true);
	
		float panelWidth = ImGui::GetContentRegionAvail().x;
		float cellSize = 128 * ImGuiManager::styleMultiplier + 10 * ImGuiManager::styleMultiplier;
		int columnCount = (int)(panelWidth / cellSize);
		columnCount = columnCount < 1 ? 1 : columnCount;
		float printedThumbnailSize = (float)128 * ImGuiManager::styleMultiplier;


		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1))  // 1 for right-click
		{
			// Open the popup when right-clicked on this button
			ImGui::OpenPopup("RightClickMenu##ExplorerPanel");
		}
		if (ImGui::BeginPopup("RightClickMenu##ExplorerPanel"))
		{
			// Menu items for the right-click menu
			if (ImGui::MenuItem("Return to Root Folder##ExplorerPanel"))
			{
				mCurrentPath = mProject->GetAssetsDir();
			}
			// Close the popup
			ImGui::EndPopup();
		}



		ImGui::Columns(columnCount);
		ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });


		if (mCurrentPath != mProject->GetAssetsDir())
		{
			
			if (ImGui::ImageButton(("##ExplorerSelected" + mCurrentPath.string()).c_str(), (ImTextureID)Resource(Icon::FOLDER_ICON)->GetTextureID(), {printedThumbnailSize, printedThumbnailSize}, {0,0}, {1,1}, {0,0,0,0}, {1,1,1,1}))
			{
				mSelectedPath = mCurrentPath.parent_path();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				mCurrentPath = mCurrentPath.parent_path();
			}
			if (mSelectedPath == mCurrentPath.parent_path())
			{
				ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));
			}
			ImGui::TextWrapped("..");
			ImGui::NextColumn();

			
		}

		if(std::filesystem::exists(mCurrentPath))
		for (auto& entry : std::filesystem::directory_iterator(mCurrentPath))
		{
			const std::filesystem::path& path = entry.path();
			std::string filenameStr = path.filename().string();
			std::string extension = path.extension().string();

			ImGui::PushID(filenameStr.c_str());
			uint64_t screenID = 0;
			if (entry.is_directory())
			{
				screenID = static_cast<uint64_t>(Resource(Icon::FOLDER_ICON)->GetTextureID());
			}
			else if (extension == ".mp4")
			{
				if (!mCache.contains(entry))
				{
					Video video(entry);
					video.Decode(0);
					mCache[entry].mTexture = video.GetFrame();
					mCache[entry].mMaxFrames = video.GetMaxFrames();
				}
				screenID = static_cast<uint64_t>(mCache[entry].mTexture->GetTextureID());
			}
			else
			{
				screenID = static_cast<uint64_t>(Resource(Icon::FILE_ICON)->GetTextureID());
			}

			if (ImGui::ImageButton(("##ExplorerEntry" + filenameStr).c_str(), (ImTextureID)screenID, {printedThumbnailSize, printedThumbnailSize}, {0,0}, {1,1}, {0,0,0,0}, {1,1,1,1}))
			{
				mSelectedPath = entry;
			}

			if (extension == ".mp4")
			{
				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("IMPORTVIDEO", entry.path().string().c_str(), entry.path().string().size() + 1);
					ImGui::EndDragDropSource();
				}
			}

			if (mSelectedPath == entry)
			{
				ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (entry.is_directory())
				{
					mCurrentPath /= path.filename();
				}
			}
			ImGui::TextWrapped(filenameStr.c_str());
			ImGui::NextColumn();

			ImGui::PopID();
		}
		ImGui::PopStyleColor();
		ImGui::Columns(1);

		ImGui::EndChild();
		ImGui::End();
	}

	Cache ExplorerPanel::GetCache(std::filesystem::path path)
	{
		if (!mCache.contains(path))
		{
			Video video(path);
			video.Decode(0);
			mCache[path].mMaxFrames = video.GetMaxFrames();
			mCache[path].mTexture = video.GetFrame();
		}
		return mCache[path];
	}
}
