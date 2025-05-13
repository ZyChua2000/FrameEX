/******************************************************************************
/*!
\file       ViewportPanel.cpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 10, 2024
\brief      Defines the Viewport Panel class.

 /******************************************************************************/
#include "FrameExtractorPCH.hpp"
#include <Core/LoggerManager.hpp>
#include "GUI/ViewportPanel.hpp"
#include <GUI/ImGuiManager.hpp>
#include <Graphics/Video.hpp>
namespace FrameExtractor
{
    ViewportPanel::ViewportPanel(const std::string& name, ImVec2& size, ImVec2& pos) : 
        mName(name),
		mViewportSize(size),
		mViewportPos(pos)
    {
        mVideo = new Video("sampleVideo.mp4");
        mVideo->Decode();
        mIcons[Icons::PLAY_ICON] = MakeRef<Texture>("resources/icons/Play.png");
        mIcons[Icons::STOP_ICON] = MakeRef<Texture>("resources/icons/Stop.png");
        mIcons[Icons::FORWARD_ICON] = MakeRef<Texture>("resources/icons/MoveFrameRight.png");
        mIcons[Icons::BACKWARD_ICON] = MakeRef<Texture>("resources/icons/MoveFrameLeft.png");
        mIcons[Icons::SPEED_UP_ICON] = MakeRef<Texture>("resources/icons/FastForward.png");
        mIcons[Icons::SLOW_DOWN_ICON] = MakeRef<Texture>("resources/icons/FastBackward.png");
        mIcons[Icons::SKIP_TO_END_ICON] = MakeRef<Texture>("resources/icons/CutToEnd.png");
        mIcons[Icons::SKIP_TO_START_ICON] = MakeRef<Texture>("resources/icons/CutToFront.png");


    }

    ViewportPanel::~ViewportPanel()
    {
        if (mVideo) delete mVideo;
    }


    void ViewportPanel::OnImGuiRender(float dt)
    {
        static float DTTrack = 0.0f;
		//ImGui::SetNextWindowSize(mViewportSize);
		//ImGui::SetNextWindowPos(mViewportPos);

		ImGui::Begin(mName.c_str());

        // Get window size


		// Render the viewport here

       
        auto contentRegion = ImGui::GetContentRegionAvail();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (contentRegion.x - contentRegion.x * 0.85f) * 0.5f);
        ImGui::Image((ImTextureID)mVideo->GetFrame()->GetTextureID(), ImVec2(contentRegion.x * 0.85f, contentRegion.y));
       
        //DTTrack += dt;
        //mFrameNumber = (int)(DTTrack * mVideo->GetFPS());

