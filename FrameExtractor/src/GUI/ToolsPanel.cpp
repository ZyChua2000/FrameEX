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
#include <Core/ExcelSerialiser.hpp>
#define ERROR_DATEFMT 1
#define ERROR_MISSINGFIELD 2
#define ERROR_NONE -1
namespace FrameExtractor
{

    int FilterNumbersAndColon(ImGuiInputTextCallbackData* data)
    {
        if (data->EventChar < 256)
        {
            char c = static_cast<char>(data->EventChar);
            if ((c >= '0' && c <= '9') || c == ':')
                return 0; // allow
            return 1;     // block
        }
        return 0;
    }


    bool isValidFormat(const char* buffer) {
        // Expected format is "DD:DD:DD" => length 8
        for (int i = 0; i < 8; ++i) {
            if (i == 2 || i == 5) {
                if (buffer[i] != ':') return false;
            }
            else {
                if (!std::isdigit(static_cast<unsigned char>(buffer[i]))) return false;
            }
        }

        // Ensure the string is exactly 8 characters and not longer
        return buffer[8] == '\0';
    }

    static std::string fmtTime(int inTime)
    {
        if (inTime >= 10)
        {
            return std::to_string(inTime) + "00hrs";
        }
        else
        {
            return "0" + std::to_string(inTime) + "00hrs";
        }
    }
    ToolsPanel::ToolsPanel(Project* project) : mProject(project)
    {


    }

    ToolsPanel::~ToolsPanel()
    {
    }

