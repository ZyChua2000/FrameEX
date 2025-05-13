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
#include <Core/LoggerManager.hpp>
#include <imgui.h>
namespace FrameExtractor
{
	ExplorerPanel::ExplorerPanel()
	{
		mCurrentPath = std::filesystem::current_path();
		mRootPath = std::filesystem::current_path();
		mFolderIcon = MakeRef<Texture>("resources/icons/folder.png");
		mFileIcon = MakeRef<Texture>("resources/icons/document.png");

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
		int cellSize = 128 * ImGuiManager::styleMultiplier + 10 * ImGuiManager::styleMultiplier;
		int columnCount = (int)(panelWidth / cellSize);
		columnCount = columnCount < 1 ? 1 : columnCount;
		float printedThumbnailSize = (float)128 * ImGuiManager::styleMultiplier;

		ImGui::Columns(columnCount);
		ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });

		if (mCurrentPath != mCurrentPath.root_directory())
		{
			
			if (ImGui::ImageButton((ImTextureID)mFolderIcon->GetTextureID(), {printedThumbnailSize, printedThumbnailSize}, {0,1}, {1,0}, -1, {0,0,0,0}, {1,1,1,1}))
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

		for (auto& entry : std::filesystem::directory_iterator(mCurrentPath))
		{
			const std::filesystem::path& path = entry.path();
			std::string filenameStr = path.filename().string();
			std::string extension = path.extension().string();

			ImGui::PushID(filenameStr.c_str());
			uint64_t screenID = 0;
			if (entry.is_directory())
			{
				screenID = static_cast<uint64_t>(mFolderIcon->GetTextureID());
			}
			else if (extension == ".mp4")
			{

			}
			else
			{
				screenID = static_cast<uint64_t>(mFileIcon->GetTextureID());
			}

			if (ImGui::ImageButton((ImTextureID)screenID, { printedThumbnailSize, printedThumbnailSize }, { 0,1 }, { 1,0 }, -1, { 0,0,0,0 }, { 1,1,1,1 }))
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
}
