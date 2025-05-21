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
#include <Graphics/Video.hpp>
namespace FrameExtractor
{
    static long long timer = 0;
    ViewportPanel::ViewportPanel(const std::string& name, ImVec2& size, ImVec2& pos) : 
        mName(name),
		mViewportSize(size),
		mViewportPos(pos)
    {
        mIcons[Icons::PLAY_ICON] = MakeRef<Texture>("resources/icons/Play.png");
        mIcons[Icons::STOP_ICON] = MakeRef<Texture>("resources/icons/Stop.png");
        mIcons[Icons::FORWARD_ICON] = MakeRef<Texture>("resources/icons/MoveFrameRight.png");
        mIcons[Icons::BACKWARD_ICON] = MakeRef<Texture>("resources/icons/MoveFrameLeft.png");
        mIcons[Icons::SPEED_UP_ICON] = MakeRef<Texture>("resources/icons/FastForward.png");
        mIcons[Icons::SLOW_DOWN_ICON] = MakeRef<Texture>("resources/icons/FastBackward.png");
        mIcons[Icons::SKIP_TO_END_ICON] = MakeRef<Texture>("resources/icons/CutToEnd.png");
        mIcons[Icons::SKIP_TO_START_ICON] = MakeRef<Texture>("resources/icons/CutToFront.png");
        mKeyIcons[KeyIcons::CTRL_ICON] = MakeRef<Texture>("resources/icons/Ctrl.png");
        mKeyIcons[KeyIcons::SHIFT_ICON] = MakeRef<Texture>("resources/icons/Shift.png");
        mKeyIcons[KeyIcons::LEFTARROW_ICON] = MakeRef<Texture>("resources/icons/leftArrow.png");
        mKeyIcons[KeyIcons::RIGHTARROW_ICON] = MakeRef<Texture>("resources/icons/rightArrow.png");
        mKeyIcons[KeyIcons::UPARROW_ICON] = MakeRef<Texture>("resources/icons/upArrow.png");
        mKeyIcons[KeyIcons::DOWNARROW_ICON] = MakeRef<Texture>("resources/icons/downArrow.png");
        mKeyIcons[KeyIcons::SPACE_ICON] = MakeRef<Texture>("resources/icons/Space.png");
        mKeyIcons[KeyIcons::PLUS_LOGO] = MakeRef<Texture>("resources/icons/PlusLogo.png");



    }

    ViewportPanel::~ViewportPanel()
    {
        if (mVideo) delete mVideo;
    }