    void ToolsPanel::OnImGuiRender(float dt)
    {
        ImGui::Begin("Tools");

        if (ImGui::BeginTabBar("##ToolsBar", ImGuiTabBarFlags_Reorderable))
        {
            {
                auto& mCountingData = mProject->mCountingData;
                auto open = ImGui::BeginTabItem("Counting##ToolsBar");
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
                    ImGui::NextColumn();

                    if (ImGui::Button("Export Data##Counting"))
                    {
                        auto projectFile = SaveFileDialog("Excel File (*.xlsx)\0*.xlsx\0");
                        projectFile.replace_extension(".xlsx");

                        if (!projectFile.empty())
                        {
                            ExcelSerialiser serialiser(projectFile);
                            serialiser.ExportSpikeDipReport(mCountingData);
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
                                }
                                for (int i = 0; i < mEntranceBuffer; i++)
                                    mCountingData[storeID][mTimeBuffer].Entrance.push_back({});
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

                    for (auto& [store, timeNData] : mCountingData)
                    {

                        auto ContentRegionAvailable = ImGui::GetContentRegionAvail();
                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4,4 });
                        float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
                        ImGui::Separator();

                        ImGui::SameLine();
                        bool open = ImGui::CollapsingHeader(store.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
                        ImGui::PopStyleVar();
                        ImGui::SameLine(ContentRegionAvailable.x - lineHeight * 0.5f); // Align to right (Button)
                        if (ImGui::Button(("+##AddOptions##Counting" + store).c_str(), ImVec2{ lineHeight,lineHeight }))
                        {
                            ImGui::OpenPopup(("AddOptionsPopup##Counting" + store).c_str(), ImGuiPopupFlags_NoOpenOverExistingPopup);
                        }

                        if (ImGui::BeginPopup(("AddOptionsPopup##Counting" + store).c_str(), ImGuiWindowFlags_NoMove))
                        {
                            if (ImGui::Button("Add New Entrance##Counting"))
                            {
                                for (auto& [time, entData] : timeNData)
                                {
                                    entData.Entrance.push_back({});
                                }
                                ImGui::CloseCurrentPopup();
                            }
                            if (ImGui::Button("Add New Time##Counting"))
                            {
                                ImGui::OpenPopup(("AddTimePopUp##Counting" + store).c_str());
                            }

                            bool closePopup = false;
                            if (ImGui::BeginPopup(("AddTimePopUp##Counting" + store).c_str(), ImGuiWindowFlags_NoMove))
                            {
                                ImGui::InputInt("##Hour:##CountingTimePopup ", &mTimeBuffer, 1, 1, ImGuiInputTextFlags_CharsDecimal);
                                if (ImGui::Button("Confirm##AddTimePopup##Counting"))
                                {
                                    if (mTimeBuffer >= 0 && mTimeBuffer < 24)
                                    {
                                        if (!mCountingData[store].contains(mTimeBuffer))
                                        {
                                            auto entranceNum = mCountingData[store].begin()->second.Entrance.size();

                                            mCountingData[store][mTimeBuffer] = {};
                                            for (int i = 0; i < entranceNum; i++)
                                            {
                                                mCountingData[store][mTimeBuffer].Entrance.push_back({});
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
                                if (ImGui::Button(("-##timeMinus##Counting" + store + std::to_string(time)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                {
                                    mCountingData[store].erase(time);
                                    break;
                                }
                                if (ImGui::IsItemHovered())
                                {
                                    ImGui::BeginTooltip();
                                    ImGui::Text("Remove this timing");
                                    ImGui::EndTooltip();
                                }
                                ImGui::SameLine();

                                if (ImGui::CollapsingHeader((fmtTime(time) + "##Counting" + store).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                                {

                                    ImGui::Columns(2);

                                    ImGui::Text("Customer");
                                    ImGui::NextColumn();
                                    {
                                        int32_t buffer = mCountingData[store][time].mCustomer;
                                        if (ImGui::InputInt(("##Customer##Counting" + store + std::to_string(time)).c_str(), &buffer, 1, 100, ImGuiInputTextFlags_CharsDecimal))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&mCountingData[store][time].mCustomer, mCountingData[store][time].mCustomer, buffer));
                                        }
                                    }
                                    ImGui::NextColumn();

                                    ImGui::Text("Re-Customer");
                                    ImGui::NextColumn();
                                    {
                                        int32_t buffer = mCountingData[store][time].mReCustomer;
                                        if (ImGui::InputInt(("##Customer Re-entry##Counting" + store + std::to_string(time)).c_str(), &buffer, 1, 100 , ImGuiInputTextFlags_CharsDecimal))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&mCountingData[store][time].mReCustomer, mCountingData[store][time].mReCustomer, buffer));
                                        }

                                    }
                                    ImGui::NextColumn();

                                    ImGui::Separator();

                                    ImGui::Text("Suspected Staff");
                                    ImGui::NextColumn();
                                    {
                                        int32_t buffer = mCountingData[store][time].mSuspectedStaff;
                                        if (ImGui::InputInt(("##Suspected Staff##Counting" + store + std::to_string(time)).c_str(), &buffer, 1, 100, ImGuiInputTextFlags_CharsDecimal))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&mCountingData[store][time].mSuspectedStaff, mCountingData[store][time].mSuspectedStaff, buffer));
                                        }
                                    }
                                    ImGui::NextColumn();

                                    ImGui::Text("Re-Suspected Staff");
                                    ImGui::NextColumn();
                                    {
                                        int32_t buffer = mCountingData[store][time].mReSuspectedStaff;
                                        if (ImGui::InputInt(("##Suspected Staff Re-entry##Counting" + store + std::to_string(time)).c_str(), &buffer, 1, 100, ImGuiInputTextFlags_CharsDecimal))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&mCountingData[store][time].mReSuspectedStaff, mCountingData[store][time].mReSuspectedStaff, buffer));
                                        }
                                    }
                                    ImGui::NextColumn();

                                    ImGui::Separator();

                                    ImGui::Text("Children");
                                    ImGui::NextColumn();
                                    {
                                        int32_t buffer = mCountingData[store][time].mChildren;
                                        if (ImGui::InputInt(("##Children##Counting" + store + std::to_string(time)).c_str(), &buffer, 1, 100, ImGuiInputTextFlags_CharsDecimal))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&mCountingData[store][time].mChildren, mCountingData[store][time].mChildren, buffer));
                                        }
                                    }
                                    ImGui::NextColumn();

                                    ImGui::Text("Re-Children");
                                    ImGui::NextColumn();
                                    {
                                        int32_t buffer = mCountingData[store][time].mReChildren;
                                        if (ImGui::InputInt(("##Children Re-entry##Counting" + store + std::to_string(time)).c_str(), &buffer, 1, 100, ImGuiInputTextFlags_CharsDecimal))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&mCountingData[store][time].mReChildren, mCountingData[store][time].mReChildren, buffer));
                                        }
                                    }
                                    ImGui::NextColumn();

                                    ImGui::Separator();

                                    ImGui::Text("Others");
                                    ImGui::NextColumn();
                                    {
                                        int32_t buffer = mCountingData[store][time].mOthers;
                                        if (ImGui::InputInt(("##Others##Counting" + store + std::to_string(time)).c_str(), &buffer, 1, 100, ImGuiInputTextFlags_CharsDecimal))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&mCountingData[store][time].mOthers, mCountingData[store][time].mOthers, buffer));
                                        }
                                    }
                                    ImGui::NextColumn();

                                    ImGui::Text("Re-Others");
                                    ImGui::NextColumn();
                                    {
                                        int32_t buffer = mCountingData[store][time].mReOthers;
                                        if (ImGui::InputInt(("##Others Re-entry##Counting" + store + std::to_string(time)).c_str(), &buffer, 1, 100, ImGuiInputTextFlags_CharsDecimal))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&mCountingData[store][time].mReOthers, mCountingData[store][time].mReOthers, buffer));
                                        }
                                    }
                                    ImGui::NextColumn();

                                    ImGui::Columns(1);
                                    ImGui::Indent(lineHeight);
                                    for (int32_t entrance = 0; entrance < mCountingData[store][time].Entrance.size(); entrance++)
                                    {
                                        if (ImGui::Button(("-##EntrancesMinus##Counting" + store + std::to_string(time) + std::to_string(entrance)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                        {
                                            for (auto& [time, counter] : mCountingData[store])
                                            {
                                                counter.Entrance.erase(counter.Entrance.begin() + entrance);
                                            }
                                            break;
                                        }
                                        if (ImGui::IsItemHovered())
                                        {
                                            ImGui::BeginTooltip();
                                            ImGui::Text("Remove this entrance");
                                            ImGui::EndTooltip();
                                        }
                                        ImGui::SameLine();

                                        if (ImGui::CollapsingHeader(("Entrance " + std::to_string(entrance + 1) + "##Entrances##Counting" + store).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                                        {
                                            ImGui::Indent(lineHeight);
                                            for (auto entryType = (int)EntryType::ReCustomer; entryType <= ReOthers; entryType++)
                                            {
                                                int32_t deleteIdx = -1;
                                                if (ImGui::Button(("+##mTimestamp2##Counting" + EntryTypeToString((EntryType)entryType) + std::to_string(time) + store + std::to_string(entrance)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                                {
                                                    mCountingData[store][time].Entrance[entrance][entryType].push_back({});
                                                }
                                                ImGui::SameLine();
                                                ImGui::Text((EntryTypeToString((EntryType)entryType) + " Descriptions").c_str());

                                                ImGui::Indent(lineHeight);
                                                for (int32_t entry = 0; entry < mCountingData[store][time].Entrance[entrance][entryType].size(); entry++)
                                                {

                                                    auto& data = mCountingData[store][time].Entrance[entrance][entryType][entry];
                                                    char DescBuffer[128] = {};
                                                    char timeStampBuffer[16] = {};
                                                    std::memcpy(timeStampBuffer, data.timeStamp.c_str(), data.timeStamp.size());
                                                    std::memcpy(DescBuffer, data.Description.c_str(), 128);

                                                    if (data.IsMale)
                                                    {
                                                        if (ImGui::Button(("M####Counting" + EntryTypeToString((EntryType)entryType) + data.timeStamp + std::to_string(time) + store + std::to_string(entry)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                                        {
                                                            data.IsMale = false;
                                                        }
                                                    }
                                                    else
                                                    {
                                                        if (ImGui::Button(("F####Counting" + EntryTypeToString((EntryType)entryType) + data.timeStamp + std::to_string(time) + store + std::to_string(entry)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                                        {
                                                            data.IsMale = true;
                                                        }
                                                    }
                                                    ImGui::SameLine();
                                                    ImGui::SetNextItemWidth(80 * ImGuiManager::styleMultiplier);
                                                    if (ImGui::InputText(("##timestamp##Counting" + EntryTypeToString((EntryType)entryType) + data.timeStamp + std::to_string(time) + store + std::to_string(entry)).c_str(),
                                                        timeStampBuffer,
                                                        9,
                                                        ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCharFilter,
                                                        FilterNumbersAndColon
                                                    ))
                                                    {
                                                        if (isValidFormat(timeStampBuffer))
                                                            data.timeStamp = timeStampBuffer;
                                                        else
                                                        {
                                                            std::string timeStampStr(timeStampBuffer);
                                                            if (timeStampStr != "")
                                                            {
                                                                timeStampStr.erase(std::remove(timeStampStr.begin(), timeStampStr.end(), ':'), timeStampStr.end());
                                                                if (timeStampStr.size() > 6)
                                                                {
                                                                    int last2Digits = std::stoi(timeStampStr.substr(timeStampStr.size() - 2));
                                                                    int mid2 = std::stoi(timeStampStr.substr(timeStampStr.size() - 4, 2));
                                                                    int firstDigits = std::stoi(timeStampStr.substr(0, timeStampStr.size() - 4));

                                                                    if (last2Digits >= 60)
                                                                    {
                                                                        last2Digits -= 60;
                                                                        mid2 += 1;
                                                                    }
                                                                    if (mid2 >= 60)
                                                                    {
                                                                        mid2 -= 60;
                                                                        firstDigits += 1;
                                                                    }
                                                                    firstDigits = firstDigits % 24;

                                                                    std::ostringstream ossTime;
                                                                    ossTime << std::setfill('0') << std::setw(2) << firstDigits << ":"
                                                                        << std::setfill('0') << std::setw(2) << mid2 << ":"
                                                                        << std::setfill('0') << std::setw(2) << last2Digits;

                                                                    data.timeStamp = ossTime.str();
                                                                }
                                                                else
                                                                {
                                                                    std::string timeStampStr(timeStampBuffer);

                                                                    int intTime = std::stoi(timeStampBuffer);
                                                                    int last2Digits = intTime % 100;
                                                                    int mid2 = (intTime / 100) % 100;
                                                                    int firstDigits = (intTime / 10000) % 100;
                                                                    if (last2Digits >= 60)
                                                                    {
                                                                        last2Digits -= 60;
                                                                        mid2 += 1;
                                                                    }
                                                                    if (mid2 >= 60)
                                                                    {
                                                                        mid2 -= 60;
                                                                        firstDigits += 1;
                                                                    }
                                                                    firstDigits = firstDigits % 24;

                                                                    std::ostringstream ossTime;
                                                                    ossTime << std::setfill('0') << std::setw(2) << firstDigits << ":"
                                                                        << std::setfill('0') << std::setw(2) << mid2 << ":"
                                                                        << std::setfill('0') << std::setw(2) << last2Digits;
                                                                    data.timeStamp = ossTime.str();
                                                                    APP_CORE_INFO(data.timeStamp.c_str());
                                                                }
                                                            }
                                                        }
                                                    }
                                                    ImGui::SameLine();
                                                    if (ImGui::InputText(("##Description##Counting" + EntryTypeToString((EntryType)entryType) + data.timeStamp + std::to_string(time) + store + std::to_string(entry)).c_str(), DescBuffer, 128, ImGuiInputTextFlags_EnterReturnsTrue))
                                                    {
                                                        data.Description = DescBuffer;
                                                    }
                                                    ImGui::SameLine();
                                                    if (ImGui::Button(("-####Counting" + EntryTypeToString((EntryType)entryType) + data.timeStamp + store + std::to_string(time) + std::to_string(entry)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                                    {
                                                        deleteIdx = entrance;
                                                    }

                                                }
                                                ImGui::Unindent(lineHeight);
                                                ImGui::Separator();
                                                if (deleteIdx != -1)
                                                {
                                                    mCountingData[store][time].Entrance[entrance][entryType].erase(mCountingData[store][time].Entrance[entrance][entryType].begin() + deleteIdx);
                                                }

                                            }
                                            ImGui::Unindent(lineHeight);

                                        }
                                    }

                                    ImGui::Unindent(lineHeight);

                                }
                            }

                            ImGui::Unindent(lineHeight);

                            if (mCountingData[store].empty())
                            {
                                mCountingData.erase(store);
                                break;
                            }
                        }

                    }


                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
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
                        auto spikeDipFile = OpenFileDialog("Excel File (*.xlsx)\0*.xlsx\0");
                        if (std::filesystem::exists(spikeDipFile))
                        {
                           // ExcelSerialiser serialiser(spikeDipFile);
                           // SetData(serialiser.ImportSpikeDipReport());
                        }
                        else
                        {
                            //APP_CORE_ERROR("Spike Dip file does not exist!");
                        }
                    }
                    ImGui::NextColumn();

                    if (ImGui::Button("Export Data##Aggregate"))
                    {
                        auto text = ExportAggregateStoreDataAsString();
                        APP_CORE_INFO(text.c_str());
                        CopyToClipboard(text);

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
                        ImGui::Text("Store Code: ");
                        ImGui::NextColumn();
                        ImGui::SetNextItemWidth(130 * ImGuiManager::styleMultiplier);
                        ImGui::InputText("##Store Code##Aggregate: ", mStoreCodeBuffer, 16);
                        ImGui::NextColumn();

                        ImGui::SetColumnWidth(0, 120 * ImGuiManager::styleMultiplier);
                        ImGui::Text("ShopperID: ");
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
                                            mAggregateStoreData[date][storeID][mTimeBuffer].ShopperTrack_ID = shopperIDBuffer;
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
                        dateFmt.insert(2, "/");
                        dateFmt.insert(5, "/");


                        if (ImGui::CollapsingHeader((dateFmt + "##Aggregate").c_str(), ImGuiTreeNodeFlags_DefaultOpen))
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

                                        if (ImGui::CollapsingHeader((fmtTime(time) + "##Aggregate" + store).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                                        {
                                            ImGui::Columns(4);
                                            ImGui::Text("Shopper ID: ");
                                            ImGui::SameLine();
                                            ImGui::Text(mAggregateStoreData[date][store][time].ShopperTrack_ID.c_str());
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
                                                    for (auto& [time, counter] : mAggregateStoreData[date][store])
                                                    {
                                                        counter.Entrance.erase(counter.Entrance.begin() + entranceNum);
                                                    }
                                                    break;
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

                                                        char buffer[16] = {};
                                                        std::memcpy(buffer, frameSkip.first.c_str(), frameSkip.first.size());
                                                        ImGui::SetNextItemWidth(lineHeight * 4);
                                                        if (ImGui::InputText(("##FrameSkipBegin##Aggregate" + std::to_string(entranceNum) + std::to_string(idx) + std::to_string(time) + store).c_str(), buffer, 16, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCharFilter, FilterNumbersAndColon))
                                                        {
                                                            if (isValidFormat(buffer))
                                                            {
                                                                frameSkip.first = buffer;
                                                            }
                                                            else
                                                            {
                                                                std::string timeStampStr(buffer);
                                                                if (timeStampStr != "")
                                                                {
                                                                    timeStampStr.erase(std::remove(timeStampStr.begin(), timeStampStr.end(), ':'), timeStampStr.end());
                                                                    if (timeStampStr.size() > 6)
                                                                    {
                                                                        int last2Digits = std::stoi(timeStampStr.substr(timeStampStr.size() - 2));
                                                                        int mid2 = std::stoi(timeStampStr.substr(timeStampStr.size() - 4, 2));
                                                                        int firstDigits = std::stoi(timeStampStr.substr(0, timeStampStr.size() - 4));

                                                                        if (last2Digits >= 60)
                                                                        {
                                                                            last2Digits -= 60;
                                                                            mid2 += 1;
                                                                        }
                                                                        if (mid2 >= 60)
                                                                        {
                                                                            mid2 -= 60;
                                                                            firstDigits += 1;
                                                                        }
                                                                        firstDigits = firstDigits % 24;

                                                                        std::ostringstream ossTime;
                                                                        ossTime << std::setfill('0') << std::setw(2) << firstDigits << ":"
                                                                            << std::setfill('0') << std::setw(2) << mid2 << ":"
                                                                            << std::setfill('0') << std::setw(2) << last2Digits;

                                                                        frameSkip.first = ossTime.str();
                                                                    }
                                                                    else
                                                                    {
                                                                        int intTime = std::stoi(buffer);
                                                                        int last2Digits = intTime % 100;
                                                                        int mid2 = (intTime / 100) % 100;
                                                                        int firstDigits = (intTime / 10000) % 100;
                                                                        if (last2Digits >= 60)
                                                                        {
                                                                            last2Digits -= 60;
                                                                            mid2 += 1;
                                                                        }
                                                                        if (mid2 >= 60)
                                                                        {
                                                                            mid2 -= 60;
                                                                            firstDigits += 1;
                                                                        }
                                                                        firstDigits = firstDigits % 24;

                                                                        std::ostringstream ossTime;
                                                                        ossTime << std::setfill('0') << std::setw(2) << firstDigits << ":"
                                                                            << std::setfill('0') << std::setw(2) << mid2 << ":"
                                                                            << std::setfill('0') << std::setw(2) << last2Digits;

                                                                        frameSkip.first = ossTime.str();
                                                                    }
                                                                }
                                                            }
                                                        }

                                                        ImGui::NextColumn();
                                                        char buffer2[16] = {};
                                                        std::memcpy(buffer2, frameSkip.second.c_str(), frameSkip.second.size());
                                                        ImGui::SetNextItemWidth(lineHeight * 4);
                                                        if (ImGui::InputText(("##FrameSkipEnd##Aggregate" + std::to_string(entranceNum) + std::to_string(idx) + std::to_string(time) + store).c_str(), buffer2, 16, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCharFilter, FilterNumbersAndColon))
                                                        {
                                                            if (isValidFormat(buffer2))
                                                            {
                                                                frameSkip.second = buffer2;
                                                            }
                                                            else
                                                            {
                                                                std::string timeStampStr(buffer2);
                                                                if (timeStampStr != "")
                                                                {
                                                                    timeStampStr.erase(std::remove(timeStampStr.begin(), timeStampStr.end(), ':'), timeStampStr.end());
                                                                    if (timeStampStr.size() > 6)
                                                                    {
                                                                        int last2Digits = std::stoi(timeStampStr.substr(timeStampStr.size() - 2));
                                                                        int mid2 = std::stoi(timeStampStr.substr(timeStampStr.size() - 4, 2));
                                                                        int firstDigits = std::stoi(timeStampStr.substr(0, timeStampStr.size() - 4));

                                                                        if (last2Digits >= 60)
                                                                        {
                                                                            last2Digits -= 60;
                                                                            mid2 += 1;
                                                                        }
                                                                        if (mid2 >= 60)
                                                                        {
                                                                            mid2 -= 60;
                                                                            firstDigits += 1;
                                                                        }
                                                                        firstDigits = firstDigits % 24;

                                                                        std::ostringstream ossTime;
                                                                        ossTime << std::setfill('0') << std::setw(2) << firstDigits << ":"
                                                                            << std::setfill('0') << std::setw(2) << mid2 << ":"
                                                                            << std::setfill('0') << std::setw(2) << last2Digits;

                                                                        frameSkip.second = ossTime.str();
                                                                    }
                                                                    else
                                                                    {
                                                                        int intTime = std::stoi(buffer2);
                                                                        int last2Digits = intTime % 100;
                                                                        int mid2 = (intTime / 100) % 100;
                                                                        int firstDigits = (intTime / 10000) % 100;
                                                                        if (last2Digits >= 60)
                                                                        {
                                                                            last2Digits -= 60;
                                                                            mid2 += 1;
                                                                        }
                                                                        if (mid2 >= 60)
                                                                        {
                                                                            mid2 -= 60;
                                                                            firstDigits += 1;
                                                                        }
                                                                        firstDigits = firstDigits % 24;

                                                                        std::ostringstream ossTime;
                                                                        ossTime << std::setfill('0') << std::setw(2) << firstDigits << ":"
                                                                            << std::setfill('0') << std::setw(2) << mid2 << ":"
                                                                            << std::setfill('0') << std::setw(2) << last2Digits;

                                                                        frameSkip.second = ossTime.str();

                                                                    }
                                                                }
                                                            }
                                                        }
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

                                                        char buffer[16] = {};
                                                        std::memcpy(buffer, blankVideo.c_str(), blankVideo.size());
                                                        ImGui::SetNextItemWidth(lineHeight * 4);
                                                        if (ImGui::InputText(("##BlankVideoTime##Aggregate" + std::to_string(entranceNum) + std::to_string(idx) + std::to_string(time) + store).c_str(), buffer, 16, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCharFilter, FilterNumbersAndColon))
                                                        {
                                                            if (isValidFormat(buffer))
                                                            {
                                                                blankVideo = buffer;
                                                            }
                                                            else
                                                            {
                                                                std::string timeStampStr(buffer);
                                                                if (timeStampStr != "")
                                                                {
                                                                    timeStampStr.erase(std::remove(timeStampStr.begin(), timeStampStr.end(), ':'), timeStampStr.end());
                                                                    if (timeStampStr.size() > 6)
                                                                    {
                                                                        int last2Digits = std::stoi(timeStampStr.substr(timeStampStr.size() - 2));
                                                                        int mid2 = std::stoi(timeStampStr.substr(timeStampStr.size() - 4, 2));
                                                                        int firstDigits = std::stoi(timeStampStr.substr(0, timeStampStr.size() - 4));

                                                                        if (last2Digits >= 60)
                                                                        {
                                                                            last2Digits -= 60;
                                                                            mid2 += 1;
                                                                        }
                                                                        if (mid2 >= 60)
                                                                        {
                                                                            mid2 -= 60;
                                                                            firstDigits += 1;
                                                                        }
                                                                        firstDigits = firstDigits % 24;

                                                                        std::ostringstream ossTime;
                                                                        ossTime << std::setfill('0') << std::setw(2) << firstDigits << ":"
                                                                            << std::setfill('0') << std::setw(2) << mid2 << ":"
                                                                            << std::setfill('0') << std::setw(2) << last2Digits;

                                                                        blankVideo = ossTime.str();
                                                                    }
                                                                    else
                                                                    {
                                                                        int intTime = std::stoi(buffer);
                                                                        int last2Digits = intTime % 100;
                                                                        int mid2 = (intTime / 100) % 100;
                                                                        int firstDigits = (intTime / 10000) % 100;
                                                                        if (last2Digits >= 60)
                                                                        {
                                                                            last2Digits -= 60;
                                                                            mid2 += 1;
                                                                        }
                                                                        if (mid2 >= 60)
                                                                        {
                                                                            mid2 -= 60;
                                                                            firstDigits += 1;
                                                                        }
                                                                        firstDigits = firstDigits % 24;

                                                                        std::ostringstream ossTime;
                                                                        ossTime << std::setfill('0') << std::setw(2) << firstDigits << ":"
                                                                            << std::setfill('0') << std::setw(2) << mid2 << ":"
                                                                            << std::setfill('0') << std::setw(2) << last2Digits;

                                                                        blankVideo = ossTime.str();

                                                                    }
                                                                }
                                                            }
                                                        }
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
        ImGui::End();
    }

    const char* ToolsPanel::GetName() const
    {
        return "Tools";
    }
    

    void ToolsPanel::OnAttach()
    {

    }
    std::string ToolsPanel::ExportAggregateStoreDataAsString()
    {
        std::stringstream ss;
        for (auto& [date, storetimedata] : mProject->mAggregateStoreData)
        {
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
                    ss << store << ", " << data.ShopperTrack_ID << ", " << reformattedDate << time << ", " << (int)data.Enters << ", " << (int)data.Exit << " ->" <<
                        data.mCustomer << " Customers";
                    int idx = 1;
                    for (auto& entrance : data.Entrance)
                    {
                        for (auto& frameSkip : entrance.mFrameSkips)
                        {
                            ss << ", Video Skips from: " << frameSkip.first << " to " << frameSkip.second;
                            if (data.Entrance.size() > 1)
                            {
                                ss << "(E" << idx << ")";
                            }
                        }

                        for (auto& blankVideo : entrance.mBlankedVideos)
                        {
                            ss << ", Video blanks after " << blankVideo;
                            if (data.Entrance.size() > 1)
                            {
                                ss << "(E" << idx << ")";
                            }
                        }

                        for (auto& corruptedVideo : entrance.mCorruptedVideos)
                        {
                            ss << ", Video " << corruptedVideo << " is corrupted";
                            if (data.Entrance.size() > 1)
                            {
                                ss << "(E" << idx << ")";
                            }
                        }
                        idx++;
                    }
                    ss << "\n";
                }
            }
        }

        return ss.str();
    }
   
}