		ImGui::End();

        ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoDecoration);
        
        ImGui::PushFont(ImGuiManager::BoldFont);
        ImGui::SetNextItemWidth(ImGui::CalcTextSize(std::to_string(mFrameNumber).c_str()).x);
        //ImGui::PushStyleVar(ImGuiStyleVar_)
        ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0.f, 0.f, 0.f, 0.f });
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, { 0.f, 0.f, 0.f, 0.f });
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, { 0.f, 0.f, 0.f, 0.f });
        if (ImGui::DragInt("##FrameNumberDisplay", &mFrameNumber, 1.0f, 0, mVideo->GetMaxFrames(), ""))
        {
            mVideo->Decode(mFrameNumber);
            mIsPlaying = false;
        }
        ImVec2 itemPos = ImGui::GetItemRectMin(); // Bottom-left position of the item
        ImVec2 itemSize = ImGui::GetItemRectSize(); // Size of the item

        // Calculate the position to draw the text (adjust the offset as needed)
        ImVec2 textPos = ImVec2(itemPos.x, itemPos.y);

        // Draw custom text on top of the widget using ImDrawList
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        if (ImGui::IsItemHovered())
        {
            // Change text color on hover
            drawList->AddText(textPos, IM_COL32(82, 168, 255, 255), std::to_string(mFrameNumber).c_str());
        }
        else
        {
            drawList->AddText(textPos, IM_COL32(45, 139, 235, 255), std::to_string(mFrameNumber).c_str());
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();

        ImGui::SetNextItemWidth(ImGui::CalcTextSize(std::to_string(mVideo->GetMaxFrames()).c_str()).x);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(std::to_string(mVideo->GetMaxFrames()).c_str()).x));
        ImGui::Text("%d", mVideo->GetMaxFrames());
        ImGui::PopFont();
        ImGui::Spacing();

        contentRegion = ImGui::GetContentRegionAvail();

        ImGui::SetNextItemWidth(contentRegion.x);
        if (ImGui::SliderInt("##FrameNumber", &mFrameNumber, 0, mVideo->GetMaxFrames(), ""))
        {
            mVideo->Decode(mFrameNumber);
            mIsPlaying = false;
        }
        else
        {
            if (mIsPlaying)
            {
                if (mFrameNumber < mVideo->GetMaxFrames())
                {
                    mVideo->Decode(mFrameNumber);
                    DTTrack += dt * mSpeedMultiplier;
                    mFrameNumber = (int)(DTTrack * mVideo->GetFPS());
                }
            }
        }

        if (ImGui::IsItemDeactivatedAfterEdit()) {
            mIsPlaying = true;
            DTTrack = (float)mFrameNumber / mVideo->GetFPS();
        }

        ImGui::Spacing();

        const int buttonCount = 7;
        const float buttonSize = 36.f * ImGuiManager::styleMultiplier;  // Width and height of each button
        const float spacing = ImGui::GetStyle().ItemSpacing.x;
        float totalWidth = buttonCount * buttonSize + (buttonCount - 1) * spacing;
        float startX = (ImGui::GetContentRegionAvail().x - totalWidth) * 0.5f;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + startX);
        if (ImGui::ImageButton((ImTextureID)mIcons[SKIP_TO_START_ICON]->GetTextureID(), { buttonSize, buttonSize }))
        {
            mFrameNumber = 0;
            mIsPlaying = false;
            mVideo->Decode(mFrameNumber);
        }

        ImGui::SameLine();

        if (ImGui::ImageButton((ImTextureID)mIcons[SLOW_DOWN_ICON]->GetTextureID(), { buttonSize,buttonSize }))
        {
            SlowDown();
        }

        ImGui::SameLine();

        if (ImGui::ImageButton((ImTextureID)mIcons[BACKWARD_ICON]->GetTextureID(), { buttonSize,buttonSize }))
        {
            mFrameNumber -= 1;
            if (mFrameNumber < 0)
            {
                mFrameNumber = 0;
            }
            mVideo->Decode(mFrameNumber);
            mIsPlaying = false;
        }

        ImGui::SameLine();

        if (!mIsPlaying)
        {
            if (ImGui::ImageButton((ImTextureID)mIcons[PLAY_ICON]->GetTextureID(), { buttonSize,buttonSize }))
            {
                mIsPlaying = true;
                DTTrack = (float)mFrameNumber / mVideo->GetFPS();
            }
        }
        else
        {
            if (ImGui::ImageButton((ImTextureID)mIcons[STOP_ICON]->GetTextureID(), { buttonSize,buttonSize }))
            {
                mIsPlaying = false;
            }
        }

        ImGui::SameLine();

        if (ImGui::ImageButton((ImTextureID)mIcons[FORWARD_ICON]->GetTextureID(), { buttonSize,buttonSize }))
        {
            mFrameNumber += 1;
            if (mFrameNumber > mVideo->GetMaxFrames())
            {
                mFrameNumber = mVideo->GetMaxFrames();
            }
            mVideo->Decode(mFrameNumber);
            mIsPlaying = false;
        }

        ImGui::SameLine();

        if (ImGui::ImageButton((ImTextureID)mIcons[SPEED_UP_ICON]->GetTextureID(), { buttonSize,buttonSize }))
        {
            SpeedUp();
        }

        ImGui::SameLine();

        if (ImGui::ImageButton((ImTextureID)mIcons[SKIP_TO_END_ICON]->GetTextureID(), { buttonSize,buttonSize }))
        {
            mFrameNumber = mVideo->GetMaxFrames();
            mIsPlaying = false;
            mVideo->Decode(mFrameNumber - 1);
        }
        ImGui::End();        
    }

    const char* ViewportPanel::GetName() const
    {
        return mName.c_str();
    }
    void ViewportPanel::SpeedUp()
    {
        if (mSpeedMultiplier == 8) return;
        if (++mSpeedMultiplierIndex == 0) { mSpeedMultiplierIndex = 1; }
        switch (mSpeedMultiplierIndex)
        {
        case 1:
            mSpeedMultiplier = 1.0f;
            return;
        case 2:
            mSpeedMultiplier = 1.25f;
            return;
        case 3:
            mSpeedMultiplier = 1.5f;
            return;
        case 4:
            mSpeedMultiplier = 2.0f;
            return;
        case 5:
            mSpeedMultiplier = 5.f;
			return;
        case 6:
            mSpeedMultiplier = 10.0f;
			return;
        case 7:
            mSpeedMultiplier = 20.0f;
            return;
        case 8:
            mSpeedMultiplier = 50.0f;
			return;
        case -1:
            mSpeedMultiplier = -1.0f;
            return;
        case -2:
            mSpeedMultiplier = -1.25f;
            return;
        case -3:
            mSpeedMultiplier = -1.5f;
            return;
        case -4:
            mSpeedMultiplier = -2.0f;
            return;
        case -5:
            mSpeedMultiplier = -5.f;
            return;
        case -6:
            mSpeedMultiplier = -10.0f;
            return;
        case -7:
            mSpeedMultiplier = -20.0f;
            return;
        case -8:
            mSpeedMultiplier = -50.0f;
            return;
        default:
            return;
        }
        
    }
    void ViewportPanel::SlowDown()
    {
        if (mSpeedMultiplier == -8) return;
        if (--mSpeedMultiplierIndex == 0) { mSpeedMultiplierIndex = -1; }
        switch (mSpeedMultiplierIndex)
        {
        case 1:
            mSpeedMultiplier = 1.0f;
            return;
        case 2:
            mSpeedMultiplier = 1.25f;
            return;
        case 3:
            mSpeedMultiplier = 1.5f;
            return;
        case 4:
            mSpeedMultiplier = 2.0f;
            return;
        case 5:
            mSpeedMultiplier = 5.f;
            return;
        case 6:
            mSpeedMultiplier = 10.0f;
            return;
        case 7:
            mSpeedMultiplier = 20.0f;
            return;
        case 8:
            mSpeedMultiplier = 50.0f;
            return;
        case -1:
            mSpeedMultiplier = -1.0f;
            return;
        case -2:
            mSpeedMultiplier = -1.25f;
            return;
        case -3:
            mSpeedMultiplier = -1.5f;
            return;
        case -4:
            mSpeedMultiplier = -2.0f;
            return;
        case -5:
            mSpeedMultiplier = -5.f;
            return;
        case -6:
            mSpeedMultiplier = -10.0f;
            return;
        case -7:
            mSpeedMultiplier = -20.0f;
            return;
        case -8:
            mSpeedMultiplier = -50.0f;
            return;
        default:
            return;
        }
    }
}