    void ViewportPanel::OnImGuiRender(float dt)
    {
		//ImGui::SetNextWindowSize(mViewportSize);
		//ImGui::SetNextWindowPos(mViewportPos);

		ImGui::Begin(mName.c_str());

        // Get window size


		// Render the viewport here

       
        auto contentRegion = ImGui::GetContentRegionAvail();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (contentRegion.x - contentRegion.x * 0.85f) * 0.5f);
        auto cursor = ImGui::GetCursorScreenPos();
        auto regionX = contentRegion.x * 0.85f;
        auto regionY = contentRegion.y;
        if (mVideo)
        {
            ImGui::Image((ImTextureID)mVideo->GetFrame()->GetTextureID(), ImVec2(contentRegion.x * 0.85f, contentRegion.y * 0.8f));
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ITEM_NAME")) {
                    const char* droppedItem = static_cast<const char*>(payload->Data);
                    SetVideo({ droppedItem });
                }
            }
        }
        else
        {
            
            ImGui::Image(0, ImVec2(contentRegion.x * 0.85f, contentRegion.y * 0.8f));
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

            float lineHeight = (ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f);

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
                int totalSeconds = mVideo->GetMaxFrames() / mVideo->GetFPS();
                int frames = mVideo->GetMaxFrames() % mVideo->GetFPS();
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
        ImGui::Spacing();


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
                    if (mFrameNumber < mVideo->GetMaxFrames())
                    {
                        if (mSpeedMultiplier < 50 && mSpeedMultiplier > 0)
                            mVideo->DecodeTime(dt, mSpeedMultiplier);
                        else
                            mVideo->Decode(mFrameNumber);
                        DTTrack += dt * mSpeedMultiplier;
                        mFrameNumber = (int)(DTTrack * mVideo->GetFPS());
                        if (mFrameNumber >= mVideo->GetMaxFrames())
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
        float lineHeight = (ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f) * 1.5f;
        const int buttonCount = 7;
        const float buttonSize = 36.f * ImGuiManager::styleMultiplier;  // Width and height of each button
        const float spacing = ImGui::GetStyle().ItemSpacing.x;
        float totalWidth = buttonCount * buttonSize + (buttonCount - 1) * spacing;
        float startX = (ImGui::GetContentRegionAvail().x - totalWidth) * 0.5f;
        ImGuiIO& io = ImGui::GetIO();
       
        auto ctrlHeld = ImGui::IsKeyDown(ImGuiKey_RightCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftCtrl);
        auto shiftHeld = ImGui::IsKeyDown(ImGuiKey_RightShift) || ImGui::IsKeyDown(ImGuiKey_LeftShift);
        bool isWindowFocused = ImGui::IsWindowFocused();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + startX);
        if (ImGui::ImageButton("#IconSkipToStart", (ImTextureID)mIcons[SKIP_TO_START_ICON]->GetTextureID(), {buttonSize, buttonSize}) ||
            (isWindowFocused && ImGui::IsKeyPressed(ImGuiKey_Home)))
        {
            if (mVideo)
            {
                CommandHistory::execute(std::make_unique<SetVideoFrameCommand>(&mFrameNumber, mFrameNumber, 0, mVideo));
                mIsPlaying = false;
            }
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Image((ImTextureID)mKeyIcons[KeyIcons::SHIFT_ICON]->GetTextureID(), { lineHeight ,lineHeight });
            ImGui::SameLine();
            ImGui::Image((ImTextureID)mKeyIcons[KeyIcons::PLUS_LOGO]->GetTextureID(), { lineHeight / 1.5f ,lineHeight / 1.5f });
            ImGui::SameLine();
            ImGui::Image((ImTextureID)mKeyIcons[KeyIcons::LEFTARROW_ICON]->GetTextureID(), { lineHeight ,lineHeight });
            ImGui::EndTooltip();

        }

        ImGui::SameLine();
      
        if (ImGui::ImageButton("#IconSpeedUp", (ImTextureID)mIcons[SLOW_DOWN_ICON]->GetTextureID(), { buttonSize,buttonSize }) ||
            (isWindowFocused && shiftHeld && ImGui::IsKeyPressed(ImGuiKey_LeftArrow)))
        {
            if (mVideo)
                CommandHistory::execute(std::make_unique<CallFunctionCommand>(std::bind(&ViewportPanel::SlowDown, this), std::bind(&ViewportPanel::SpeedUp, this)));
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Image((ImTextureID)mKeyIcons[KeyIcons::CTRL_ICON]->GetTextureID(), { lineHeight ,lineHeight });
            ImGui::SameLine();
            ImGui::Image((ImTextureID)mKeyIcons[KeyIcons::PLUS_LOGO]->GetTextureID(), { lineHeight / 1.5f ,lineHeight / 1.5f });
            ImGui::SameLine();
            ImGui::Image((ImTextureID)mKeyIcons[KeyIcons::LEFTARROW_ICON]->GetTextureID(), { lineHeight ,lineHeight });
            ImGui::EndTooltip();
        }

        ImGui::SameLine();

        if (ImGui::ImageButton("#IconBackward", (ImTextureID)mIcons[BACKWARD_ICON]->GetTextureID(), { buttonSize,buttonSize }) ||
            (isWindowFocused && !shiftHeld && ImGui::IsKeyPressed(ImGuiKey_LeftArrow)))
        {
            if(mVideo)
            {
                float buffer = mFrameNumber;
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
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Image((ImTextureID)mKeyIcons[KeyIcons::LEFTARROW_ICON]->GetTextureID(), { lineHeight ,lineHeight });
            ImGui::EndTooltip();
        }

        ImGui::SameLine();

        if (!mIsPlaying)
        {
            if (ImGui::ImageButton("#IconPlay", (ImTextureID)mIcons[PLAY_ICON]->GetTextureID(), { buttonSize,buttonSize })
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
            if (ImGui::ImageButton("#IconStop", (ImTextureID)mIcons[STOP_ICON]->GetTextureID(), { buttonSize,buttonSize })
                || (isWindowFocused && ImGui::IsKeyPressed(ImGuiKey_Space)))
            {
                if(mVideo)
                {
                    CommandHistory::execute(std::make_unique<ModifyPropertyCommand<bool>>(&mIsPlaying, true, false));
                }
            }
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Image((ImTextureID)mKeyIcons[KeyIcons::SPACE_ICON]->GetTextureID(), { lineHeight ,lineHeight });
            ImGui::EndTooltip();
        }

        ImGui::SameLine();

        if (ImGui::ImageButton("#IconForward", (ImTextureID)mIcons[FORWARD_ICON]->GetTextureID(), { buttonSize,buttonSize }) ||
            (isWindowFocused && !shiftHeld && ImGui::IsKeyPressed(ImGuiKey_RightArrow)))
        {
            if(mVideo)
            {
                float buffer = mFrameNumber;
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
                    buffer = mVideo->GetMaxFrames();
                }
                CommandHistory::execute(std::make_unique<SetVideoFrameCommand>(&mFrameNumber, mFrameNumber, buffer, mVideo));
                mIsPlaying = false;
            }
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Image((ImTextureID)mKeyIcons[KeyIcons::RIGHTARROW_ICON]->GetTextureID(), { lineHeight ,lineHeight });
            ImGui::EndTooltip();
        }

        ImGui::SameLine();

        if (ImGui::ImageButton("#IconSlowDown", (ImTextureID)mIcons[SPEED_UP_ICON]->GetTextureID(), { buttonSize,buttonSize }) ||
            (isWindowFocused && shiftHeld && ImGui::IsKeyPressed(ImGuiKey_RightArrow)))
        {
            if(mVideo)
            {
                CommandHistory::execute(std::make_unique<CallFunctionCommand>(std::bind(&ViewportPanel::SpeedUp, this), std::bind(&ViewportPanel::SlowDown, this)));
            }
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Image((ImTextureID)mKeyIcons[KeyIcons::CTRL_ICON]->GetTextureID(), { lineHeight ,lineHeight });
            ImGui::SameLine();
            ImGui::Image((ImTextureID)mKeyIcons[KeyIcons::PLUS_LOGO]->GetTextureID(), { lineHeight / 1.5f ,lineHeight / 1.5f });
            ImGui::SameLine();
            ImGui::Image((ImTextureID)mKeyIcons[KeyIcons::RIGHTARROW_ICON]->GetTextureID(), { lineHeight ,lineHeight });
            ImGui::EndTooltip();
        }

        ImGui::SameLine();

        if (ImGui::ImageButton("#IconSkipToEnd", (ImTextureID)mIcons[SKIP_TO_END_ICON]->GetTextureID(), { buttonSize,buttonSize }) ||
            (isWindowFocused && ImGui::IsKeyPressed(ImGuiKey_End)))
        {
            if(mVideo)
            {
                mIsPlaying = false;
                CommandHistory::execute(std::make_unique<SetVideoFrameCommand>(&mFrameNumber, mFrameNumber, mVideo->GetMaxFrames() - 1, mVideo));
            }
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Image((ImTextureID)mKeyIcons[KeyIcons::SHIFT_ICON]->GetTextureID(), { lineHeight ,lineHeight });
            ImGui::SameLine();
            ImGui::Image((ImTextureID)mKeyIcons[KeyIcons::PLUS_LOGO]->GetTextureID(), { lineHeight / 1.5f ,lineHeight / 1.5f });
            ImGui::SameLine();
            ImGui::Image((ImTextureID)mKeyIcons[KeyIcons::RIGHTARROW_ICON]->GetTextureID(), { lineHeight ,lineHeight });
            ImGui::EndTooltip();
        }

        std::string statusText;
        if (mIsPlaying == false && initialIn == false)
        {
			statusText = "Paused";
		}
        else
        {
            if (mSpeedMultiplier > 0)
            {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(2) << mSpeedMultiplier;
				statusText = "  " + oss.str() + " x >>";
			}
            else if (mSpeedMultiplier < 0)
            {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(2) << -mSpeedMultiplier;
                statusText = "<<" + oss.str() + " x   ";
            }
        }

        // Move text to middle
        {
            ImGui::PushFont(ImGuiManager::BoldFont);
            ImVec2 textSize = ImGui::CalcTextSize(statusText.c_str());
            ImVec2 textPos2 = ImGui::GetCursorScreenPos();
            textPos2.x += (contentRegion.x - textSize.x) * 0.5f;
            // y is next line
            textPos2.y += ImGui::GetTextLineHeightWithSpacing() * 0.5f;
            drawList->AddText(textPos2, IM_COL32(255, 255, 255, 255), statusText.c_str());
            ImGui::PopFont();
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
