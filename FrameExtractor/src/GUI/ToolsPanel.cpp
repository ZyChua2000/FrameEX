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
#include <Core/Command.hpp>
#include <Core/PlatformUtils.hpp>
#include <GUI/ExplorerPanel.hpp>
#include <GUI/ImGuiManager.hpp>
#include <GUI/GUIUtils.hpp>
#include <GUI/GuiResourcesManager.hpp>
#include <Core/ExcelSerialiser.hpp>
#define ERROR_DATEFMT 1
#define ERROR_MISSINGFIELD 2
#define ERROR_NONE -1
namespace FrameExtractor
{

    ToolsPanel::ToolsPanel(Project* project) : mProject(project)
    {


    }

    ToolsPanel::~ToolsPanel()
    {
    }

    void ToolsPanel::OnImGuiRender(float dt)
    {
        ImGui::Begin("Tools");
        bool open_error_popup = false;

        if (ImGui::BeginTabBar("##ToolsBar", ImGuiTabBarFlags_Reorderable))
        {
            {
                CountingTab();
               
            }

            {
                AggregateTab();

            }

            {
                auto open = ImGui::BeginTabItem("Frame Extraction##Toolsbar");
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("Frame Extraction Tasks");
                    ImGui::EndTooltip();
                }
                if (open)
                {
                    // FRAME EXTRACTION TOOLSET HERE
                    ImGui::EndTabItem();
                }
            }

            {
                auto open = ImGui::BeginTabItem("Labelling##Toolsbar");
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("Labelling Tasks");
                    ImGui::EndTooltip();
                }
                if (open)
                {
                    // LABELLING TOOLSET HERE
                    ImGui::EndTabItem();
                }
            }

