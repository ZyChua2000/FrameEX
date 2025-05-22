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
#include <Core/Command.hpp>
#include "GUI/ViewportPanel.hpp"
#include <GUI/ImGuiManager.hpp>
#include <GUI/GuiResourcesManager.hpp>
#include <Graphics/Video.hpp>
namespace FrameExtractor
{

    long long timer = 0;
    ViewportPanel::ViewportPanel(const std::string& name, ImVec2& size, ImVec2& pos) : 
        mName(name),
		mViewportSize(size),
		mViewportPos(pos)
    {



    }

    ViewportPanel::~ViewportPanel()
    {
        if (mVideo) delete mVideo;
    }


    void ViewportPanel::OnImGuiRender(float dt)
    {
		//ImGui::SetNextWindowSize(mViewportSize);
		//ImGui::SetNextWindowPos(mViewportPos);

        if (mVideo)
            ImGui::Begin((mVideo->GetPath().filename().string() + "###ViewportID").c_str());
        else
            ImGui::Begin("Viewport Window###ViewportID");

        ImGui::DockSpace(ImGui::GetID("ViewportDockspace"));
        bool isWindowFocused = 0| ImGui::IsWindowFocused();
        ImGui::End();
        // Get window size

        ImGui::Begin(mName.c_str());
        isWindowFocused |= ImGui::IsWindowFocused();
		// Render the viewport here
        float lineHeight = (ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f);
        auto contentRegion = ImGui::GetContentRegionAvail();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (contentRegion.x - contentRegion.x * 0.85f) * 0.5f);
        auto cursor = ImGui::GetCursorScreenPos();
        auto regionX = contentRegion.x * 0.85f;
        auto regionY = contentRegion.y;
        if (mVideo)
        {
            ImGui::Image((ImTextureID)mVideo->GetFrame()->GetTextureID(), ImVec2(contentRegion.x * 0.85f, contentRegion.y - lineHeight * 2.5f));
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ITEM_NAME")) {
                    const char* droppedItem = static_cast<const char*>(payload->Data);
                    SetVideo({ droppedItem });
                }
            }
        }
        else
        {
            
            ImGui::Image(0, ImVec2(contentRegion.x * 0.85f, contentRegion.y - lineHeight * 2.5f));
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ITEM_NAME")) {
                    const char* droppedItem = static_cast<const char*>(payload->Data);
                    SetVideo({ droppedItem });
                }
            }
        }

        ImDrawList* drawList = ImGui::GetWindowDrawList();
      
        //DTTrack += dt;
        //mFrameNumber = (int)(DTTrack * mVideo->GetFPS());
        
        ImGui::PushFont(ImGuiManager::BoldFont);
        //ImGui::PushStyleVar(ImGuiStyleVar_)
        ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0.f, 0.f, 0.f, 0.f });
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, { 0.f, 0.f, 0.f, 0.f });
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, { 0.f, 0.f, 0.f, 0.f });
        if (mVideo)
        {

           

            ImVec2 widgetSize(ImGui::CalcTextSize(std::to_string(mFrameNumber).c_str()).x, lineHeight); // Size of the widget
            ImVec2 p = ImGui::GetCursorScreenPos();            
            ImGui::SetNextItemAllowOverlap();
            // We make a "dummy" item that doesn't interact but allows hover detection
            ImGui::InvisibleButton("##frameNumber_hover_check", widgetSize); // Invisible button (no interaction)

            // Check if the mouse is hovering over the dummy area
            bool isHovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

            if (isHovered)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(82, 168, 255, 255)); // Change text color on hover
            }
            ImGui::SetCursorScreenPos(p);

            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, { 0.f,0.f });
            ImGui::SetNextItemWidth(ImGui::CalcTextSize("XXXXXX").x);
            if (ImGui::DragInt("##FrameNumberDisplay", &mFrameNumber, 1.0f, 0, mVideo->GetMaxFrames()-1, "%d"))
            {
                mVideo->Decode((uint32_t)mFrameNumber);
                mIsPlaying = false;
            }
            ImGui::PopStyleVar();

            if (isHovered)
            {
                ImGui::PopStyleColor();
            }
            
        }
        else
        {
            int buffertmp = 0;
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, { 0.f,0.f });
            ImGui::DragInt("##FrameNumberDisplay", &buffertmp, 1.0f, 0, 0, "%d", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoInput);
            ImGui::PopStyleVar();
        }

        ImGui::PopStyleColor(3);

        ImGui::SameLine();


       
        if (mVideo)
        {
            ImGui::SetNextItemWidth(ImGui::CalcTextSize(std::to_string(mVideo->GetMaxFrames()).c_str() - 1).x);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(std::to_string(mVideo->GetMaxFrames() - 1).c_str()).x));
            ImGui::Text("%d", mVideo->GetMaxFrames() - 1);
        }
        else
        {
            ImGui::SetNextItemWidth(ImGui::CalcTextSize("-").x);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("-").x));
            ImGui::Text("-");
        }

        {
            if (mVideo)
            {
                int totalSeconds = mFrameNumber / mVideo->GetFPS();
                int frames = mFrameNumber % mVideo->GetFPS();
                int seconds = totalSeconds % 60;
                int minutes = (totalSeconds / 60) % 60;
                int hours = totalSeconds / 3600;

                std::ostringstream oss;
                oss << std::setfill('0')
                    << std::setw(2) << hours << ":"
                    << std::setw(2) << minutes << ":"
                    << std::setw(2) << seconds << ":"
                    << std::setw(2) << frames;

                ImGui::Text(oss.str().c_str());
            }
            else
            {
				ImGui::Text("-");
			}
        }
        ImGui::SameLine();

        {
            if(mVideo)
            {
                int totalSeconds = (mVideo->GetMaxFrames() - 1) / mVideo->GetFPS();
                int frames = (mVideo->GetMaxFrames() - 1) % mVideo->GetFPS();
                int seconds = totalSeconds % 60;
                int minutes = (totalSeconds / 60) % 60;
                int hours = totalSeconds / 3600;

                std::ostringstream oss;
                oss << std::setfill('0')
                    << std::setw(2) << hours << ":"
                    << std::setw(2) << minutes << ":"
                    << std::setw(2) << seconds << ":"
                    << std::setw(2) << frames;

                ImGui::SetNextItemWidth(ImGui::CalcTextSize(oss.str().c_str()).x);
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(oss.str().c_str()).x));

                ImGui::Text(oss.str().c_str());
            }
            else
            {
                ImGui::SetNextItemWidth(ImGui::CalcTextSize("-").x);
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("-").x));
                ImGui::Text("-");
            }

        }


        ImGui::PopFont();
        ImGui::End();

        ImGui::Begin("Viewport Controls");
        isWindowFocused |= ImGui::IsWindowFocused();
        contentRegion = ImGui::GetContentRegionAvail();
        ImGui::SetNextItemWidth(contentRegion.x);
        if(mVideo)
        {
            if (ImGui::SliderInt("##FrameNumber", &mFrameNumber, 0, mVideo->GetMaxFrames()-1, ""))
            {
                if (!initialIn)
                {
                    wasPlaying = mIsPlaying;
                }
                initialIn = true;
                mVideo->Decode((uint32_t)mFrameNumber);
                mIsPlaying = false;
            }
            else
            {
                if (mIsPlaying)
                {
                    // measure time here
                    auto currentTime = std::chrono::high_resolution_clock::now();
                    if ((uint32_t)mFrameNumber < mVideo->GetMaxFrames())
                    {
                        if (mSpeedMultiplier < 50 && mSpeedMultiplier > 0)
                            mVideo->DecodeTime(dt, mSpeedMultiplier);
                        else
                            mVideo->Decode(mFrameNumber);
                        DTTrack += dt * mSpeedMultiplier;
                        mFrameNumber = (int)(DTTrack * mVideo->GetFPS());
                        if ((uint32_t)mFrameNumber >= mVideo->GetMaxFrames())
                        {
                            mFrameNumber = mVideo->GetMaxFrames() - 1;
                        }
                        else if (mFrameNumber < 0)
                        {
                            mFrameNumber = 0;
                        }
                    }
                    auto endTime = std::chrono::high_resolution_clock::now();
                    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - currentTime).count();
                    timer += elapsedTime;
                }
              
            }
        }
        else
        {
            int bufferTmp = 0;
            ImGui::SliderInt("##FrameNumber", &bufferTmp, 0, 0, "", ImGuiSliderFlags_NoInput | ImGuiSliderFlags_AlwaysClamp);
        }
        // Get ImGui's draw list for custom rendering
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 slider_pos = ImGui::GetItemRectMin();
        ImVec2 slider_size = ImGui::GetItemRectSize();
        float slider_width = slider_size.x;

        // Draw ticks
        for (int i = 0; i <= 60; ++i)
        {
            float t = (float)i /60;
            float x = slider_pos.x + t * slider_width;
            float y1 = slider_pos.y + slider_size.y;
            float y2 = y1 - 14.0f;  // Tick length

            draw_list->AddLine(ImVec2(x, y1), ImVec2(x, y2), IM_COL32(200, 200, 200, 255));
        }

        if (mVideo)
        {
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                initialIn = false;
                mIsPlaying = wasPlaying;
                DTTrack = (float)mFrameNumber / mVideo->GetFPS();
            }
        }

        ImGui::Spacing();
        const int buttonCount = 8;
        const float buttonSize = 36.f * ImGuiManager::styleMultiplier;  // Width and height of each button
        const float spacing = ImGui::GetStyle().ItemSpacing.x;
        float totalWidth = buttonCount * buttonSize + (buttonCount - 1) * spacing + ImGui::GetStyle().FramePadding.x;
        float startX = (ImGui::GetContentRegionAvail().x - totalWidth) * 0.5f;
        ImGuiIO& io = ImGui::GetIO();
       
        auto ctrlHeld = ImGui::IsKeyDown(ImGuiKey_RightCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftCtrl);
        auto shiftHeld = ImGui::IsKeyDown(ImGuiKey_RightShift) || ImGui::IsKeyDown(ImGuiKey_LeftShift);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + startX);
        if (ImGui::ImageButton("#IconSkipToStart", (ImTextureID)Resource(SKIP_TO_START_ICON)->GetTextureID(), {buttonSize, buttonSize}) ||
            (isWindowFocused && ImGui::IsKeyPressed(ImGuiKey_Home)))
        {
            if (mVideo)
            {
                CommandHistory::execute(std::make_unique<SetVideoFrameCommand>(&mFrameNumber, mFrameNumber, 0, mVideo));
                mIsPlaying = false;
            }
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
        {
            ImGui::BeginTooltip();
            ImGui::Image((ImTextureID)Resource(HOME_ICON)->GetTextureID(), { lineHeight ,lineHeight });
            ImGui::EndTooltip();

        }

        ImGui::SameLine();
      
        if (ImGui::ImageButton("#IconSlowDown", (ImTextureID)Resource(SLOW_DOWN_ICON)->GetTextureID(), { buttonSize,buttonSize }) ||
            (isWindowFocused && ctrlHeld && ImGui::IsKeyPressed(ImGuiKey_DownArrow)))
        {
            if (mVideo)
                CommandHistory::execute(std::make_unique<CallFunctionCommand>(std::bind(&ViewportPanel::SlowDown, this), std::bind(&ViewportPanel::SpeedUp, this)));
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
        {
            ImGui::BeginTooltip();
            ImGui::Image((ImTextureID)Resource(CTRL_ICON)->GetTextureID(), { lineHeight ,lineHeight });
            ImGui::SameLine();
            ImGui::Image((ImTextureID)Resource(PLUS_LOGO)->GetTextureID(), { lineHeight / 1.5f ,lineHeight / 1.5f });
            ImGui::SameLine();
            ImGui::Image((ImTextureID)Resource(DOWNARROW_ICON)->GetTextureID(), { lineHeight ,lineHeight });
            ImGui::EndTooltip();
        }

        ImGui::SameLine();

        if (ImGui::ImageButton("#IconBackward", (ImTextureID)Resource(BACKWARD_ICON)->GetTextureID(), { buttonSize,buttonSize }) ||
            (isWindowFocused && !shiftHeld && ImGui::IsKeyPressed(ImGuiKey_LeftArrow)))
        {
            if(mVideo)
            {
                float buffer = (float)mFrameNumber;
                if (ctrlHeld)
                {
                    buffer -= 5 * std::clamp(std::fabsf(mSpeedMultiplier), 1.f, 100.f);
                }
                else
                {
                    buffer -= 1;
                }
                CommandHistory::execute(std::make_unique<SetVideoFrameCommand>(&mFrameNumber, mFrameNumber, buffer, mVideo));
                mIsPlaying = false;
            }
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
        {
            ImGui::BeginTooltip();
            ImGui::Image((ImTextureID)Resource(LEFTARROW_ICON)->GetTextureID(), { lineHeight ,lineHeight });
            ImGui::Image((ImTextureID)Resource(CTRL_ICON)->GetTextureID(), { lineHeight ,lineHeight });
            ImGui::SameLine();
            ImGui::Image((ImTextureID)Resource(PLUS_LOGO)->GetTextureID(), { lineHeight / 1.5f ,lineHeight / 1.5f });
            ImGui::SameLine();
            ImGui::Image((ImTextureID)Resource(LEFTARROW_ICON)->GetTextureID(), { lineHeight ,lineHeight });
            ImGui::EndTooltip();
        }

        ImGui::SameLine();

        if (!mIsPlaying)
        {
            if (ImGui::ImageButton("#IconPlay", (ImTextureID)Resource(PLAY_ICON)->GetTextureID(), { buttonSize,buttonSize })
                || (isWindowFocused && ImGui::IsKeyPressed(ImGuiKey_Space)))
            {
                if(mVideo)
                {
                    CommandHistory::execute(std::make_unique<PlayCommand>(&mIsPlaying, &mFrameNumber, mFrameNumber, mVideo));
                    DTTrack = (float)mFrameNumber / mVideo->GetFPS();
                }
            }
        }
        else
        {
            if (ImGui::ImageButton("#IconStop", (ImTextureID)Resource((STOP_ICON))->GetTextureID(), { buttonSize,buttonSize })
                || (isWindowFocused && ImGui::IsKeyPressed(ImGuiKey_Space)))
            {
                if(mVideo)
                {
                    CommandHistory::execute(std::make_unique<ModifyPropertyCommand<bool>>(&mIsPlaying, true, false));
                }
            }
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
        {
            ImGui::BeginTooltip();
            ImGui::Image((ImTextureID)Resource(SPACE_ICON)->GetTextureID(), { lineHeight ,lineHeight });
            ImGui::EndTooltip();
        }

        ImGui::SameLine();

        if (ImGui::ImageButton("#IconForward", (ImTextureID)Resource((FORWARD_ICON))->GetTextureID(), { buttonSize,buttonSize }) ||
            (isWindowFocused && !shiftHeld && ImGui::IsKeyPressed(ImGuiKey_RightArrow)))
        {
            if(mVideo)
            {
                float buffer = (float)mFrameNumber;
                if (ctrlHeld) 
                { 
                    buffer += 5 * std::clamp(std::fabsf(mSpeedMultiplier), 1.f, 100.f);
                }
                else
                {
                    buffer += 1;
                }

                if (buffer > mVideo->GetMaxFrames())
                {
                    uint32_t(buffer) = mVideo->GetMaxFrames();
                }
                CommandHistory::execute(std::make_unique<SetVideoFrameCommand>(&mFrameNumber, mFrameNumber, buffer, mVideo));
                mIsPlaying = false;
            }
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
        {
            ImGui::BeginTooltip();
            ImGui::Image((ImTextureID)Resource(RIGHTARROW_ICON)->GetTextureID(), { lineHeight ,lineHeight });
            ImGui::Image((ImTextureID)Resource(CTRL_ICON)->GetTextureID(), { lineHeight ,lineHeight });
            ImGui::SameLine();
            ImGui::Image((ImTextureID)Resource(PLUS_LOGO)->GetTextureID(), { lineHeight / 1.5f ,lineHeight / 1.5f });
            ImGui::SameLine();
            ImGui::Image((ImTextureID)Resource(RIGHTARROW_ICON)->GetTextureID(), { lineHeight ,lineHeight });
            ImGui::EndTooltip();
        }

        ImGui::SameLine();

        if (ImGui::ImageButton("##IconSpeedUp", (ImTextureID)Resource(SPEED_UP_ICON)->GetTextureID(), { buttonSize,buttonSize }) ||
            (isWindowFocused && ctrlHeld && ImGui::IsKeyPressed(ImGuiKey_UpArrow)))
        {
            if(mVideo)
            {
                CommandHistory::execute(std::make_unique<CallFunctionCommand>(std::bind(&ViewportPanel::SpeedUp, this), std::bind(&ViewportPanel::SlowDown, this)));
            }
        }

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
        {
            ImGui::BeginTooltip();
            ImGui::Image((ImTextureID)Resource(CTRL_ICON)->GetTextureID(), { lineHeight ,lineHeight });
            ImGui::SameLine();
            ImGui::Image((ImTextureID)Resource(PLUS_LOGO)->GetTextureID(), { lineHeight / 1.5f ,lineHeight / 1.5f });
            ImGui::SameLine();
            ImGui::Image((ImTextureID)Resource(UPARROW_ICON)->GetTextureID(), { lineHeight ,lineHeight });
            ImGui::EndTooltip();
        }

        ImGui::SameLine();

        if (ImGui::ImageButton("#IconSkipToEnd", (ImTextureID)Resource(SKIP_TO_END_ICON)->GetTextureID(), { buttonSize,buttonSize }) ||
            (isWindowFocused && ImGui::IsKeyPressed(ImGuiKey_End)))
        {
            if(mVideo)
            {
                mIsPlaying = false;
                CommandHistory::execute(std::make_unique<SetVideoFrameCommand>(&mFrameNumber, mFrameNumber, mVideo->GetMaxFrames() - 1, mVideo));
            }
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
        {
            ImGui::BeginTooltip();
            ImGui::Image((ImTextureID)Resource(END_ICON)->GetTextureID(), { lineHeight ,lineHeight });
            ImGui::EndTooltip();
        }

        std::string statusText;


        if (mSpeedMultiplier > 0)
        {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << mSpeedMultiplier;
			statusText = "  " + oss.str() + " x >>" + "##ViewportControl";
		}
        else if (mSpeedMultiplier < 0)
        {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << -mSpeedMultiplier;
            statusText = "<<" + oss.str() + " x   " + "##ViewportControl";
        }
        

        // Move text to middle
        {   
            ImGui::PushStyleColor(ImGuiCol_Button, {});
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, {});
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {});

            ImGui::PushFont(ImGuiManager::BoldFont);
            ImGui::Button(statusText.c_str(), { ImGui::GetContentRegionAvail().x , 0});
            if (mIsPlaying == false && initialIn == false)
            {
                ImGui::Button("Paused##ViewportControl", {ImGui::GetContentRegionAvail().x , 0});
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(3);
        }


        if(mVideo)
        {
            if (mBBCache.find(mVideo->GetPath()) != mBBCache.end())
            {
                if (mBBCache[mVideo->GetPath()].find(mFrameNumber) != mBBCache[mVideo->GetPath()].end())
                {
                    const std::vector<BoundingBox>& boxes = mBBCache[mVideo->GetPath()][mFrameNumber];
                    for (const auto& AABB : boxes)
                    {
                        ImVec2 topLeft = { cursor.x + AABB.minX / mVideo->GetWidth() * regionX, cursor.y + AABB.minY / mVideo->GetHeight() * regionY };
                        ImVec2 bottomRight = { cursor.x + AABB.maxX / mVideo->GetWidth() * regionX, cursor.y + AABB.maxY / mVideo->GetHeight() * regionY };
                        drawList->AddRect(topLeft, bottomRight, IM_COL32(150, 170, 23, 255), 0.0f, 0, 5.0f); // red border, 2px thick
                    }
                }
            }
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
    void ViewportPanel::SetVideo(std::filesystem::path path)
    {
        if (mVideo)
            delete mVideo;
        mVideo = new Video(path);
        mVideo->DecodeTime(1.0f/60, mSpeedMultiplier);
        DTTrack = 0.f;
        mFrameNumber = 0;
    }
}
