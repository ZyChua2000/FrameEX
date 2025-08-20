/******************************************************************************/
/*!
\file       ExplorerPanel.cpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 13, 2025
\brief      Defines the Explorer Panel class which creates the interface for a
			exploring files

 ******************************************************************************/

#include <FrameExtractorPCH.hpp>

 // Third-party includes
#include <imgui.h>
#include <imgui_toggle.h>

 // Project includes
#include <GUI/ExplorerPanel.hpp>
#include <GUI/ImGuiManager.hpp>
#include <GUI/GuiResourcesManager.hpp>
#include <Core/LoggerManager.hpp>
#include <Core/AssetManager.hpp>
#include <Graphics/Video.hpp>

namespace FrameExtractor
{
	ExplorerPanel::ExplorerPanel(Project* project) : mProject(project)
	{}
	ExplorerPanel::~ExplorerPanel()
	{}
	void ExplorerPanel::OnImGuiRender(float dt)
	{
		ImGui::Begin("Content Browser");

		ImGui::Toggle("##IsUsingDirectory", &mIsUsingDirectory, ImGuiToggleFlags_Animated);
		ImGui::SameLine();
		if (mIsUsingDirectory)
		{
			ImGui::Text("Mode: Directory");
		}
		else
		{
			ImGui::Text("Mode: Assets");
		}
		ImGui::Separator();

		ImVec2 windowSize = ImGui::GetContentRegionAvail();
		ImGui::BeginChild("ScrollableRegion", ImVec2(windowSize.x, windowSize.y), true);

		if (mIsUsingDirectory)
		{
			//DrawDirectory();
		}
		else
		{
			DrawAssets();
		}

		ImGui::EndChild();
		ImGui::End();
	}

	ExplorerPanel::Cache ExplorerPanel::GetCache(std::filesystem::path path)
	{
		if (!mCache.contains(path))
		{
			Video video(path);
			video.Load();
			if (video.IsLoaded())
			{
				video.Decode(0);
				mCache[path].mMaxFrames = video.GetMaxFrames();
				mCache[path].mTexture = video.GetFrame();
				video.Unload();
				return mCache[path];
			}
			video.Load();
			video.Unload();
			return { nullptr, 0 };
		}
		else
		{
			return mCache[path];
		}
	}
	void ExplorerPanel::DrawDirectory()
	{
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
			if (ImGui::ImageButton(("##ExplorerSelected" + mCurrentPath.string()).c_str(), (ImTextureID)Resource(Icon::FOLDER_ICON)->GetTextureID(), { printedThumbnailSize, printedThumbnailSize }, { 0,0 }, { 1,1 }, { 0,0,0,0 }, { 1,1,1,1 }))
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

		if (std::filesystem::exists(mCurrentPath))
		{
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
					auto Cache = GetCache(entry);
					if (Cache.mTexture)
					{
						screenID = Cache.mTexture->GetTextureID();
					}
					else
					{
						screenID = static_cast<uint64_t>(Resource(Icon::FILE_ICON)->GetTextureID());
					}
				}
				else
				{
					screenID = static_cast<uint64_t>(Resource(Icon::FILE_ICON)->GetTextureID());
				}

				if (ImGui::ImageButton(("##ExplorerEntry" + filenameStr).c_str(), (ImTextureID)screenID, { printedThumbnailSize, printedThumbnailSize }, { 0,0 }, { 1,1 }, { 0,0,0,0 }, { 1,1,1,1 }))
				{
					mSelectedPath = entry;
				}

				if (extension == ".mp4")
				{
					if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("IMPORT_VIDEO_FILENAME", entry.path().string().c_str(), entry.path().string().size() + 1);
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
		}
		ImGui::PopStyleColor();
		ImGui::Columns(1);
	}
	void ExplorerPanel::DrawAssets()
	{
		float panelWidth = ImGui::GetContentRegionAvail().x;
		float cellSize = 128 * ImGuiManager::styleMultiplier + 10 * ImGuiManager::styleMultiplier;
		int columnCount = (int)(panelWidth / cellSize);
		columnCount = columnCount < 1 ? 1 : columnCount;
		float printedThumbnailSize = (float)128 * ImGuiManager::styleMultiplier;

		ImGui::Columns(columnCount);
		ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });

		for (const auto& [handle, metaData] : AssetManager::GetMetaDatas())
		{
			const std::filesystem::path& path = metaData.mPath;
			std::string filenameStr = path.filename().string();
			std::string extension = path.extension().string();

			ImGui::PushID(filenameStr.c_str());
			uint64_t screenID = 0;

			if (metaData.mAssetType == AssetType::Video)
			{
				auto Cache = GetCache(path);
				if (Cache.mTexture)
				{
					screenID = Cache.mTexture->GetTextureID();
				}
				else
				{
					screenID = static_cast<uint64_t>(Resource(Icon::FILE_ICON)->GetTextureID());
				}
			}
			else if (metaData.mAssetType == AssetType::Texture)
			{
				screenID = AssetManager::GetAsset<Texture>(handle)->GetTextureID();
			}

			if (ImGui::ImageButton(("##ExplorerEntry" + filenameStr).c_str(), (ImTextureID)screenID, { printedThumbnailSize, printedThumbnailSize }, { 0,0 }, { 1,1 }, { 0,0,0,0 }, { 1,1,1,1 }))
			{
				mSelectedPath = path;
			}

			if (metaData.mAssetType == AssetType::Video)
			{
				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("IMPORT_VIDEO_HANDLE", &handle, sizeof(handle));
					ImGui::EndDragDropSource();
				}
			}

			if (mSelectedPath == path)
			{
				ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));
			}

			ImGui::TextWrapped(filenameStr.c_str());
			ImGui::NextColumn();

			ImGui::PopID();
		}
		ImGui::PopStyleColor();
		ImGui::Columns(1);
	}

}