            ImGui::EndTabBar();

           
        }
        if (open_error_popup)
        {
            ImGui::OpenPopup("No Project Loaded##Modal");
        }
        {
            ImVec2 center = ImGui::GetWindowViewport()->Pos;
            center.x += ImGui::GetWindowViewport()->Size.x * 0.5f;
            center.y += ImGui::GetWindowViewport()->Size.y * 0.5f;
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        }
        if (ImGui::BeginPopupModal("No Project Loaded##Modal", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
        {
            ImGui::Text("No Project Loaded!\nPlease create or load a project first.");
            float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
            if (ImGui::Button("X##NoProjectLoadedModal", { lineHeight, 0 }))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        ImGui::End();
    }

    const char* ToolsPanel::GetName() const
    {
        return "Tools";
    }
    

    void ToolsPanel::OnAttach()
    {

    }

    static std::string MonthToString(int month)
    {
        if (month < 0 || month > 12) return "Unknown";

        switch (month)
        {
        case 1: return "January";
        case 2: return "February";
        case 3: return "March";
        case 4: return "April";
        case 5: return "May";
        case 6: return "June";
        case 7: return "July";
        case 8: return "August";
        case 9: return "September";
        case 10: return "October";
        case 11: return "November";
        case 12: return "December";
        default: return "Unknown";
        }
    }

    static std::string IntDayToSuffix(int day) {
        if (day < 1 || day > 31) return "Invalid day";

        std::string suffix;

        if (day % 100 >= 11 && day % 100 <= 13) {
            suffix = "th";
        }
        else {
            switch (day % 10) {
            case 1: suffix = "st"; break;
            case 2: suffix = "nd"; break;
            case 3: suffix = "rd"; break;
            default: suffix = "th"; break;
            }
        }

        return std::to_string(day) + suffix;
    }
    
    static std::string DateIntToStr(const int& date)
    {
        int day = date / 1000000;
        int month = (date / 10000) % 100;
        int year = date % 10000;

        return IntDayToSuffix(day) + " " + MonthToString(month) + " " + std::to_string(year);
    }

    std::string ToolsPanel::ExportAggregateStoreDataAsString(int date)
    {
        std::stringstream ss;
        ss << "Hi all, Mailers and additional checks done:" << std::endl << "Anu Selma Jose" << std::endl << std::endl;
        for (auto& [store, timeNData] : mProject->mAggregateStoreData)
        {
            ss << DateIntToStr(date) << ":" << std::endl << std::endl;
 
            for (auto& [time, data] : timeNData)
            {

                std::string reformattedDate = std::to_string(date);
                // change date from DDMMYYYY to YYYYMMDD
                std::string year = reformattedDate.substr(4, 4);
                std::string month = reformattedDate.substr(2, 2);
                std::string day = reformattedDate.substr(0, 2);
                reformattedDate = year + month + day;
                ss << store << ", " << data.StoreID << ", " << reformattedDate << ", "  << time << ", " << (int)data.Enters << ", " << (int)data.Exit << " -> " <<
                    data.mCustomer << " Customers";
                int idx = 1;
                for (auto& entrance : data.Entrance)
                {
                    if (data.Entrance.size() > 1)
                    {
                        ss << std::endl << "E" << idx;
                    }
                    for (auto& frameSkip : entrance.mFrameSkips)
                    {
                        ss << ", Video Skips from: " << frameSkip.first << " to " << frameSkip.second;              
                    }

                    for (auto& blankVideo : entrance.mBlankedVideos)
                    {
                        if (blankVideo.first)
                        {
                            ss << ", Video starts after " << blankVideo.second;
                        }
                        else
                        {
                            ss << ", Video blanks after " << blankVideo.second;
                        }
                    }

                    for (auto& corruptedVideo : entrance.mCorruptedVideos)
                    {
                        ss << ", Video " << corruptedVideo << " is corrupted";
                    }
                    idx++;
                }
                ss << "\n";
            }
            ss << std::endl;
            
            ss << 
                "Spike dip for " << IntDayToSuffix(date / 1000000) << " " << 
                MonthToString((date / 10000) % 100) << " counted" << std::endl;
        }

        return ss.str();
    }

    void ToolsPanel::CountingTab()
    {
        bool open_clear_popup = false;
        bool open_error_popup = false;
        bool delete_store_popup = false;
        auto& mCountingData = mProject->mCountingData;
        auto open = ImGui::BeginTabItem("Counting##ToolsBar");
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4,4 });
        float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
        {
            ImGui::BeginTooltip();
            ImGui::Text("Counting Tasks (e.g. Spike Dip)");
            ImGui::EndTooltip();
        }
        if (open)
        {
            if (ImGui::ImageButton("Add Entry##Counting", Resource(Icon::ADDFILE_ICON)->GetTextureID(), { lineHeight, lineHeight }))
            {
                if (!mProject->IsProjectLoaded())
                {
                   open_error_popup = true;
                }
                else
                    ImGui::OpenPopup("AddEntryPopup##Counting");
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
            {
                ImGui::BeginTooltip();
                ImGui::Text("Add Entry");
                ImGui::EndTooltip();
            }

            ImGui::SameLine();

            if (ImGui::ImageButton("Clear##Counting", Resource(Icon::CLEAR_ICON)->GetTextureID(), { lineHeight, lineHeight }))
            {
                if (!mProject->IsProjectLoaded())
                {
					open_error_popup = true;
				}
				else
					open_clear_popup = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
            {
                ImGui::BeginTooltip();
                ImGui::Text("Clear All Entries");
                ImGui::EndTooltip();
            }

            ImGui::SameLine();

            if (ImGui::ImageButton("Import Data##Counting", Resource(Icon::IMPORT_ICON)->GetTextureID(), { lineHeight ,lineHeight  }))
            {
                if (!mProject->IsProjectLoaded())
                {
                    open_error_popup = true;
                }
                else
                {
                    auto spikeDipFile = OpenFileDialog("Excel File (*.xlsx)\0*.xlsx\0");
                    if (std::filesystem::exists(spikeDipFile))
                    {
                        ExcelSerialiser serialiser(spikeDipFile);
                        CommandHistory::execute(std::make_unique<ModifyPropertyCommand<std::map<Project::StoreCode, std::map<Project::Hour, CountData>>>>(&mProject->mCountingData, mProject->mCountingData, serialiser.ImportSpikeDipReport()));
                    }
                    else
                    {
                        APP_CORE_ERROR("Spike Dip file does not exist!");
                    }
                }
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
            {
                ImGui::BeginTooltip();
                ImGui::Text("Import Data (.xlsx)");
                ImGui::EndTooltip();
            }
            ImGui::SameLine();

            if (ImGui::ImageButton("Export Data##Counting", Resource(Icon::EXPORT_ICON)->GetTextureID(), { lineHeight, lineHeight }))
            {
                if (!mProject->IsProjectLoaded())
                {
                    open_error_popup = true;
                }
                else {
                    auto projectFile = SaveFileDialog("Excel File (*.xlsx)\0*.xlsx\0");
                    projectFile.replace_extension(".xlsx");
                    ExcelSerialiser serialiser(projectFile);
                    serialiser.ExportSpikeDipReport(mProject->mCountingData);
                }

            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
            {
                ImGui::BeginTooltip();
                ImGui::Text("Export Data (.xlsx)");
                ImGui::EndTooltip();
            }


            ImGui::SetNextWindowSize({ 270 * ImGuiManager::styleMultiplier, 120 * ImGuiManager::styleMultiplier + 40 }, ImGuiCond_Always);
            if (ImGui::BeginPopup("AddEntryPopup##Counting", ImGuiWindowFlags_NoMove))
            {

                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 120 * ImGuiManager::styleMultiplier);
                ImGui::Text("Store Code: ");
                ImGui::NextColumn();
                ImGui::SetNextItemWidth(130 * ImGuiManager::styleMultiplier);
                ImGui::InputText("##Store Code##Counting: ", mStoreCodeBuffer, 16);
                ImGui::NextColumn();

                ImGui::Text("Hour: ");
                ImGui::NextColumn();
                ImGui::SetNextItemWidth(130 * ImGuiManager::styleMultiplier);
                ImGui::InputInt("##Hour:##Counting ", &mTimeBuffer, 1, 1);
                ImGui::NextColumn();

                ImGui::Text("Entrances: ");
                ImGui::NextColumn();
                ImGui::SetNextItemWidth(130 * ImGuiManager::styleMultiplier);
                ImGui::InputInt("##Entrances##Counting: ", &mEntranceBuffer, 1, 1);
                ImGui::Columns(1);
                ImGui::Separator();
                if (ImGui::Button("Confirm"))
                {
                    std::string storeID(mStoreCodeBuffer);
                    if (storeID != "")
                    {
                        std::memset(mStoreCodeBuffer, 0, 16);
                        if (!mCountingData.contains(storeID))
                        {
                            if (!mCountingData.empty()){
                                auto currentIT = mCountingData.begin();
                                std::advance(currentIT, mCountingPage.mStorePage);
                                if (currentIT->first > storeID)
                                {
                                    mCountingPage.mStorePage++;
                                }
                            }
                        }
                        CommandHistory::execute(std::make_unique<AddStoreEntryCounting>(&mCountingData, storeID, mEntranceBuffer, mTimeBuffer));

                        mEntranceBuffer = 1;
                        mTimeBuffer = 0;
                    }
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SameLine();

                if (ImGui::Button("Cancel##Counting"))
                {
                    std::memset(mStoreCodeBuffer, 0, 16);
                    mEntranceBuffer = 1;
                    mTimeBuffer = 0;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }


            ImVec2 windowSize = ImGui::GetContentRegionAvail();
            ImGui::BeginChild("ScrollableRegion##Counting", ImVec2(windowSize.x, windowSize.y), true);
            if (mCountingPage.mStorePage > 0)
            {
                if (ImGui::ArrowButton("##CountingPageBack", ImGuiDir_Left))
                {

                    CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int>>(&mCountingPage.mStorePage, mCountingPage.mStorePage, mCountingPage.mStorePage - 1));

                }
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("Previous Store");
                    ImGui::EndTooltip();
                }
            }
            else
            {
                ImGui::InvisibleButton("##DummyArrow1", ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize()));
            }
           
            ImGui::SameLine();

            float spacing = ImGui::GetStyle().ItemSpacing.x;
            float arrow_button_width = ImGui::GetFrameHeight(); // Arrow buttons are square
            float total_spacing = spacing * 3; // space between 3 items

            // Calculate remaining width
            float remaining_width = ImGui::GetContentRegionAvail().x;
            float middle_button_width = remaining_width - (arrow_button_width * 2 + total_spacing);

            std::string PageNumStr = "NIL";

            if (!mProject->IsProjectLoaded()) PageNumStr = "No Project Loaded";
            else
                if (!mCountingData.empty())
                {
                    if (mCountingPage.mStorePage >= mCountingData.size())
                    {
                        mCountingPage.mStorePage = (int)mCountingData.size() - 1;
                    }

                    auto StorePageIT = mCountingData.begin();
                    std::advance(StorePageIT, mCountingPage.mStorePage);
                    PageNumStr = (StorePageIT->first);
                }

            std::vector<std::string> keys;

            // Iterate over the map and push the keys into the vector
            for (const auto& pair : mCountingData) {
                keys.push_back(pair.first);  // pair.first is the key
            }

            ImGui::SetNextItemWidth(middle_button_width);

            // Save cursor before drawing label so we can restore it later
            ImVec2 cursor_before_label = ImGui::GetCursorScreenPos();

            if (ImGui::BeginCombo("##CountingStoreList", "", ImGuiComboFlags_None))
            {
                for (int i = 0; i < keys.size(); i++)
                {
                    bool is_selected = mCountingPage.mStorePage == i;
                    if (ImGui::Selectable(keys[i].c_str(), &is_selected))
                        CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int>>(&mCountingPage.mStorePage, mCountingPage.mStorePage, i));

                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::PushFont(ImGuiManager::BoldFont);
            if (!keys.empty())
            {
                // Center the label manually
                ImVec2 combo_pos = ImGui::GetItemRectMin(); // Position of combo box
                ImVec2 combo_size = ImGui::GetItemRectSize();
                const char* label = keys[mCountingPage.mStorePage].c_str();
                ImVec2 text_size = ImGui::CalcTextSize(label);

                ImVec2 text_pos = ImVec2(
                    combo_pos.x + (combo_size.x - text_size.x) * 0.5f,
                    combo_pos.y + (combo_size.y - text_size.y) * 0.5f
                );

                ImGui::SetCursorScreenPos(text_pos);
                ImGui::TextUnformatted(label);

                // Restore cursor after drawing the centered text
                ImGui::SetCursorScreenPos(cursor_before_label);
                ImGui::Dummy(combo_size);  // Reserve the space for the combo box
            }
            else
            {
                ImVec2 combo_pos = ImGui::GetItemRectMin(); // Position of combo box
                ImVec2 combo_size = ImGui::GetItemRectSize();
                const char* label = PageNumStr.c_str();
                ImVec2 text_size = ImGui::CalcTextSize(label);

                ImVec2 text_pos = ImVec2(
                    combo_pos.x + (combo_size.x - text_size.x) * 0.5f,
                    combo_pos.y + (combo_size.y - text_size.y) * 0.5f
                );

                ImGui::SetCursorScreenPos(text_pos);
                ImGui::TextUnformatted(label);

                // Restore cursor after drawing the centered text
                ImGui::SetCursorScreenPos(cursor_before_label);
                ImGui::Dummy(combo_size);  // Reserve the space for the combo box
            }
            ImGui::PopFont();
            

            // Ensure SameLine aligns the arrow correctly
            ImGui::SameLine();  // This forces the next item to be on the same line


            if (mCountingPage.mStorePage + 1< mCountingData.size())
            {
                if (ImGui::ArrowButton("##CountingPageNext", ImGuiDir_Right))
                {

                    CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int>>(&mCountingPage.mStorePage, mCountingPage.mStorePage, mCountingPage.mStorePage + 1));
                }
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("Next Store");
                    ImGui::EndTooltip();
                }
            }
            else
            {
                ImGui::InvisibleButton("##DummyArrow2", ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize()));
            }

            if (!mCountingData.empty())
            {
                ImGui::SameLine();
                if (ImGui::ImageButton("##CountingPageSettings", Resource(Icon::SETTINGS_ICON)->GetTextureID(), {ImGui::GetFontSize(), ImGui::GetFontSize() }))
                {
                    ImGui::OpenPopup("StoreSettings##Counting");
                }
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("Store Settings");
                    ImGui::EndTooltip();
                }
            }

 
            ImGui::Separator();
            ImGui::Spacing();

            if (!mCountingData.empty())
            {
                auto StorePageIT = mCountingData.begin();
                std::advance(StorePageIT, mCountingPage.mStorePage);
                auto& StorePageITData = *StorePageIT;
                auto StoreCodeFromIT = StorePageITData.first;
                auto& TimeDataFromIT = StorePageITData.second;
       


                if (ImGui::BeginPopup("StoreSettings##Counting", ImGuiWindowFlags_NoMove))
                {
                    if (ImGui::MenuItem("Add New Entrance##CountingStoreSettings"))
                    {
                        for (auto& [time, data] : mCountingData[StoreCodeFromIT])
                        {
                            data.Entrance.push_back({});
                        }
                    }
                    ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false);
                    if (ImGui::MenuItem("Add New Time##CountingStoreSettings"))
                    {
                        ImGui::OpenPopup("AddTimePopUp##Counting");
                    }
                    ImGui::PopItemFlag();

                    ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false);
                    if (ImGui::MenuItem("Remove This Store##CountingStoreSettings"))
                    {
                        ImGui::OpenPopup("Remove Store##Counting");
                    }
                    ImGui::PopItemFlag();

                    ImGui::Separator();

                    if (ImGui::MenuItem("Close##Counting"))
                        ImGui::CloseCurrentPopup();

                    {
                        bool closePopup = false;
                        if (ImGui::BeginPopup("AddTimePopUp##Counting", ImGuiWindowFlags_NoMove))
                        {
                            ImGui::Text("Hour: ");
                            ImGui::SameLine();
                            ImGui::InputInt("##Hour:##CountingTimePopup ", &mTimeBuffer, 1, 1, ImGuiInputTextFlags_CharsDecimal);
                            if (ImGui::Button("Confirm##AddTimePopup##Counting"))
                            {
                                if (mTimeBuffer >= 0 && mTimeBuffer < 24)
                                {
                                    if (!mCountingData[StoreCodeFromIT].contains(mTimeBuffer))
                                    {
                                        auto entranceNum = mCountingData[StoreCodeFromIT].begin()->second.Entrance.size();

                                        mCountingData[StoreCodeFromIT][mTimeBuffer] = {};
                                        for (int i = 0; i < entranceNum; i++)
                                        {
                                            mCountingData[StoreCodeFromIT][mTimeBuffer].Entrance.push_back({});
                                        }
                                    }
                                    mTimeBuffer = 0;
                                }
                                closePopup = true;
                                ImGui::CloseCurrentPopup();
                            }

                            if (ImGui::Button("Cancel##AddTimePopup##Counting"))
                            {
                                ImGui::CloseCurrentPopup();
                                mTimeBuffer = 0;
                            }

                            ImGui::EndPopup();
                        }

                        if (closePopup)
                        {
                            ImGui::CloseCurrentPopup();
                        }
                    }

                    {
                        bool closePopup = false;
                        {
                            ImVec2 center = ImGui::GetWindowViewport()->Pos;
                            center.x += ImGui::GetWindowViewport()->Size.x * 0.5f;
                            center.y += ImGui::GetWindowViewport()->Size.y * 0.5f;
                            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
                        }
                        if (ImGui::BeginPopupModal("Remove Store##Counting", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize))
                        {
                            ImGui::Text("Are you sure you want to remove this store?");

                            float spacing = ImGui::GetStyle().ItemSpacing.x;
                            float totalWidth = lineHeight * 4 + spacing;
                            float windowWidth = ImGui::GetWindowSize().x;
                            float startX = (windowWidth - totalWidth) * 0.5f;
                            ImGui::Spacing();
                            ImGui::SetCursorPosX(startX);
                            if (ImGui::Button("No##RemoveStoreModal", { lineHeight * 2, lineHeight }))
                            {
                                ImGui::CloseCurrentPopup();
                            }
                            ImGui::SameLine();
                            if (ImGui::Button("Yes##RemoveStoreModal", { lineHeight * 2, lineHeight }))
                            {
                                CommandHistory::execute(std::make_unique<EraseKeyCommand<std::map<Project::StoreCode,std::map<Project::Hour,CountData>>>>(&mCountingData, mCountingPage.mStorePage));
                                if (mCountingPage.mStorePage != 0)
                                {
                                    mCountingPage.mStorePage--;
                                }
                                //CommandHistory::execute(std::make_unique<EraseKeyIteratorCommand<std::map<Project::StoreCode, std::map<Project::Hour, CountData>>>>(&mCountingData, &StorePageIT));
                               // if (StorePageIT != mCountingData.begin())
                                {
                                //    std::advance(StorePageIT, -1);
                                //    mCountingPage.mStorePage -= 1;
                                }

                                closePopup = true;
                                ImGui::CloseCurrentPopup();
                            }

                            ImGui::EndPopup();
                        }

                        if (closePopup)
                        {
                            ImGui::CloseCurrentPopup();
                        }
                    }

                    ImGui::EndPopup();
                }

               


                if (!mCountingData.empty())
                {
                    auto StorePageITPostOp = mCountingData.begin();
                    std::advance(StorePageITPostOp, mCountingPage.mStorePage);

                    auto StoreCode = StorePageITPostOp->first;

                    if (ImGui::BeginTabBar("##CountingTabBar"))
                    {
                        int houridx = 0;
                        for (auto& [hour, Data] : mCountingData[StoreCode])
                        {
                            std::string hourText = "        ";
                            if (hour >= 10)
                                hourText += std::to_string(hour) + "hrs  ";
                            else
                                hourText += "0" + std::to_string(hour) + "hrs  ";

                            bool hour2Bool = true;
                            if (ImGui::BeginTabItem((hourText + "##Counting").c_str(), &hour2Bool, ImGuiTabItemFlags_NoReorder))
                            {
                                mCountingPage.mHourPage = houridx;

                                auto hourIT = mCountingData[StoreCode].begin();
                                std::advance(hourIT, mCountingPage.mHourPage);
                                auto& Hour = hourIT->first;
                                auto& Data = hourIT->second;


                                if (ImGui::CollapsingHeader("Statistics##Counting", ImGuiTreeNodeFlags_DefaultOpen))
                                {
                                    ImGui::Indent(lineHeight);
                                    ImGui::Columns(2);
                                    {
                                        ImGui::PushFont(ImGuiManager::BoldFont);
                                        ImGui::Text("Customer: ");
                                        ImGui::PopFont();
                                        ImGui::NextColumn();
                                        auto buffer = Data.mCustomer;
                                        ImGui::SetNextItemWidth(lineHeight * 2);

                                        if (ImGui::InputInt("##Customer##Counting", &buffer, 0, 0, ImGuiInputTextFlags_CharsDecimal))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mCustomer, Data.mCustomer, buffer));
                                        }
                                        
                                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4,4 });
                                        ImGui::SameLine();
                                        if (ImGui::Button("-##Customer##Counting", { lineHeight, 0 }))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mCustomer, Data.mCustomer, Data.mCustomer - 1));
                                        }
                                        ImGui::SameLine();
                                        if (ImGui::Button("+##Customer##Counting", { lineHeight, 0 }))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mCustomer, Data.mCustomer, Data.mCustomer + 1));
                                        }
                                        ImGui::PopStyleVar();
                                        ImGui::NextColumn();
                                    }


                                    {
                                        ImGui::PushFont(ImGuiManager::BoldFont);
                                        ImGui::Text("Re-entry Customer: ");
                                        ImGui::PopFont();
                                        ImGui::NextColumn();
                                        auto buffer = Data.mReCustomer;
                                        ImGui::SetNextItemWidth(lineHeight * 2);

                                        if (ImGui::InputInt("##ReCustomer##Counting", &buffer, 0, 0, ImGuiInputTextFlags_CharsDecimal))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mReCustomer, Data.mReCustomer, buffer));
                                        }
                                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4,4 });
                                        ImGui::SameLine();
                                        if (ImGui::Button("-##ReCustomer##Counting", { lineHeight, 0 }))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyPairCommand<int32_t>>(&Data.mCustomer, Data.mCustomer, Data.mCustomer - 1, &Data.mReCustomer, Data.mReCustomer, Data.mReCustomer - 1));
                                        }
                                        ImGui::SameLine();
                                        if (ImGui::Button("+##ReCustomer##Counting", { lineHeight, 0 }))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyPairCommand<int32_t>>(&Data.mCustomer, Data.mCustomer, Data.mCustomer + 1, &Data.mReCustomer, Data.mReCustomer, Data.mReCustomer + 1));
                                        }
                                        ImGui::PopStyleVar();
                                        ImGui::NextColumn();

                                    }

                                    {
                                        ImGui::PushFont(ImGuiManager::BoldFont);
                                        ImGui::Text("Suspected Staff: ");
                                        ImGui::PopFont();
                                        ImGui::NextColumn();
                                        auto buffer = Data.mSuspectedStaff;
                                        ImGui::SetNextItemWidth(lineHeight * 2);

                                        if (ImGui::InputInt("##SusStaff##Counting", &buffer, 0, 0, ImGuiInputTextFlags_CharsDecimal))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mSuspectedStaff, Data.mSuspectedStaff, buffer));
                                        }
                                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4,4 });
                                        ImGui::SameLine();
                                        if (ImGui::Button("-##mSuspectedStaff##Counting", { lineHeight, 0 }))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mSuspectedStaff, Data.mSuspectedStaff, Data.mSuspectedStaff - 1));
                                        }
                                        ImGui::SameLine();
                                        if (ImGui::Button("+##mSuspectedStaff##Counting", { lineHeight, 0 }))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mSuspectedStaff, Data.mSuspectedStaff, Data.mSuspectedStaff + 1));
                                        }
                                        ImGui::PopStyleVar();
                                        ImGui::NextColumn();

                                    }

                                    {
                                        ImGui::PushFont(ImGuiManager::BoldFont);
                                        ImGui::Text("Re-entry Suspected Staff: ");
                                        ImGui::PopFont();
                                        ImGui::NextColumn();
                                        auto buffer = Data.mReSuspectedStaff;
                                        ImGui::SetNextItemWidth(lineHeight * 2);

                                        if (ImGui::InputInt("##ReSusStaff##Counting", &buffer, 0, 0, ImGuiInputTextFlags_CharsDecimal))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mReSuspectedStaff, Data.mReSuspectedStaff, buffer));
                                        }
                                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4,4 });
                                        ImGui::SameLine();
                                        if (ImGui::Button("-##ReSusStaff##Counting", { lineHeight, 0 }))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyPairCommand<int32_t>>(&Data.mSuspectedStaff, Data.mSuspectedStaff, Data.mSuspectedStaff - 1, &Data.mReSuspectedStaff, Data.mReSuspectedStaff, Data.mReSuspectedStaff - 1));
                                        }
                                        ImGui::SameLine();
                                        if (ImGui::Button("+##ReSusStaff##Counting", { lineHeight, 0 }))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyPairCommand<int32_t>>(&Data.mSuspectedStaff, Data.mSuspectedStaff, Data.mSuspectedStaff + 1, &Data.mReSuspectedStaff, Data.mReSuspectedStaff, Data.mReSuspectedStaff + 1));
                                        }
                                        ImGui::PopStyleVar();
                                        ImGui::NextColumn();

                                    }

                                    {
                                        ImGui::PushFont(ImGuiManager::BoldFont);
                                        ImGui::Text("Children: ");
                                        ImGui::PopFont();
                                        ImGui::NextColumn();
                                        auto buffer = Data.mChildren;
                                        ImGui::SetNextItemWidth(lineHeight * 2);

                                        if (ImGui::InputInt("##Children##Counting", &buffer, 0, 0, ImGuiInputTextFlags_CharsDecimal))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mChildren, Data.mChildren, buffer));
                                        }
                                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4,4 });
                                        ImGui::SameLine();
                                        if (ImGui::Button("-##Children##Counting", { lineHeight, 0 }))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mChildren, Data.mChildren, Data.mChildren - 1));
                                        }
                                        ImGui::SameLine();
                                        if (ImGui::Button("+##Children##Counting", { lineHeight, 0 }))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mChildren, Data.mChildren, Data.mChildren + 1));
                                        }
                                        ImGui::PopStyleVar();
                                        ImGui::NextColumn();

                                    }

                                    {
                                        ImGui::PushFont(ImGuiManager::BoldFont);
                                        ImGui::Text("Re-entry Children: ");
                                        ImGui::PopFont();
                                        ImGui::NextColumn();
                                        auto buffer = Data.mReChildren;
                                        ImGui::SetNextItemWidth(lineHeight * 2);

                                        if (ImGui::InputInt("##ReChildren##Counting", &buffer, 0, 0, ImGuiInputTextFlags_CharsDecimal))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mReChildren, Data.mReChildren, buffer));
                                        }
                                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4,4 });
                                        ImGui::SameLine();
                                        if (ImGui::Button("-##ReChildren##Counting", { lineHeight, 0 }))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyPairCommand<int32_t>>(&Data.mChildren, Data.mChildren, Data.mChildren - 1, &Data.mReChildren, Data.mReChildren, Data.mReChildren - 1));
                                        }
                                        ImGui::SameLine();
                                        if (ImGui::Button("+##ReChildren##Counting", { lineHeight, 0 }))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyPairCommand<int32_t>>(&Data.mChildren, Data.mChildren, Data.mChildren + 1, &Data.mReChildren, Data.mReChildren, Data.mReChildren + 1));
                                        }
                                        ImGui::PopStyleVar();
                                        ImGui::NextColumn();

                                    }
                                    {
                                        ImGui::PushFont(ImGuiManager::BoldFont);
                                        ImGui::Text("Others: ");
                                        ImGui::PopFont();
                                        ImGui::NextColumn();
                                        auto buffer = Data.mOthers;
                                        ImGui::SetNextItemWidth(lineHeight * 2);

                                        if (ImGui::InputInt("##Others##Counting", &buffer, 0, 0, ImGuiInputTextFlags_CharsDecimal))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mOthers, Data.mOthers, buffer));
                                        }
                                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4,4 });
                                        ImGui::SameLine();
                                        if (ImGui::Button("-##Others##Counting", { lineHeight, 0 }))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mOthers, Data.mOthers, Data.mOthers - 1));
                                        }
                                        ImGui::SameLine();
                                        if (ImGui::Button("+##Others##Counting", { lineHeight, 0 }))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mOthers, Data.mOthers, Data.mOthers + 1));
                                        }
                                        ImGui::PopStyleVar();
                                        ImGui::NextColumn();

                                    }

                                    {
                                        ImGui::PushFont(ImGuiManager::BoldFont);
                                        ImGui::Text("Re-entry Others: ");
                                        ImGui::PopFont();
                                        ImGui::NextColumn();
                                        auto buffer = Data.mReOthers;
                                        ImGui::SetNextItemWidth(lineHeight * 2);
                                        if (ImGui::InputInt("##ReOthers##Counting", &buffer, 0, 0, ImGuiInputTextFlags_CharsDecimal))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mReOthers, Data.mReOthers, buffer));
                                        }
                                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4,4 });
                                        ImGui::SameLine();
                                        if (ImGui::Button("-##ReOthers##Counting", { lineHeight, 0 }))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyPairCommand<int32_t>>(&Data.mOthers, Data.mOthers, Data.mOthers - 1, &Data.mReOthers, Data.mReOthers, Data.mReOthers - 1));
                                        }
                                        ImGui::SameLine();
                                        if (ImGui::Button("+##ReOthers##Counting", { lineHeight, 0 }))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyPairCommand<int32_t>>(&Data.mOthers, Data.mOthers, Data.mOthers + 1, &Data.mReOthers, Data.mReOthers, Data.mReOthers + 1));
                                        }
                                        ImGui::PopStyleVar();
                                        ImGui::NextColumn();

                                    }
                                    ImGui::Columns(1);
                                    ImGui::Unindent(lineHeight);

                                }

                                auto notesOpen = ImGui::CollapsingHeader("Notes##Counting", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen);
                                ImGui::Indent(lineHeight);
                                if (notesOpen)
                                {
                                    int idx = 1;
                                    for (auto& Entrance : Data.Entrance)
                                    {

                                        auto entranceOpen = ImGui::CollapsingHeader(("Entrance " + std::to_string(idx) + "##Counting").c_str(), ImGuiTreeNodeFlags_AllowItemOverlap);
                                        if (Data.Entrance.size() > 1)
                                        {
                                            ImGui::SameLine(ImGui::GetContentRegionAvail().x + lineHeight * 0.5f); // Align to right (Button)


                                            if (ImGui::Button(("-##MinusEntranceCounting" + std::to_string(idx)).c_str(), { lineHeight, 0 }))
                                            {
                                                CommandHistory::execute(std::make_unique<VectorEraseCommand<CountingEntrance>>(&Data.Entrance, idx-1));
                                                break;
                                            }

                                        }
                                        if (entranceOpen)
                                        {
                                            ImGui::SetNextWindowSizeConstraints({ ImGui::GetContentRegionAvail().x , lineHeight * 20 }, { ImGui::GetContentRegionAvail().x , lineHeight * 20 });
                                            ImGui::BeginChild(("Entrance" + std::to_string(idx) + "##CountingChild").c_str(), {}, ImGuiChildFlags_Border);
                                            int idx2 = 0;


                                            for (int entryType = EntryType::ReCustomer; entryType <= EntryType::ReOthers; entryType++)
                                            {
                                                int32_t deleteIdx = -1;
                                                if (ImGui::Button(("+##mTimestamp2##Counting" + EntryTypeToString((EntryType)entryType) + std::to_string(hour) + StoreCode + std::to_string(idx)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                                {
                                                    CommandHistory::execute(std::make_unique<PushBackCommand<PersonDesc>>(&mCountingData[StoreCode][hour].Entrance[idx - 1].mDesc[entryType], PersonDesc{}));
                                                }
                                                ImGui::SameLine();
                                                ImGui::Text((EntryTypeToString((EntryType)entryType) + " Descriptions").c_str());

                                                ImGui::Indent(lineHeight);
                                                for (int32_t entry = 0; entry < mCountingData[StoreCode][hour].Entrance[idx-1].mDesc[entryType].size(); entry++)
                                                {

                                                    auto& data = mCountingData[StoreCode][hour].Entrance[idx - 1].mDesc[entryType][entry];
                                                    char DescBuffer[128] = {};
                                                    char timeStampBuffer[16] = {};
                                                    std::memcpy(timeStampBuffer, data.timeStamp.c_str(), data.timeStamp.size());
                                                    std::memcpy(DescBuffer, data.Description.c_str(), 128);

                                                    if (data.IsMale)
                                                    {
                                                        if (ImGui::Button(("M####Counting" + EntryTypeToString((EntryType)entryType) + data.timeStamp + std::to_string(hour) + StoreCode + std::to_string(entry)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                                        {
                                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<bool>>(&data.IsMale, data.IsMale, !data.IsMale));
                                                        }
                                                    }
                                                    else
                                                    {
                                                        if (ImGui::Button(("F####Counting" + EntryTypeToString((EntryType)entryType) + data.timeStamp + std::to_string(hour) + StoreCode + std::to_string(entry)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                                        {
                                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<bool>>(&data.IsMale, data.IsMale, !data.IsMale));
                                                        }
                                                    }
                                                    ImGui::SameLine();

                                                    Widget::Time(("##timestamp##Counting" + EntryTypeToString((EntryType)entryType) + data.timeStamp + std::to_string(hour) + StoreCode + std::to_string(entry)).c_str(), data.timeStamp, 80 * ImGuiManager::styleMultiplier);
                                                   
                                                    ImGui::SameLine();
                                                    {
                                                        auto lineLength = ImGui::GetContentRegionAvail().x;
                                                        lineLength -= (lineHeight + ImGui::GetStyle().FramePadding.x * 3);
                                                        ImGui::SetNextItemWidth(lineLength);
                                                    }


                                                    if (ImGui::InputText(("##Description##Counting" + EntryTypeToString((EntryType)entryType) + data.timeStamp + std::to_string(hour) + StoreCode + std::to_string(entry)).c_str(), DescBuffer, 128))
                                                    {
                                                        CommandHistory::execute(std::make_unique<ModifyPropertyCommand<std::string>>(&data.Description, data.Description, std::string(DescBuffer)));

                                                    }
                                                    ImGui::SameLine();
                                                    if (ImGui::Button(("-####Counting" + EntryTypeToString((EntryType)entryType) + data.timeStamp + StoreCode + std::to_string(hour) + std::to_string(entry)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                                    {
                                                        deleteIdx = entry;
                                                    }

                                                }
                                                ImGui::Unindent(lineHeight);

                                                ImGui::Separator();
                                                if (deleteIdx != -1)
                                                {
                                                    CommandHistory::execute(std::make_unique<VectorEraseCommand<PersonDesc>>(&mCountingData[StoreCode][hour].Entrance[idx - 1].mDesc[entryType], deleteIdx));
                                                }

                                            }

                                            ImGui::NewLine();

                                            if (ImGui::Button(("+##AddFrameSkip##Counting" + std::to_string(idx)).c_str(), ImVec2{ lineHeight, 0 }))
                                            {

                                                CommandHistory::execute(std::make_unique<PushBackCommand<std::pair<std::string, std::string>>>(&Entrance.mFrameSkips, std::pair<std::string, std::string>("00:00:00", "00:00:00")));
                                            }

                                            ImGui::SameLine();
                                            ImGui::PushFont(ImGuiManager::BoldFont);
                                            ImGui::Text("Frame Skips");
                                            ImGui::PopFont();

                                            ImGui::Columns(2);
                                            ImGui::SetColumnWidth(0, lineHeight * 5);
                                            if (!Entrance.mFrameSkips.empty())
                                            {
                                                ImGui::Text("Start Time");
                                                ImGui::NextColumn();
                                                ImGui::Text("End Time");
                                                ImGui::NextColumn();

                                            }

                                            for (auto& frameSkip : Entrance.mFrameSkips)
                                            {
                                                Widget::Time(("##FrameSkipsStart##Counting" + std::to_string(idx2)).c_str(),
                                                    frameSkip.first, lineHeight * 4);

                                                ImGui::NextColumn();

                                                Widget::Time(("##FrameSkipsEnd##Counting" + std::to_string(idx2)).c_str(),
                                                    frameSkip.second, lineHeight * 4);

                                                ImGui::SameLine();

                                                if (ImGui::Button(("-##RemoveFrameSkip##Counting" + std::to_string(idx2)).c_str(), ImVec2{ lineHeight, 0 }))
                                                {
                                                    CommandHistory::execute(std::make_unique<VectorEraseCommand<std::pair<std::string, std::string>>>(&Entrance.mFrameSkips, idx2));
                                                    break;
                                                }
                                                ImGui::NextColumn();
                                                idx2++;
                                            }
                                            ImGui::Columns(1);


                                            ImGui::Separator();
                                            idx2 = 0;

                                            if (Entrance.mBlankedVideos.empty())
                                            {
                                                if (ImGui::Button(("+##AddBlankVideo##Counting" + std::to_string(idx)).c_str(), ImVec2{ lineHeight, 0 }))
                                                {
                                                    CommandHistory::execute(std::make_unique<PushBackCommand<std::pair<bool, std::string>>>(&Entrance.mBlankedVideos, std::pair<bool, std::string>(false, "00:00:00")));
                                                }
                                            }
                                            else
                                            {
                                                if (ImGui::Button(("-##RemoveBlankVideo##Counting" + std::to_string(idx2)).c_str(), ImVec2{ lineHeight, 0 }))
                                                {
                                                    CommandHistory::execute(std::make_unique<VectorEraseCommand<std::pair<bool, std::string>>>(&Entrance.mBlankedVideos, 0));
                                                }
                                            }

                                            ImGui::SameLine();
                                            ImGui::PushFont(ImGuiManager::BoldFont);
                                            ImGui::Text("Blank Videos");
                                            ImGui::PopFont();


                                            if (!Entrance.mBlankedVideos.empty())
                                            {
                                                if (Entrance.mBlankedVideos[0].first)
                                                {
                                                    if (ImGui::Button("Start##CountingBlankedVideos", { lineHeight * 2,0 }))
                                                    {
                                                        CommandHistory::execute(std::make_unique<ModifyPropertyCommand<bool>>(&Entrance.mBlankedVideos[0].first, Entrance.mBlankedVideos[0].first, false));
                                                    }
                                                }
                                                else
                                                {
                                                    if (ImGui::Button("End##CountingBlankedVideos", { lineHeight * 2,0 }))
                                                    {
                                                        CommandHistory::execute(std::make_unique<ModifyPropertyCommand<bool>>(&Entrance.mBlankedVideos[0].first, Entrance.mBlankedVideos[0].first, true));
                                                    }
                                                }

                                                ImGui::SameLine();
                                                Widget::Time(("##BlankVideoTime##Counting" + std::to_string(idx2)).c_str(),
                                                    Entrance.mBlankedVideos[0].second, lineHeight * 4);

                                            }

                                            ImGui::Separator();
                                            idx2 = 0;

                                            if (ImGui::Button(("+##AddCorruptedTime##Counting" + std::to_string(idx2)).c_str(), ImVec2{ lineHeight, 0 }))
                                            {
                                                CommandHistory::execute(std::make_unique<PushBackCommand<std::string>>(&Entrance.mCorruptedVideos, std::string("")));
                                            }

                                            ImGui::SameLine();
                                            ImGui::PushFont(ImGuiManager::BoldFont);
                                            ImGui::Text("Corrupted Videos");
                                            ImGui::PopFont();

                                            for (auto& corruptedVideo : Entrance.mCorruptedVideos)
                                            {
                                                char buffer[16] = {};
                                                std::memcpy(buffer, corruptedVideo.c_str(), corruptedVideo.size());
                                                ImGui::SetNextItemWidth(lineHeight * 4);
                                                if (ImGui::InputTextWithHint(("##CorruptedName##Counting" + std::to_string(idx2)).c_str(), "Video Name", buffer, 16))
                                                {
                                                    std::string newText = buffer;
                                                    CommandHistory::execute(std::make_unique<ModifyPropertyCommand<std::string>>(&corruptedVideo, corruptedVideo, newText));
                                                }


                                                ImGui::SameLine();
                                                if (ImGui::Button(("-##RemoveCorruptedVideo##Counting" + std::to_string(idx2)).c_str(), ImVec2{ lineHeight, 0 }))
                                                {
                                                    CommandHistory::execute(std::make_unique<VectorEraseCommand<std::string>>(&Entrance.mCorruptedVideos, idx2));
                                                    break;
                                                }
                                                idx2++;
                                            }

                                            ImGui::Separator();
                                            ImGui::PushFont(ImGuiManager::BoldFont);
                                            ImGui::Text("Additional Notes");
                                            ImGui::PopFont();
                                            char buffer[256] = {};
                                            std::memcpy(buffer, Entrance.mAdditionalNotes.c_str(), Entrance.mAdditionalNotes.size());
                                            if (ImGui::InputTextMultiline("##NotesCounting", buffer, IM_ARRAYSIZE(buffer), ImVec2(ImGui::GetContentRegionAvail().x, lineHeight * 5)))
                                            {
                                                std::string newText = buffer;
                                                CommandHistory::execute(std::make_unique<ModifyPropertyCommand<std::string>>(&Entrance.mAdditionalNotes, Entrance.mAdditionalNotes, newText));
                                            }

                                            ImGui::EndChild();
                                        }
                                        idx++;
                                    }
                                }
                                ImGui::Unindent(lineHeight);
                                ImGui::EndTabItem();

                            }
                            if (!hour2Bool)
                            {
                                ImGui::OpenPopup(("Remove Hour##Modal" + std::to_string(hour)).c_str());
                            }
                            {
                                ImVec2 center = ImGui::GetWindowViewport()->Pos;
                                center.x += ImGui::GetWindowViewport()->Size.x * 0.5f;
                                center.y += ImGui::GetWindowViewport()->Size.y * 0.5f;
                                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
                            }
                            if (ImGui::BeginPopupModal(("Remove Hour##Modal" + std::to_string(hour)).c_str(), NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
                            {
                                ImGui::Text("Are you sure you want to remove this hour?");

                                float spacing = ImGui::GetStyle().ItemSpacing.x;
                                float totalWidth = lineHeight * 4 + spacing;
                                float windowWidth = ImGui::GetWindowSize().x;
                                float startX = (windowWidth - totalWidth) * 0.5f;
                                ImGui::Spacing();
                                ImGui::SetCursorPosX(startX);
                                if (ImGui::Button("No##ClearDataModal", { lineHeight * 2, lineHeight }))
                                {
                                    ImGui::CloseCurrentPopup();
                                }
                                ImGui::SameLine();
                                if (ImGui::Button("Yes##ClearDataModal", { lineHeight * 2, lineHeight }))
                                {
                                    if (mCountingData[StoreCode].size() == 1)
                                    {
                                        CommandHistory::execute(std::make_unique<EraseKeyCommand<std::map<Project::StoreCode, std::map<Project::Hour, CountData>>>>(&mCountingData, mCountingPage.mStorePage));
                                    }
                                    else
                                        CommandHistory::execute(std::make_unique<EraseKeyCommand<std::map<Project::Hour,CountData>>>(&mCountingData[StoreCode], houridx));
                                    ImGui::CloseCurrentPopup();
                                    ImGui::EndPopup();
                                    break;
                                }
                                ImGui::EndPopup();
                            }
                            houridx++;
                        }
                       
                        ImGui::EndTabBar();
                    }
                }
            }

            ImGui::EndChild();

            ImGui::EndTabItem();
            if (open_error_popup)
            {
                ImGui::OpenPopup("No Project Loaded##Modal");
            }
            {
                ImVec2 center = ImGui::GetWindowViewport()->Pos;
                center.x += ImGui::GetWindowViewport()->Size.x * 0.5f;
                center.y += ImGui::GetWindowViewport()->Size.y * 0.5f;
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            }
            bool projectLoadedModal = true;
            if (ImGui::BeginPopupModal("No Project Loaded##Modal", &projectLoadedModal, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
            {
                ImGui::Text("No Project Loaded!\nPlease create or load a project first.");
                ImGui::EndPopup();
            }

            if (open_clear_popup)
            {
				ImGui::OpenPopup("Clear Data##Modal");
			}
            {
                ImVec2 center = ImGui::GetWindowViewport()->Pos;
                center.x += ImGui::GetWindowViewport()->Size.x * 0.5f;
                center.y += ImGui::GetWindowViewport()->Size.y * 0.5f;
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            }
            if (ImGui::BeginPopupModal("Clear Data##Modal", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
            {
                ImGui::Text("Are you sure you want to clear the data?");

                float spacing = ImGui::GetStyle().ItemSpacing.x;
                float totalWidth = lineHeight * 4 + spacing;
                float windowWidth = ImGui::GetWindowSize().x;
                float startX = (windowWidth - totalWidth) * 0.5f;
                ImGui::Spacing();
                ImGui::SetCursorPosX(startX);
                if (ImGui::Button("No##ClearDataModal", { lineHeight * 2, lineHeight }))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Yes##ClearDataModal", { lineHeight * 2, lineHeight }))
                {
                    CommandHistory::execute(std::make_unique<ClearContainerCommand<std::map<Project::StoreCode, std::map<Project::Hour, CountData>>>>(&mCountingData));
                    mCountingData.clear();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
        ImGui::PopStyleVar();

    }

    void ToolsPanel::AggregateTab()
    {
        bool open_clear_popup = false;
        bool open_error_popup = false;
        bool delete_store_popup = false;
        auto& mAggregateStoreData = mProject->mAggregateStoreData;
        auto open = ImGui::BeginTabItem("Aggregate##Toolsbar");
        float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
        {
            ImGui::BeginTooltip();
            ImGui::Text("Aggregate Tasks");
            ImGui::EndTooltip();
        }
        if (open)
        {
            if (ImGui::ImageButton("Add Entry##Aggregate", Resource(Icon::ADDFILE_ICON)->GetTextureID(), { lineHeight, lineHeight }))
            {
                if (!mProject->IsProjectLoaded())
                {
                    open_error_popup = true;
                }
                else
                    ImGui::OpenPopup("AddEntryPopup##Aggregate");
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
            {
                ImGui::BeginTooltip();
                ImGui::Text("Add Entry");
                ImGui::EndTooltip();
            }

            ImGui::SameLine();

            if (ImGui::ImageButton("Clear##Aggregate", Resource(Icon::CLEAR_ICON)->GetTextureID(), { lineHeight, lineHeight }))
            {
                if (!mProject->IsProjectLoaded())
                {
                    open_error_popup = true;
                }
                else
                    open_clear_popup = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
            {
                ImGui::BeginTooltip();
                ImGui::Text("Clear All Entries");
                ImGui::EndTooltip();
            }

            ImGui::SameLine();

            if (ImGui::ImageButton("Import Data##Aggregate", Resource(Icon::IMPORT_ICON)->GetTextureID(), { lineHeight ,lineHeight }))
            {
                if (!mProject->IsProjectLoaded())
                {
                    open_error_popup = true;
                }
                else
                {
                    auto spikeDipFile = OpenFileDialog("Excel File (*.xlsx)\0*.xlsx\0");
                    if (std::filesystem::exists(spikeDipFile))
                    {
                        ExcelSerialiser serialiser(spikeDipFile);
                        CommandHistory::execute(std::make_unique<ModifyPropertyCommand<std::map<Project::StoreCode, std::map<Project::Hour, AggregateData>>>>(&mProject->mAggregateStoreData, mProject->mAggregateStoreData, serialiser.ImportAggregatorReport()));
                    }
                    else
                    {
                        APP_CORE_ERROR("Spike Dip file does not exist!");
                    }
                }
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
            {
                ImGui::BeginTooltip();
                ImGui::Text("Import Data (.xlsx)");
                ImGui::EndTooltip();
            }
            ImGui::SameLine();

            if (ImGui::ImageButton("Export Data##Aggregate", Resource(Icon::EXPORT_ICON)->GetTextureID(), { lineHeight, lineHeight }))
            {
                if (!mProject->IsProjectLoaded())
                {
                    open_error_popup = true;
                }
                else {
                    ExportAggregateStoreDataAsString(12082025);
                }

            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
            {
                ImGui::BeginTooltip();
                ImGui::Text("Export Data (.xlsx)");
                ImGui::EndTooltip();
            }
            ImGui::SetNextWindowSize({ 270 * ImGuiManager::styleMultiplier, 120 * ImGuiManager::styleMultiplier + 40 }, ImGuiCond_Always);
            if (ImGui::BeginPopup("AddEntryPopup##Aggregate", ImGuiWindowFlags_NoMove))
            {

                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 120 * ImGuiManager::styleMultiplier);
                ImGui::Text("Store Code: ");
                ImGui::NextColumn();
                ImGui::SetNextItemWidth(130 * ImGuiManager::styleMultiplier);
                ImGui::InputText("##Store Code##Aggregate: ", mStoreCodeBuffer, 16);
                ImGui::NextColumn();

                ImGui::Text("Hour: ");
                ImGui::NextColumn();
                ImGui::SetNextItemWidth(130 * ImGuiManager::styleMultiplier);
                ImGui::InputInt("##Hour:##Aggregate ", &mTimeBuffer, 1, 1);
                ImGui::NextColumn();

                ImGui::Text("Entrances: ");
                ImGui::NextColumn();
                ImGui::SetNextItemWidth(130 * ImGuiManager::styleMultiplier);
                ImGui::InputInt("##Entrances##Aggregate: ", &mEntranceBuffer, 1, 1);
                ImGui::Columns(1);
                ImGui::Separator();
                if (ImGui::Button("Confirm"))
                {
                    std::string storeID(mStoreCodeBuffer);
                    if (storeID != "")
                    {
                        std::memset(mStoreCodeBuffer, 0, 16);
                        if (!mAggregateStoreData.contains(storeID))
                        {
                            if (!mAggregateStoreData.empty()) {
                                auto currentIT = mAggregateStoreData.begin();
                                std::advance(currentIT, mAggregatePage.mStorePage);
                                if (currentIT->first > storeID)
                                {
                                    mAggregatePage.mStorePage++;
                                }
                            }
                        }
                        CommandHistory::execute(std::make_unique<AddStoreAggregateEntry>(&mAggregateStoreData, storeID, mEntranceBuffer, mTimeBuffer));

                        mEntranceBuffer = 1;
                        mTimeBuffer = 0;
                    }
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SameLine();

                if (ImGui::Button("Cancel##Aggregate"))
                {
                    std::memset(mStoreCodeBuffer, 0, 16);
                    mEntranceBuffer = 1;
                    mTimeBuffer = 0;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            ImVec2 windowSize = ImGui::GetContentRegionAvail();
            ImGui::BeginChild("ScrollableRegion##Aggregate", ImVec2(windowSize.x, windowSize.y), true);
            if (mAggregatePage.mStorePage > 0)
            {
                if (ImGui::ArrowButton("##AggregatePageBack", ImGuiDir_Left))
                {

                    CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int>>(&mAggregatePage.mStorePage, mAggregatePage.mStorePage, mAggregatePage.mStorePage - 1));

                }
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("Previous Store");
                    ImGui::EndTooltip();
                }
            }
            else
            {
                ImGui::InvisibleButton("##DummyArrow1", ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize()));
            }

            ImGui::SameLine();

            float spacing = ImGui::GetStyle().ItemSpacing.x;
            float arrow_button_width = ImGui::GetFrameHeight(); // Arrow buttons are square
            float total_spacing = spacing * 3; // space between 3 items

            // Calculate remaining width
            float remaining_width = ImGui::GetContentRegionAvail().x;
            float middle_button_width = remaining_width - (arrow_button_width * 2 + total_spacing);

            std::string PageNumStr = "NIL";

            if (!mProject->IsProjectLoaded()) PageNumStr = "No Project Loaded";
            else
                if (!mAggregateStoreData.empty())
                {
                    if (mAggregatePage.mStorePage >= mAggregateStoreData.size())
                    {
                        mAggregatePage.mStorePage = (int)mAggregateStoreData.size() - 1;
                    }

                    auto StorePageIT = mAggregateStoreData.begin();
                    std::advance(StorePageIT, mAggregatePage.mStorePage);
                    PageNumStr = (StorePageIT->first);
                }

            std::vector<std::string> keys;

            // Iterate over the map and push the keys into the vector
            for (const auto& pair : mAggregateStoreData) {
                keys.push_back(pair.first);  // pair.first is the key
            }

            ImGui::SetNextItemWidth(middle_button_width);

            // Save cursor before drawing label so we can restore it later
            ImVec2 cursor_before_label = ImGui::GetCursorScreenPos();

            if (ImGui::BeginCombo("##AggregateStoreList", "", ImGuiComboFlags_None))
            {
                for (int i = 0; i < keys.size(); i++)
                {
                    bool is_selected = mAggregatePage.mStorePage == i;
                    if (ImGui::Selectable(keys[i].c_str(), &is_selected))
                        CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int>>(&mAggregatePage.mStorePage, mAggregatePage.mStorePage, i));

                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::PushFont(ImGuiManager::BoldFont);
            if (!keys.empty())
            {
                // Center the label manually
                ImVec2 combo_pos = ImGui::GetItemRectMin(); // Position of combo box
                ImVec2 combo_size = ImGui::GetItemRectSize();
                const char* label = keys[mAggregatePage.mStorePage].c_str();
                ImVec2 text_size = ImGui::CalcTextSize(label);

                ImVec2 text_pos = ImVec2(
                    combo_pos.x + (combo_size.x - text_size.x) * 0.5f,
                    combo_pos.y + (combo_size.y - text_size.y) * 0.5f
                );

                ImGui::SetCursorScreenPos(text_pos);
                ImGui::TextUnformatted(label);

                // Restore cursor after drawing the centered text
                ImGui::SetCursorScreenPos(cursor_before_label);
                ImGui::Dummy(combo_size);  // Reserve the space for the combo box
            }
            else
            {
                ImVec2 combo_pos = ImGui::GetItemRectMin(); // Position of combo box
                ImVec2 combo_size = ImGui::GetItemRectSize();
                const char* label = PageNumStr.c_str();
                ImVec2 text_size = ImGui::CalcTextSize(label);

                ImVec2 text_pos = ImVec2(
                    combo_pos.x + (combo_size.x - text_size.x) * 0.5f,
                    combo_pos.y + (combo_size.y - text_size.y) * 0.5f
                );

                ImGui::SetCursorScreenPos(text_pos);
                ImGui::TextUnformatted(label);

                // Restore cursor after drawing the centered text
                ImGui::SetCursorScreenPos(cursor_before_label);
                ImGui::Dummy(combo_size);  // Reserve the space for the combo box
            }
            ImGui::PopFont();


            // Ensure SameLine aligns the arrow correctly
            ImGui::SameLine();  // This forces the next item to be on the same line


            if (mAggregatePage.mStorePage + 1 < mAggregateStoreData.size())
            {
                if (ImGui::ArrowButton("##AggregatePageNext", ImGuiDir_Right))
                {

                    CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int>>(&mAggregatePage.mStorePage, mAggregatePage.mStorePage, mAggregatePage.mStorePage + 1));
                }
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("Next Store");
                    ImGui::EndTooltip();
                }
            }
            else
            {
                ImGui::InvisibleButton("##DummyArrow2", ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize()));
            }

            if (!mAggregateStoreData.empty())
            {
                ImGui::SameLine();
                if (ImGui::ImageButton("##AggregatePageSettings", Resource(Icon::SETTINGS_ICON)->GetTextureID(), { ImGui::GetFontSize(), ImGui::GetFontSize() }))
                {
                    ImGui::OpenPopup("StoreSettings##Aggregate");
                }
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("Store Settings");
                    ImGui::EndTooltip();
                }
            }

            if (!mAggregateStoreData.empty())
            {
                auto StorePageIT = mAggregateStoreData.begin();
                std::advance(StorePageIT, mAggregatePage.mStorePage);
                auto& StorePageITData = *StorePageIT;
                auto StoreCodeFromIT = StorePageITData.first;
                auto& TimeDataFromIT = StorePageITData.second;



                if (ImGui::BeginPopup("StoreSettings##Aggregate", ImGuiWindowFlags_NoMove))
                {
                    if (ImGui::MenuItem("Add New Entrance##AggregateStoreSettings"))
                    {
                        for (auto& [time, data] : mAggregateStoreData[StoreCodeFromIT])
                        {
                            data.Entrance.push_back({});
                        }
                    }
                    ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false);
                    if (ImGui::MenuItem("Add New Time##AggregateStoreSettings"))
                    {
                        ImGui::OpenPopup("AddTimePopUp##Aggregate");
                    }
                    ImGui::PopItemFlag();

                    ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false);
                    if (ImGui::MenuItem("Remove This Store##AggregateStoreSettings"))
                    {
                        ImGui::OpenPopup("Remove Store##Aggregate");
                    }
                    ImGui::PopItemFlag();

                    ImGui::Separator();

                    if (ImGui::MenuItem("Close##Aggregate"))
                        ImGui::CloseCurrentPopup();

                    {
                        bool closePopup = false;
                        if (ImGui::BeginPopup("AddTimePopUp##Aggregate", ImGuiWindowFlags_NoMove))
                        {
                            ImGui::Text("Hour: ");
                            ImGui::SameLine();
                            ImGui::InputInt("##Hour:##AggregateTimePopup ", &mTimeBuffer, 1, 1, ImGuiInputTextFlags_CharsDecimal);
                            if (ImGui::Button("Confirm##AddTimePopup##Aggregate"))
                            {
                                if (mTimeBuffer >= 0 && mTimeBuffer < 24)
                                {
                                    if (!mAggregateStoreData[StoreCodeFromIT].contains(mTimeBuffer))
                                    {
                                        auto entranceNum = mAggregateStoreData[StoreCodeFromIT].begin()->second.Entrance.size();

                                        mAggregateStoreData[StoreCodeFromIT][mTimeBuffer] = {};
                                        for (int i = 0; i < entranceNum; i++)
                                        {
                                            mAggregateStoreData[StoreCodeFromIT][mTimeBuffer].Entrance.push_back({});
                                        }
                                    }
                                    mTimeBuffer = 0;
                                }
                                closePopup = true;
                                ImGui::CloseCurrentPopup();
                            }

                            if (ImGui::Button("Cancel##AddTimePopup##Aggregate"))
                            {
                                ImGui::CloseCurrentPopup();
                                mTimeBuffer = 0;
                            }

                            ImGui::EndPopup();
                        }

                        if (closePopup)
                        {
                            ImGui::CloseCurrentPopup();
                        }
                    }

                    {
                        bool closePopup = false;
                        {
                            ImVec2 center = ImGui::GetWindowViewport()->Pos;
                            center.x += ImGui::GetWindowViewport()->Size.x * 0.5f;
                            center.y += ImGui::GetWindowViewport()->Size.y * 0.5f;
                            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
                        }
                        if (ImGui::BeginPopupModal("Remove Store##Aggregate", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize))
                        {
                            ImGui::Text("Are you sure you want to remove this store?");

                            float spacing = ImGui::GetStyle().ItemSpacing.x;
                            float totalWidth = lineHeight * 4 + spacing;
                            float windowWidth = ImGui::GetWindowSize().x;
                            float startX = (windowWidth - totalWidth) * 0.5f;
                            ImGui::Spacing();
                            ImGui::SetCursorPosX(startX);
                            if (ImGui::Button("No##RemoveStoreModal", { lineHeight * 2, lineHeight }))
                            {
                                ImGui::CloseCurrentPopup();
                            }
                            ImGui::SameLine();
                            if (ImGui::Button("Yes##RemoveStoreModal", { lineHeight * 2, lineHeight }))
                            {
                                CommandHistory::execute(std::make_unique<EraseKeyCommand<std::map<Project::StoreCode, std::map<Project::Hour, AggregateData>>>>(&mAggregateStoreData, mAggregatePage.mStorePage));
                                if (mAggregatePage.mStorePage != 0)
                                {
                                    mAggregatePage.mStorePage--;
                                }
                                //CommandHistory::execute(std::make_unique<EraseKeyIteratorCommand<std::map<Project::StoreCode, std::map<Project::Hour, AggregateData>>>>(&mAggregateStoreData, &StorePageIT));
                               // if (StorePageIT != mAggregateStoreData.begin())
                                {
                                    //    std::advance(StorePageIT, -1);
                                    //    mAggregatePage.mStorePage -= 1;
                                }

                                closePopup = true;
                                ImGui::CloseCurrentPopup();
                            }

                            ImGui::EndPopup();
                        }

                        if (closePopup)
                        {
                            ImGui::CloseCurrentPopup();
                        }
                    }

                    ImGui::EndPopup();
                }




                if (!mAggregateStoreData.empty())
                {
                    auto StorePageITPostOp = mAggregateStoreData.begin();
                    std::advance(StorePageITPostOp, mAggregatePage.mStorePage);

                    auto StoreCode = StorePageITPostOp->first;

                    if (ImGui::BeginTabBar("##AggregateTabBar"))
                    {
                        int houridx = 0;
                        for (auto& [hour, Data] : mAggregateStoreData[StoreCode])
                        {
                            std::string hourText = "        ";
                            if (hour >= 10)
                                hourText += std::to_string(hour) + "hrs  ";
                            else
                                hourText += "0" + std::to_string(hour) + "hrs  ";

                            bool hour2Bool = true;
                            if (ImGui::BeginTabItem((hourText + "##Aggregate").c_str(), &hour2Bool, ImGuiTabItemFlags_NoReorder))
                            {
                                mAggregatePage.mHourPage = houridx;

                                auto hourIT = mAggregateStoreData[StoreCode].begin();
                                std::advance(hourIT, mAggregatePage.mHourPage);
                                auto& Hour = hourIT->first;
                                auto& Data = hourIT->second;


                                if (ImGui::CollapsingHeader("Statistics##Aggregate", ImGuiTreeNodeFlags_DefaultOpen))
                                {
                                    ImGui::Indent(lineHeight);
                                    ImGui::Columns(2);
                                    {
                                        ImGui::PushFont(ImGuiManager::BoldFont);
                                        ImGui::Text("Customer: ");
                                        ImGui::PopFont();
                                        ImGui::NextColumn();
                                        auto buffer = Data.mCustomer;
                                        ImGui::SetNextItemWidth(lineHeight * 2);

                                        if (ImGui::InputInt("##Customer##Aggregate", &buffer, 0, 0, ImGuiInputTextFlags_CharsDecimal))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mCustomer, Data.mCustomer, buffer));
                                        }

                                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4,4 });
                                        ImGui::SameLine();
                                        if (ImGui::Button("-##Customer##Aggregate", { lineHeight, 0 }))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mCustomer, Data.mCustomer, Data.mCustomer - 1));
                                        }
                                        ImGui::SameLine();
                                        if (ImGui::Button("+##Customer##Aggregate", { lineHeight, 0 }))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mCustomer, Data.mCustomer, Data.mCustomer + 1));
                                        }
                                        ImGui::PopStyleVar();
                                        ImGui::NextColumn();
                                    }

                                    ImGui::Columns(1);
                                    ImGui::Unindent(lineHeight);

                                }

                                auto notesOpen = ImGui::CollapsingHeader("Notes##Aggregate", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen);
                                ImGui::Indent(lineHeight);
                                if (notesOpen)
                                {
                                    int idx = 1;
                                    for (auto& Entrance : Data.Entrance)
                                    {

                                        auto entranceOpen = ImGui::CollapsingHeader(("Entrance " + std::to_string(idx) + "##Aggregate").c_str(), ImGuiTreeNodeFlags_AllowItemOverlap);
                                        if (Data.Entrance.size() > 1)
                                        {
                                            ImGui::SameLine(ImGui::GetContentRegionAvail().x + lineHeight * 0.5f); // Align to right (Button)


                                            if (ImGui::Button(("-##MinusEntranceAggregate" + std::to_string(idx)).c_str(), { lineHeight, 0 }))
                                            {
                                                CommandHistory::execute(std::make_unique<VectorEraseCommand<AggregateEntrance>>(&Data.Entrance, idx - 1));
                                                break;
                                            }

                                        }
                                        if (entranceOpen)
                                        {
                                            ImGui::SetNextWindowSizeConstraints({ ImGui::GetContentRegionAvail().x , lineHeight * 20 }, { ImGui::GetContentRegionAvail().x , lineHeight * 20 });
                                            ImGui::BeginChild(("Entrance" + std::to_string(idx) + "##AggregateChild").c_str(), {}, ImGuiChildFlags_Border);
                                            int idx2 = 0;


                                            ImGui::NewLine();

                                            if (ImGui::Button(("+##AddFrameSkip##Aggregate" + std::to_string(idx)).c_str(), ImVec2{ lineHeight, 0 }))
                                            {

                                                CommandHistory::execute(std::make_unique<PushBackCommand<std::pair<std::string, std::string>>>(&Entrance.mFrameSkips, std::pair<std::string, std::string>("00:00:00", "00:00:00")));
                                            }

                                            ImGui::SameLine();
                                            ImGui::PushFont(ImGuiManager::BoldFont);
                                            ImGui::Text("Frame Skips");
                                            ImGui::PopFont();

                                            ImGui::Columns(2);
                                            ImGui::SetColumnWidth(0, lineHeight * 5);
                                            if (!Entrance.mFrameSkips.empty())
                                            {
                                                ImGui::Text("Start Time");
                                                ImGui::NextColumn();
                                                ImGui::Text("End Time");
                                                ImGui::NextColumn();

                                            }

                                            for (auto& frameSkip : Entrance.mFrameSkips)
                                            {
                                                Widget::Time(("##FrameSkipsStart##Aggregate" + std::to_string(idx2)).c_str(),
                                                    frameSkip.first, lineHeight * 4);

                                                ImGui::NextColumn();

                                                Widget::Time(("##FrameSkipsEnd##Aggregate" + std::to_string(idx2)).c_str(),
                                                    frameSkip.second, lineHeight * 4);

                                                ImGui::SameLine();

                                                if (ImGui::Button(("-##RemoveFrameSkip##Aggregate" + std::to_string(idx2)).c_str(), ImVec2{ lineHeight, 0 }))
                                                {
                                                    CommandHistory::execute(std::make_unique<VectorEraseCommand<std::pair<std::string, std::string>>>(&Entrance.mFrameSkips, idx2));
                                                    break;
                                                }
                                                ImGui::NextColumn();
                                                idx2++;
                                            }
                                            ImGui::Columns(1);


                                            ImGui::Separator();
                                            idx2 = 0;

                                            if (Entrance.mBlankedVideos.empty())
                                            {
                                                if (ImGui::Button(("+##AddBlankVideo##Aggregate" + std::to_string(idx)).c_str(), ImVec2{ lineHeight, 0 }))
                                                {
                                                    CommandHistory::execute(std::make_unique<PushBackCommand<std::pair<bool, std::string>>>(&Entrance.mBlankedVideos, std::pair<bool, std::string>(false, "00:00:00")));
                                                }
                                            }
                                            else
                                            {
                                                if (ImGui::Button(("-##RemoveBlankVideo##Aggregate" + std::to_string(idx2)).c_str(), ImVec2{ lineHeight, 0 }))
                                                {
                                                    CommandHistory::execute(std::make_unique<VectorEraseCommand<std::pair<bool, std::string>>>(&Entrance.mBlankedVideos, 0));
                                                }
                                            }

                                            ImGui::SameLine();
                                            ImGui::PushFont(ImGuiManager::BoldFont);
                                            ImGui::Text("Blank Videos");
                                            ImGui::PopFont();


                                            if (!Entrance.mBlankedVideos.empty())
                                            {
                                                if (Entrance.mBlankedVideos[0].first)
                                                {
                                                    if (ImGui::Button("Start##AggregateBlankedVideos", { lineHeight * 2,0 }))
                                                    {
                                                        CommandHistory::execute(std::make_unique<ModifyPropertyCommand<bool>>(&Entrance.mBlankedVideos[0].first, Entrance.mBlankedVideos[0].first, false));
                                                    }
                                                }
                                                else
                                                {
                                                    if (ImGui::Button("End##AggregateBlankedVideos", { lineHeight * 2,0 }))
                                                    {
                                                        CommandHistory::execute(std::make_unique<ModifyPropertyCommand<bool>>(&Entrance.mBlankedVideos[0].first, Entrance.mBlankedVideos[0].first, true));
                                                    }
                                                }

                                                ImGui::SameLine();
                                                Widget::Time(("##BlankVideoTime##Aggregate" + std::to_string(idx2)).c_str(),
                                                    Entrance.mBlankedVideos[0].second, lineHeight * 4);

                                            }

                                            ImGui::Separator();
                                            idx2 = 0;

                                            if (ImGui::Button(("+##AddCorruptedTime##Aggregate" + std::to_string(idx2)).c_str(), ImVec2{ lineHeight, 0 }))
                                            {
                                                CommandHistory::execute(std::make_unique<PushBackCommand<std::string>>(&Entrance.mCorruptedVideos, std::string("")));
                                            }

                                            ImGui::SameLine();
                                            ImGui::PushFont(ImGuiManager::BoldFont);
                                            ImGui::Text("Corrupted Videos");
                                            ImGui::PopFont();

                                            for (auto& corruptedVideo : Entrance.mCorruptedVideos)
                                            {
                                                char buffer[16] = {};
                                                std::memcpy(buffer, corruptedVideo.c_str(), corruptedVideo.size());
                                                ImGui::SetNextItemWidth(lineHeight * 4);
                                                if (ImGui::InputTextWithHint(("##CorruptedName##Aggregate" + std::to_string(idx2)).c_str(), "Video Name", buffer, 16))
                                                {
                                                    std::string newText = buffer;
                                                    CommandHistory::execute(std::make_unique<ModifyPropertyCommand<std::string>>(&corruptedVideo, corruptedVideo, newText));
                                                }


                                                ImGui::SameLine();
                                                if (ImGui::Button(("-##RemoveCorruptedVideo##Aggregate" + std::to_string(idx2)).c_str(), ImVec2{ lineHeight, 0 }))
                                                {
                                                    CommandHistory::execute(std::make_unique<VectorEraseCommand<std::string>>(&Entrance.mCorruptedVideos, idx2));
                                                    break;
                                                }
                                                idx2++;
                                            }

                                            ImGui::Separator();
                                            ImGui::PushFont(ImGuiManager::BoldFont);
                                            ImGui::Text("Additional Notes");
                                            ImGui::PopFont();
                                            char buffer[256] = {};
                                            std::memcpy(buffer, Entrance.mAdditionalNotes.c_str(), Entrance.mAdditionalNotes.size());
                                            if (ImGui::InputTextMultiline("##NotesAggregate", buffer, IM_ARRAYSIZE(buffer), ImVec2(ImGui::GetContentRegionAvail().x, lineHeight * 5)))
                                            {
                                                std::string newText = buffer;
                                                CommandHistory::execute(std::make_unique<ModifyPropertyCommand<std::string>>(&Entrance.mAdditionalNotes, Entrance.mAdditionalNotes, newText));
                                            }

                                            ImGui::EndChild();
                                        }
                                        idx++;
                                    }
                                }
                                ImGui::Unindent(lineHeight);
                                ImGui::EndTabItem();

                            }
                            if (!hour2Bool)
                            {
                                ImGui::OpenPopup(("Remove Hour##Modal" + std::to_string(hour)).c_str());
                            }
                            {
                                ImVec2 center = ImGui::GetWindowViewport()->Pos;
                                center.x += ImGui::GetWindowViewport()->Size.x * 0.5f;
                                center.y += ImGui::GetWindowViewport()->Size.y * 0.5f;
                                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
                            }
                            if (ImGui::BeginPopupModal(("Remove Hour##Modal" + std::to_string(hour)).c_str(), NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
                            {
                                ImGui::Text("Are you sure you want to remove this hour?");

                                float spacing = ImGui::GetStyle().ItemSpacing.x;
                                float totalWidth = lineHeight * 4 + spacing;
                                float windowWidth = ImGui::GetWindowSize().x;
                                float startX = (windowWidth - totalWidth) * 0.5f;
                                ImGui::Spacing();
                                ImGui::SetCursorPosX(startX);
                                if (ImGui::Button("No##ClearDataModal", { lineHeight * 2, lineHeight }))
                                {
                                    ImGui::CloseCurrentPopup();
                                }
                                ImGui::SameLine();
                                if (ImGui::Button("Yes##ClearDataModal", { lineHeight * 2, lineHeight }))
                                {
                                    if (mAggregateStoreData[StoreCode].size() == 1)
                                    {
                                        CommandHistory::execute(std::make_unique<EraseKeyCommand<std::map<Project::StoreCode, std::map<Project::Hour, CountData>>>>(&mAggregateStoreData, mAggregatePage.mStorePage));
                                    }
                                    else
                                        CommandHistory::execute(std::make_unique<EraseKeyCommand<std::map<Project::Hour, CountData>>>(&mAggregateStoreData[StoreCode], houridx));
                                    ImGui::CloseCurrentPopup();
                                    ImGui::EndPopup();
                                    break;
                                }
                                ImGui::EndPopup();
                            }
                            houridx++;
                        }

                        ImGui::EndTabBar();
                    }
                }
            }



            ImGui::Separator();
            ImGui::Spacing();

            ImGui::EndChild();


            ImGui::EndTabItem();
        }
    }
   
}
