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
                auto& mAggregateStoreData = mProject->mAggregateStoreData;
                auto open = ImGui::BeginTabItem("Aggregate##Toolsbar");
                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("Aggregate Tasks");
                    ImGui::EndTooltip();
                }
                if (open)
                {
                    ImGui::Columns(4);
                    if (ImGui::Button("Add Entry##Aggregate"))
                    {
                        if (!mProject->IsProjectLoaded())
                        {
                            open_error_popup = true;
                        }
                        else
                            ImGui::OpenPopup("AddEntryPopup##Aggregate");
                    }

                    ImGui::NextColumn();

                    if (ImGui::Button("Clear##Aggregate"))
                    {
                        mAggregateStoreData.clear();
                    }

                    ImGui::NextColumn();

                    if (ImGui::Button("Import Data##Aggregate"))
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
                                mProject->mAggregateStoreData = serialiser.ImportAggregatorReport();
                            }
                            else
                            {
                                APP_CORE_ERROR("Spike Dip file does not exist!");
                            }
                        }
                    }
                    ImGui::NextColumn();

                    if (ImGui::Button("Export Data##Aggregate"))
                    {

                        if (!mProject->IsProjectLoaded())
                        {
                            open_error_popup = true;
                        }
                        else
                        {
                            auto text = ExportAggregateStoreDataAsString();
                            APP_CORE_INFO(text.c_str());
                            CopyToClipboard(text);
                        }

                    }

                    ImGui::Columns(1);
                    if (errorCodeBool)
                        ImGui::SetNextWindowSize({ 284 * ImGuiManager::styleMultiplier, 310 * ImGuiManager::styleMultiplier + 40 }, ImGuiCond_Always);
                    else
                        ImGui::SetNextWindowSize({ 270 * ImGuiManager::styleMultiplier, 260 * ImGuiManager::styleMultiplier + 40 }, ImGuiCond_Always);
                    if (ImGui::BeginPopup("AddEntryPopup##Aggregate", ImGuiWindowFlags_NoMove))
                    {
                        ImGui::Columns(2);
                        ImGui::SetColumnWidth(0, 120 * ImGuiManager::styleMultiplier);
                        ImGui::Text("Shopper ID: ");
                        ImGui::NextColumn();
                        ImGui::SetNextItemWidth(130 * ImGuiManager::styleMultiplier);
                        ImGui::InputText("##Store Code##Aggregate: ", mStoreCodeBuffer, 16);
                        ImGui::NextColumn();

                        ImGui::SetColumnWidth(0, 120 * ImGuiManager::styleMultiplier);
                        ImGui::Text("Store Code: ");
                        ImGui::NextColumn();
                        ImGui::SetNextItemWidth(130 * ImGuiManager::styleMultiplier);
                        ImGui::InputText("##ShopperID##Aggregate: ", shopperIDBuffer, IM_ARRAYSIZE(shopperIDBuffer));
                        ImGui::NextColumn();

                        ImGui::SetColumnWidth(0, 120 * ImGuiManager::styleMultiplier);
                        ImGui::Text("Date: ");
                        ImGui::NextColumn();
                        ImGui::SetNextItemWidth(130 * ImGuiManager::styleMultiplier);
                        ImGui::InputText("##Date##Aggregate: ", dateBuffer, IM_ARRAYSIZE(dateBuffer), ImGuiInputTextFlags_CharsDecimal);
                        ImGui::NextColumn();

                        ImGui::Text("Hour: ");
                        ImGui::NextColumn();
                        ImGui::SetNextItemWidth(130 * ImGuiManager::styleMultiplier);
                        ImGui::InputInt("##Hour:##Aggregate ", &mTimeBuffer, 1, 1);
                        ImGui::NextColumn();

                        ImGui::Text("Enters: ");
                        ImGui::NextColumn();
                        ImGui::SetNextItemWidth(130 * ImGuiManager::styleMultiplier);
                        ImGui::InputInt("##Enters:##Aggregate ", &mEnterBuffer, 1, 1);
                        ImGui::NextColumn();

                        ImGui::Text("Exit: ");
                        ImGui::NextColumn();
                        ImGui::SetNextItemWidth(130 * ImGuiManager::styleMultiplier);
                        ImGui::InputInt("##Exit:##Aggregate ", &mExitBuffer, 1, 1);
                        ImGui::NextColumn();

                        ImGui::Text("Entrances: ");
                        ImGui::NextColumn();
                        ImGui::SetNextItemWidth(130 * ImGuiManager::styleMultiplier);
                        ImGui::InputInt("##Entrances##Aggregate: ", &mEntranceBuffer, 1, 1);
                        ImGui::Columns(1);
                        ImGui::Separator();
                        int Error = ERROR_NONE;
                        if (ImGui::Button("Confirm"))
                        {
                            std::string storeID(mStoreCodeBuffer);
                            if (std::string(dateBuffer) == "" || storeID == "" || std::string(shopperIDBuffer) == "")
                            {
                                errorCodeBool = true;
                                Error = ERROR_MISSINGFIELD;

                            }
                            else if (std::string(dateBuffer).length() < 8)
                            {
                                Error = ERROR_DATEFMT;
                                errorCodeBool = true;
                            }
                            else {
                                int date = std::stoi(dateBuffer);
                                if (storeID != "")
                                {
                                    std::string DD = std::string(dateBuffer).substr(0, 2);
                                    std::string MM = std::string(dateBuffer).substr(2, 2);

                                    int DDInt = std::stoi(DD);
                                    int MMInt = std::stoi(MM);

                                    if (DDInt > 31 || MMInt > 12)
                                    {
                                        Error = ERROR_DATEFMT;
                                        errorCodeBool = true;
                                    }

                                    else
                                    {
                                        std::memset(mStoreCodeBuffer, 0, 16);
                                        if (!mAggregateStoreData.contains(date))
                                        {
                                            mAggregateStoreData[date] = {};
                                        }

                                        if (!mAggregateStoreData[date].contains(storeID))
                                        {
                                            mAggregateStoreData[date][storeID] = {};
                                        }

                                        if (!mAggregateStoreData[date][storeID].empty())
                                        {
                                            if (mEntranceBuffer > mAggregateStoreData[date][storeID].begin()->second.Entrance.size())
                                            {
                                                for (auto& [time, counter] : mAggregateStoreData[date][storeID])
                                                {
                                                    counter.Entrance.resize(mEntranceBuffer, {});
                                                }
                                            }
                                            else
                                            {
                                                mEntranceBuffer = mAggregateStoreData[date][storeID].begin()->second.Entrance.size();
                                            }
                                        }

                                        if (!mAggregateStoreData[date][storeID].contains(mTimeBuffer))
                                        {
                                            mAggregateStoreData[date][storeID][mTimeBuffer] = {};
                                            mAggregateStoreData[date][storeID][mTimeBuffer].StoreID = shopperIDBuffer;
                                            mAggregateStoreData[date][storeID][mTimeBuffer].Enters = mEnterBuffer;
                                            mAggregateStoreData[date][storeID][mTimeBuffer].Exit = mExitBuffer;

                                            std::memset(dateBuffer, 0, 9);
                                            std::memset(shopperIDBuffer, 0, 16);
                                            mEnterBuffer = 0;
                                            mExitBuffer = 0;
                                        }
                                        for (int i = 0; i < mEntranceBuffer; i++)
                                            mAggregateStoreData[date][storeID][mTimeBuffer].Entrance.push_back({});
                                        mEntranceBuffer = 1;
                                        mTimeBuffer = 0;
                                        errorCodeBool = false;
                                        ImGui::CloseCurrentPopup();
                                    }
                                }
                            }
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("Cancel##Aggregate"))
                        {
                            std::memset(mStoreCodeBuffer, 0, 16);
                            mEntranceBuffer = 1;
                            mTimeBuffer = 0;
                            std::memset(dateBuffer, 0, 9);
                            std::memset(shopperIDBuffer, 0, 16);
                            mEnterBuffer = 0;
                            mExitBuffer = 0;
                            ImGui::CloseCurrentPopup();
                            errorCodeBool = false;
                        }

                        if (errorCodeBool == true)
                        {
                            if (Error == ERROR_DATEFMT)
                            {
                                errorLine1 = "Invalid Date Format!";
                                errorLine2 = "Please enter date in DDMMYYYY.";
                            }
                            else if (Error == ERROR_MISSINGFIELD)
                            {
                                errorLine1 = "Please fill all fields!";
                                errorLine2 = "";
                            }
                            ImGui::Separator();
                            ImGui::TextColored({ 1,0.2f,0.2f,1 }, errorLine1.c_str());
                            ImGui::TextColored({ 1,0.2f,0.2f,1 }, errorLine2.c_str());
                        }
                        ImGui::EndPopup();
                    }



                    ImVec2 windowSize = ImGui::GetContentRegionAvail();
                    ImGui::BeginChild("ScrollableRegion##Aggregate", ImVec2(windowSize.x, windowSize.y), true);
                    for (auto& [date, storetimedata] : mAggregateStoreData)
                    {
                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4,4 });
                        std::string dateFmt = std::to_string(date);
                        if (dateFmt.length() < 8)
                        {
                            dateFmt = "0" + dateFmt;
                        }
                        dateFmt.insert(2, "/");
                        dateFmt.insert(5, "/");


                        if (ImGui::CollapsingHeader((dateFmt + "##Aggregate").c_str(), ImGuiTreeNodeFlags_AllowItemOverlap))
                        {
                            float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;

                            ImGui::Indent(lineHeight);
                            for (auto& [store, timeNData] : storetimedata)
                            {
                                auto ContentRegionAvailable = ImGui::GetContentRegionAvail();

                                bool open = ImGui::CollapsingHeader(store.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
                                ImGui::SameLine(ContentRegionAvailable.x + lineHeight * 0.5f); // Align to right (Button)
                                if (ImGui::Button(("+##AddOptions##Aggregate" + store).c_str(), ImVec2{ lineHeight,lineHeight }))
                                {
                                    ImGui::OpenPopup(("AddOptionsPopup##Aggregate" + store).c_str(), ImGuiPopupFlags_NoOpenOverExistingPopup);
                                }

                                if (ImGui::BeginPopup(("AddOptionsPopup##Aggregate" + store).c_str(), ImGuiWindowFlags_NoMove))
                                {
                                    if (ImGui::Button("Add New Time##Aggregate"))
                                    {
                                        ImGui::OpenPopup(("AddTimePopUp##Aggregate" + store).c_str());
                                    }

                                    if (ImGui::Button("Add New Entrance##Aggregate"))
                                    {
                                        for (auto& [time, data] : timeNData)
                                        {
                                            data.Entrance.push_back({});
                                        }
                                        ImGui::CloseCurrentPopup();
                                    }

                                    bool closePopup = false;
                                    if (ImGui::BeginPopup(("AddTimePopUp##Aggregate" + store).c_str(), ImGuiWindowFlags_NoMove))
                                    {
                                        ImGui::InputInt("##Hour:##AggregateTimePopup ", &mTimeBuffer, 1, 1, ImGuiInputTextFlags_CharsDecimal);
                                        if (ImGui::Button("Confirm##AddTimePopup##Counting"))
                                        {
                                            if (mTimeBuffer >= 0 && mTimeBuffer < 24)
                                            {
                                                if (!mAggregateStoreData[date][store].contains(mTimeBuffer))
                                                {
                                                    auto entranceNum = mAggregateStoreData[date][store].begin()->second.Entrance.size();

                                                    mAggregateStoreData[date][store][mTimeBuffer] = {};
                                                    for (int i = 0; i < entranceNum; i++)
                                                    {
                                                        mAggregateStoreData[date][store][mTimeBuffer].Entrance.push_back({});
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

                                    ImGui::Separator();

                                    if (ImGui::Button("X", { lineHeight, lineHeight }))
                                    {
                                        ImGui::CloseCurrentPopup();
                                    }

                                    if (closePopup)
                                    {
                                        ImGui::CloseCurrentPopup();
                                    }

                                    ImGui::EndPopup();
                                }
                                if (open)
                                {
                                    ImGui::Indent(lineHeight);
                                    for (auto& [time, data] : timeNData)
                                    {
                                        if (ImGui::Button(("-##timeMinus##Aggregate" + store + std::to_string(time)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                        {
                                            mAggregateStoreData[date][store].erase(time);
                                            break;
                                        }
                                        if (ImGui::IsItemHovered())
                                        {
                                            ImGui::BeginTooltip();
                                            ImGui::Text("Remove this timing");
                                            ImGui::EndTooltip();
                                        }
                                        ImGui::SameLine();

                                        if (ImGui::CollapsingHeader((Format::fmtTime(time) + "##Aggregate" + store).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                                        {
                                            ImGui::Columns(4);
                                            ImGui::Text("Store ID: ");
                                            ImGui::SameLine();
                                            ImGui::Text(mAggregateStoreData[date][store][time].StoreID.c_str());
                                            ImGui::NextColumn();

                                            ImGui::Text("Enters: ");
                                            ImGui::SameLine();
                                            ImGui::Text(std::to_string(mAggregateStoreData[date][store][time].Enters).c_str());
                                            ImGui::NextColumn();

                                            ImGui::Text("Exits: ");
                                            ImGui::SameLine();
                                            ImGui::Text(std::to_string(mAggregateStoreData[date][store][time].Exit).c_str());
                                            ImGui::Columns(1);

                                            ImGui::Separator();

                                            ImGui::Text("Customer");
                                            ImGui::SameLine();
                                            {
                                                int32_t buffer = mAggregateStoreData[date][store][time].mCustomer;
                                                ImGui::SetNextItemWidth(lineHeight * 4);
                                                if (ImGui::InputInt(("##Customer##Aggregate" + store + std::to_string(time)).c_str(), &buffer, 1, 100, ImGuiInputTextFlags_CharsDecimal))
                                                {
                                                    CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&mAggregateStoreData[date][store][time].mCustomer, mAggregateStoreData[date][store][time].mCustomer, buffer));
                                                }
                                            }
                                                
                                            ImGui::Indent(lineHeight);
                                            for (int entranceNum = 0; entranceNum < mAggregateStoreData[date][store][time].Entrance.size(); entranceNum++)
                                            {
                                                if (ImGui::Button(("-##EntrancesMinus##Aggregate" + store + std::to_string(time) + std::to_string(entranceNum)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                                {
                                                    if (mAggregateStoreData[date][store].begin()->second.Entrance.size() > 1)
                                                    {
                                                        for (auto& [time, counter] : mAggregateStoreData[date][store])
                                                        {
                                                            counter.Entrance.erase(counter.Entrance.begin() + entranceNum);
                                                        }
                                                        break;
                                                    }
                                                }
                                                if (ImGui::IsItemHovered())
                                                {
                                                    ImGui::BeginTooltip();
                                                    ImGui::Text("Remove this entrance");
                                                    ImGui::EndTooltip();
                                                }
                                                ImGui::SameLine();

                                                if (ImGui::CollapsingHeader(("Entrance " + std::to_string(entranceNum + 1) + "##Entrances##Aggregate" + store).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                                                {
                                                    ImGui::Indent(lineHeight);
                                                    if (ImGui::Button(("+##AddFrameSkip##Aggregate" + std::to_string(entranceNum) + std::to_string(time) + store).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                                    {
                                                        mAggregateStoreData[date][store][time].Entrance[entranceNum].mFrameSkips.push_back({ "00:00:00", "00:00:00" });
                                                    }

                                                    ImGui::SameLine();
                                                    ImGui::PushFont(ImGuiManager::BoldFont);
                                                    ImGui::Text("Frame Skips");
                                                    ImGui::PopFont();
                                                    ImGui::Indent(lineHeight * 2);
                                                    int idx = 0;
                                                    if (!mAggregateStoreData[date][store][time].Entrance[entranceNum].mFrameSkips.empty())
                                                    {
                                                        ImGui::Columns(2);
                                                        ImGui::SetColumnWidth(0, lineHeight * 4.5f);
                                                        ImGui::Text("Start Time");
                                                        ImGui::NextColumn();
                                                        ImGui::Text("End Time");
                                                        ImGui::NextColumn();
                                                    }
                                                    for (auto& frameSkip : mAggregateStoreData[date][store][time].Entrance[entranceNum].mFrameSkips)
                                                    {
                                                        Widget::Time(("##FrameSkipBegin##Aggregate" + std::to_string(entranceNum) + std::to_string(idx) + std::to_string(time) + store).c_str(),
                                                            frameSkip.first, lineHeight * 4);

                                                        ImGui::NextColumn();

                                                        Widget::Time(("##FrameSkipEnd##Aggregate" + std::to_string(entranceNum) + std::to_string(idx) + std::to_string(time) + store).c_str(),
                                                            frameSkip.second, lineHeight * 4);

                                                        ImGui::SameLine();
                                                        if (ImGui::Button(("-##RemoveFrameSkip##Aggregate" + std::to_string(entranceNum) + std::to_string(time) + store + std::to_string(idx)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                                        {
                                                            mAggregateStoreData[date][store][time].Entrance[entranceNum].mFrameSkips.erase(mAggregateStoreData[date][store][time].Entrance[entranceNum].mFrameSkips.begin() + idx);
                                                            break;
                                                        }
                                                        ImGui::NextColumn();

                                                        idx++;

                                                    }

                                                    ImGui::Columns(1);
                                                    ImGui::Unindent(lineHeight * 2);

                                                    ImGui::Separator();

                                                    if (ImGui::Button(("+##AddBlankVideo##Aggregate" + std::to_string(entranceNum) + std::to_string(time) + store).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                                    {
                                                        mAggregateStoreData[date][store][time].Entrance[entranceNum].mBlankedVideos.push_back({ "00:00:00" });
                                                    }

                                                    ImGui::SameLine();
                                                    ImGui::PushFont(ImGuiManager::BoldFont);
                                                    ImGui::Text("Blank Videos");
                                                    ImGui::PopFont();
                                                    ImGui::Indent(lineHeight * 2);
                                                    idx = 0;

                                                    if (!mAggregateStoreData[date][store][time].Entrance[entranceNum].mBlankedVideos.empty())
                                                    {
                                                        ImGui::Text("Blank Time");
                                                    }

                                                    for (auto& blankVideo : mAggregateStoreData[date][store][time].Entrance[entranceNum].mBlankedVideos)
                                                    {
                                                        Widget::Time(("##BlankVideoTime##Aggregate" + std::to_string(entranceNum) + std::to_string(idx) + std::to_string(time) + store).c_str(),
                                                            blankVideo, lineHeight * 4);
                                                       
                                                        ImGui::SameLine();
                                                        if (ImGui::Button(("-##RemoveBlankVideo##Aggregate" + std::to_string(entranceNum) + std::to_string(time) + store + std::to_string(idx)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                                        {
                                                            mAggregateStoreData[date][store][time].Entrance[entranceNum].mBlankedVideos.erase(mAggregateStoreData[date][store][time].Entrance[entranceNum].mBlankedVideos.begin() + idx);
                                                            break;
                                                        }
                                                        idx++;

                                                    }

                                                    ImGui::Columns(1);
                                                    ImGui::Unindent(lineHeight * 2);

                                                    ImGui::Separator();

                                                    if (ImGui::Button(("+##AddCorruptedVideo##Aggregate" + std::to_string(entranceNum) + std::to_string(time) + store).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                                    {
                                                        mAggregateStoreData[date][store][time].Entrance[entranceNum].mCorruptedVideos.push_back({ });
                                                    }

                                                    ImGui::SameLine();
                                                    ImGui::PushFont(ImGuiManager::BoldFont);
                                                    ImGui::Text("Corrupted Videos");
                                                    ImGui::PopFont();
                                                    ImGui::Indent(lineHeight * 2);
                                                    idx = 0;
                                                    if (!mAggregateStoreData[date][store][time].Entrance[entranceNum].mCorruptedVideos.empty())
                                                    {
                                                        ImGui::Text("Video Name");
                                                    }

                                                    for (auto& corruptedVideo : mAggregateStoreData[date][store][time].Entrance[entranceNum].mCorruptedVideos)
                                                    {

                                                        char buffer[32] = {};
                                                        std::memcpy(buffer, corruptedVideo.c_str(), corruptedVideo.size());
                                                        ImGui::SetNextItemWidth(lineHeight * 4);
                                                        if (ImGui::InputText(("##CorruptedName##Aggregate" + std::to_string(entranceNum) + std::to_string(idx) + std::to_string(time) + store).c_str(), buffer, 16))
                                                        {
                                                            corruptedVideo = buffer;
                                                        }

                                                        ImGui::SameLine();
                                                        if (ImGui::Button(("-##RemoveCorruptedVideo##Aggregate" + std::to_string(entranceNum) + std::to_string(time) + store + std::to_string(idx)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                                        {
                                                            mAggregateStoreData[date][store][time].Entrance[entranceNum].mCorruptedVideos.erase(mAggregateStoreData[date][store][time].Entrance[entranceNum].mCorruptedVideos.begin() + idx);
                                                            break;
                                                        }
                                                        ImGui::NextColumn();
                                                        idx++;

                                                    }

                                                    ImGui::Unindent(lineHeight * 2);


                                                    ImGui::Unindent(lineHeight);
                                                }
                                            }
                                            ImGui::Unindent(lineHeight);
                                        }

                                    }
                                    ImGui::Unindent(lineHeight);
                                    

                                }

                                if (timeNData.empty())
                                {
                                    storetimedata.erase(store);
                                    break;
                                }
                            }

                            
                            ImGui::Unindent(lineHeight);

                        }
                        ImGui::PopStyleVar();

                        if (storetimedata.empty())
                        {
                            mAggregateStoreData.erase(date);
                            break;
                        }
                    }
                    //AGGREGATE TOOLSET HERE
                    ImGui::EndChild();

                    ImGui::EndTabItem();
                }

            }

            {
                auto open = ImGui::BeginTabItem("Frame Extraction##Toolsbar");
                if (ImGui::IsItemHovered())
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
                if (ImGui::IsItemHovered())
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
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 center = { io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f };

        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize({ 900 * ImGuiManager::styleMultiplier , 600 * ImGuiManager::styleMultiplier }, ImGuiCond_Appearing);
        if (ImGui::BeginPopupModal("No Project Loaded##Modal", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
        {
            ImGui::Text("No Project Loaded!\nPlease create or load a project first.");
            float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
            if (ImGui::Button("X##NoProjectLoadedModal", { lineHeight, lineHeight }))
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

    std::string ToolsPanel::ExportAggregateStoreDataAsString()
    {
        std::stringstream ss;
        ss << "Hi all, Mailers and additional checks done:" << std::endl << "Anu Selma Jose" << std::endl << std::endl;
        for (auto& [date, storetimedata] : mProject->mAggregateStoreData)
        {
            ss << DateIntToStr(date) << ":" << std::endl << std::endl;
            for (auto& [store, timeNData] : storetimedata)
            {
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
                            ss << ", Video blanks after " << blankVideo;
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
            }
            ss << 
                "Spike dip for " << IntDayToSuffix(date / 1000000) << " " << 
                MonthToString((date / 10000) % 100) << " counted" << std::endl;
        }

        return ss.str();
    }

    void ToolsPanel::CountingTab()
    {
        auto& mCountingData = mProject->mCountingData;
        auto open = ImGui::BeginTabItem("Counting##ToolsBar");
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4,4 });
        float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;

        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Counting Tasks (e.g. Spike Dip)");
            ImGui::EndTooltip();
        }
        if (open)
        {

            ImGui::Columns(4);
            if (ImGui::Button("Add Entry##Counting"))
            {
                if (!mProject->IsProjectLoaded())
                {
                   // open_error_popup = true;
                }
                else
                    ImGui::OpenPopup("AddEntryPopup##Counting");
            }

            ImGui::NextColumn();

            if (ImGui::Button("Clear##Counting"))
            {
                mCountingData.clear();
            }

            ImGui::NextColumn();

            if (ImGui::Button("Import Data##Counting"))
            {
                if (!mProject->IsProjectLoaded())
                {
                    //open_error_popup = true;
                }
                else
                {
                    auto spikeDipFile = OpenFileDialog("Excel File (*.xlsx)\0*.xlsx\0");
                    if (std::filesystem::exists(spikeDipFile))
                    {
                        ExcelSerialiser serialiser(spikeDipFile);
                        mProject->mCountingData = serialiser.ImportSpikeDipReport();
                    }
                    else
                    {
                        APP_CORE_ERROR("Spike Dip file does not exist!");
                    }
                }
            }
            ImGui::NextColumn();

            if (ImGui::Button("Export Data##Counting"))
            {
                if (!mProject->IsProjectLoaded())
                {
                    //open_error_popup = true;
                }
                else {
                    auto projectFile = SaveFileDialog("Excel File (*.xlsx)\0*.xlsx\0");
                    projectFile.replace_extension(".xlsx");
                    ExcelSerialiser serialiser(projectFile);
                    serialiser.ExportSpikeDipReport(mProject->mCountingData);
                }

            }

            ImGui::Columns(1);

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
                            mCountingData[storeID] = {};
                        }

                        if (!mCountingData[storeID].empty())
                        {
                            if (mEntranceBuffer > mCountingData[storeID].begin()->second.Entrance.size())
                            {
                                for (auto& [time, counter] : mCountingData[storeID])
                                {
                                    counter.Entrance.resize(mEntranceBuffer, {});
                                }
                            }
                            else
                            {
                                mEntranceBuffer = mCountingData[storeID].begin()->second.Entrance.size();
                            }
                        }

                        if (!mCountingData[storeID].contains(mTimeBuffer))
                        {
                            mCountingData[storeID][mTimeBuffer] = {};
                            for (int i = 0; i < mEntranceBuffer; i++)
                                mCountingData[storeID][mTimeBuffer].Entrance.push_back({});
                        }
                      
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

            if (ImGui::ArrowButton("##CountingPageBack", ImGuiDir_Left))
            {
                if (mCountingPage.mStorePage > 0)
                {
                    mCountingPage.mStorePage--;
                }
           }
            ImGui::SameLine();

            float spacing = ImGui::GetStyle().ItemSpacing.x;
            float arrow_button_width = ImGui::GetFrameHeight(); // Arrow buttons are square
            float total_spacing = spacing * 2; // space between 3 items

            // Calculate remaining width
            float remaining_width = ImGui::GetContentRegionAvail().x;
            float middle_button_width = remaining_width - (arrow_button_width + total_spacing);

            std::string PageNumStr = "NIL";

            if (!mProject->IsProjectLoaded()) PageNumStr = "No Project Loaded";
            else
                if (!mCountingData.empty())
                {
                    if (mCountingPage.mStorePage >= mCountingData.size())
                    {
                        mCountingPage.mDatePage = mCountingData.size() - 1;
                    }
                    auto StorePageIT = mCountingData.begin();
                    std::advance(StorePageIT, mCountingPage.mStorePage);
                    PageNumStr = (StorePageIT->first);
                }


            ImGui::Button(PageNumStr.c_str(), ImVec2(middle_button_width, 0));
            ImGui::SameLine();
            if (ImGui::ArrowButton("##CountingPageNext", ImGuiDir_Right))
            {
                if (mCountingPage.mStorePage < mCountingData.size() - 1)
                    mCountingPage.mStorePage++;
            }
            ImGui::Separator();


            if (!mCountingData.empty())
            {
                auto StorePageIT = mCountingData.begin();
                std::advance(StorePageIT, mCountingPage.mStorePage);

                auto& StoreCode = StorePageIT->first;                
                if (ImGui::BeginTabBar("##CountingTabBar"))
                {
                    int houridx = 0;
                    for (auto& [hour, Data] : StorePageIT->second)
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

                            auto hourIT = StorePageIT->second.begin();
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
                                    if (ImGui::InputInt("##Customer##Counting", &buffer, 1, 1, ImGuiInputTextFlags_CharsDecimal))
                                    {
                                        CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mCustomer, Data.mCustomer, buffer));
                                    }
                                    ImGui::NextColumn();

                                }


                                {
                                    ImGui::PushFont(ImGuiManager::BoldFont);
                                    ImGui::Text("Re-entry Customer: ");
                                    ImGui::PopFont();
                                    ImGui::NextColumn();
                                    auto buffer = Data.mReCustomer;
                                    if (ImGui::InputInt("##ReCustomer##Counting", &buffer, 1, 1, ImGuiInputTextFlags_CharsDecimal))
                                    {
                                        CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mReCustomer, Data.mReCustomer, buffer));
                                    }
                                    ImGui::NextColumn();

                                }

                                {
                                    ImGui::PushFont(ImGuiManager::BoldFont);
                                    ImGui::Text("Suspected Staff: ");
                                    ImGui::PopFont();
                                    ImGui::NextColumn();
                                    auto buffer = Data.mSuspectedStaff;
                                    if (ImGui::InputInt("##SusStaff##Counting", &buffer, 1, 1, ImGuiInputTextFlags_CharsDecimal))
                                    {
                                        CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mSuspectedStaff, Data.mSuspectedStaff, buffer));
                                    }
                                    ImGui::NextColumn();

                                }

                                {
                                    ImGui::PushFont(ImGuiManager::BoldFont);
                                    ImGui::Text("Re-entry Suspected Staff: ");
                                    ImGui::PopFont();
                                    ImGui::NextColumn();
                                    auto buffer = Data.mReSuspectedStaff;
                                    if (ImGui::InputInt("##ReSusStaff##Counting", &buffer, 1, 1, ImGuiInputTextFlags_CharsDecimal))
                                    {
                                        CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mReSuspectedStaff, Data.mReSuspectedStaff, buffer));
                                    }
                                    ImGui::NextColumn();

                                }

                                {
                                    ImGui::PushFont(ImGuiManager::BoldFont);
                                    ImGui::Text("Children: ");
                                    ImGui::PopFont();
                                    ImGui::NextColumn();
                                    auto buffer = Data.mChildren;
                                    if (ImGui::InputInt("##Children##Counting", &buffer, 1, 1, ImGuiInputTextFlags_CharsDecimal))
                                    {
                                        CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mChildren, Data.mChildren, buffer));
                                    }
                                    ImGui::NextColumn();

                                }

                                {
                                    ImGui::PushFont(ImGuiManager::BoldFont);
                                    ImGui::Text("Re-entry Children: ");
                                    ImGui::PopFont();
                                    ImGui::NextColumn();
                                    auto buffer = Data.mReChildren;
                                    if (ImGui::InputInt("##ReChildren##Counting", &buffer, 1, 1, ImGuiInputTextFlags_CharsDecimal))
                                    {
                                        CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mReChildren, Data.mReChildren, buffer));
                                    }
                                    ImGui::NextColumn();

                                }
                                {
                                    ImGui::PushFont(ImGuiManager::BoldFont);
                                    ImGui::Text("Others: ");
                                    ImGui::PopFont();
                                    ImGui::NextColumn();
                                    auto buffer = Data.mOthers;
                                    if (ImGui::InputInt("##Others##Counting", &buffer, 1, 1, ImGuiInputTextFlags_CharsDecimal))
                                    {
                                        CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mOthers, Data.mOthers, buffer));
                                    }
                                    ImGui::NextColumn();

                                }

                                {
                                    ImGui::PushFont(ImGuiManager::BoldFont);
                                    ImGui::Text("Re-entry Others: ");
                                    ImGui::PopFont();
                                    ImGui::NextColumn();
                                    auto buffer = Data.mReOthers;
                                    if (ImGui::InputInt("##ReOthers##Counting", &buffer, 1, 1, ImGuiInputTextFlags_CharsDecimal))
                                    {
                                        CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&Data.mReOthers, Data.mReOthers, buffer));
                                    }
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
                                    auto entranceOpen = ImGui::CollapsingHeader(("Entrance" + std::to_string(idx) + "##Counting").c_str(), ImGuiTreeNodeFlags_AllowItemOverlap);
                                    if (entranceOpen)
                                    {
                                        ImGui::BeginChild(("Entrance" + std::to_string(idx) + "##CountingChild").c_str(), {}, ImGuiChildFlags_Border);
                                        int idx2 = 0;

                                        if (ImGui::Button(("+##AddFrameSkip##Counting" + std::to_string(idx)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                        {
                                            Entrance.mFrameSkips.push_back({ "00:00:00" , "00:00:00" });
                                        }

                                        ImGui::SameLine();
                                        ImGui::PushFont(ImGuiManager::BoldFont);
                                        ImGui::Text("Frame Skips");
                                        ImGui::PopFont();

                                        ImGui::Columns(2);
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

                                            if (ImGui::Button(("-##RemoveFrameSkip##Counting" + std::to_string(idx2)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                            {
                                                Entrance.mFrameSkips.erase(Entrance.mFrameSkips.begin() + idx2);
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
                                            if (ImGui::Button(("+##AddBlankVideo##Counting" + std::to_string(idx)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                            {
                                                Entrance.mBlankedVideos.push_back({ false, "00:00:00" });
                                            }
                                        }
                                        else
                                        {
                                            if (ImGui::Button(("-##RemoveBlankVideo##Counting" + std::to_string(idx2)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                            {
                                                Entrance.mBlankedVideos.clear();
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

                                        if (ImGui::Button(("+##AddCorruptedTime##Counting" + std::to_string(idx2)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                        {
                                            Entrance.mCorruptedVideos.push_back({ "" });
                                        }

                                        ImGui::SameLine();
                                        ImGui::PushFont(ImGuiManager::BoldFont);
                                        ImGui::Text("Corrupted Videos");
                                        ImGui::PopFont();

                                        if (!Entrance.mCorruptedVideos.empty())
                                        {
                                            ImGui::Text("Video Name");
                                        }

                                        for (auto& corruptedVideo : Entrance.mCorruptedVideos)
                                        {
                                            char buffer[16] = {};
                                            std::memcpy(buffer, corruptedVideo.c_str(), corruptedVideo.size());
                                            ImGui::SetNextItemWidth(lineHeight * 4);
                                            if (ImGui::InputText(("##CorruptedName##Counting" + std::to_string(idx2)).c_str(), buffer, 16))
                                            {
                                                std::string newText = buffer;
                                                CommandHistory::execute(std::make_unique<ModifyPropertyCommand<std::string>>(&corruptedVideo, corruptedVideo, newText));
                                            }


                                            ImGui::SameLine();
                                            if (ImGui::Button(("-##RemoveCorruptedVideo##Counting" + std::to_string(idx2)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                            {
                                                Entrance.mCorruptedVideos.erase(Entrance.mCorruptedVideos.begin() + idx2);
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
                            StorePageIT->second.erase(hour);
                            break;
                        }
                        houridx++;
                    }
                    ImGui::EndTabBar();
                }
            }

            ImGui::EndChild();
            ImGui::PopStyleVar();

            ImGui::EndTabItem();
        }
    }
   
}
