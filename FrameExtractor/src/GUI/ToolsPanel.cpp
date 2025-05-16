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
    ToolsPanel::ToolsPanel()
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
                    if (ImGui::Button("Add Entry"))
                    {
                        ImGui::OpenPopup("AddEntryPopup##");
                    }

                    ImGui::NextColumn();

                    if (ImGui::Button("Clear"))
                    {
                        mData.clear();
                    }

                    ImGui::NextColumn();

                    if (ImGui::Button("Import Data"))
                    {
                        auto spikeDipFile = OpenFileDialog("Excel File (*.xlsx)\0*.xlsx\0");
                        if (std::filesystem::exists(spikeDipFile))
                        {
                            ExcelSerialiser serialiser(spikeDipFile);
                            SetData(serialiser.ImportSpikeDipReport());
                        }
                        else
                        {
                            APP_CORE_ERROR("Spike Dip file does not exist!");
                        }
                    }
                    ImGui::NextColumn();

                    if (ImGui::Button("Export Data"))
                    {
                        auto projectFile = SaveFileDialog("Excel File (*.xlsx)\0*.xlsx\0");
                        projectFile.replace_extension(".xlsx");

                        if (!projectFile.empty())
                        {
                            ExcelSerialiser serialiser(projectFile);
                            serialiser.ExportSpikeDipReport(mData);
                        }
                    }

                    ImGui::Columns(1);

                    ImGui::SetNextWindowSize({ 270 * ImGuiManager::styleMultiplier, 120 * ImGuiManager::styleMultiplier + 40 }, ImGuiCond_Always);
                    if (ImGui::BeginPopup("AddEntryPopup##", ImGuiWindowFlags_NoMove))
                    {

                        ImGui::Columns(2);
                        ImGui::SetColumnWidth(0, 120 * ImGuiManager::styleMultiplier);
                        ImGui::Text("Store Code: ");
                        ImGui::NextColumn();
                        ImGui::SetNextItemWidth(130 * ImGuiManager::styleMultiplier);
                        ImGui::InputText("##Store Code: ", mStoreCodeBuffer, 16);
                        ImGui::NextColumn();

                        ImGui::Text("Hour: ");
                        ImGui::NextColumn();
                        ImGui::SetNextItemWidth(130 * ImGuiManager::styleMultiplier);
                        ImGui::InputInt("##Hour: ", &mTimeBuffer, 1, 1);
                        ImGui::NextColumn();

                        ImGui::Text("Entrances: ");
                        ImGui::NextColumn();
                        ImGui::SetNextItemWidth(130 * ImGuiManager::styleMultiplier);
                        ImGui::InputInt("##Entrances: ", &mEntranceBuffer, 1, 1);
                        ImGui::Columns(1);
                        ImGui::Separator();
                        if (ImGui::Button("Confirm"))
                        {
                            std::string storeID(mStoreCodeBuffer);
                            if (storeID != "")
                            {
                                std::memset(mStoreCodeBuffer, 0, 16);
                                if (!mData.contains(storeID))
                                {
                                    mData[storeID] = {};
                                }

                                if (!mData[storeID].empty())
                                {
                                    if (mEntranceBuffer > mData[storeID].begin()->second.Entrance.size())
                                    {
                                        for (auto& [time, counter] : mData[storeID])
                                        {
                                            counter.Entrance.resize(mEntranceBuffer, {});
                                        }
                                    }
                                    else
                                    {
                                        mEntranceBuffer = mData[storeID].begin()->second.Entrance.size();
                                    }
                                }

                                if (!mData[storeID].contains(mTimeBuffer))
                                {
                                    mData[storeID][mTimeBuffer] = {};
                                }
                                for (int i = 0; i < mEntranceBuffer; i++)
                                    mData[storeID][mTimeBuffer].Entrance.push_back({});
                                mEntranceBuffer = 1;
                                mTimeBuffer = 0;
                            }
                            ImGui::CloseCurrentPopup();
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("Cancel"))
                        {
                            std::memset(mStoreCodeBuffer, 0, 16);
                            mEntranceBuffer = 1;
                            mTimeBuffer = 0;
                            ImGui::CloseCurrentPopup();
                        }

                        ImGui::EndPopup();
                    }


                    ImVec2 windowSize = ImGui::GetContentRegionAvail();
                    ImGui::BeginChild("ScrollableRegion", ImVec2(windowSize.x, windowSize.y), true);

                    for (auto& [store, timeNData] : mData)
                    {

                        auto ContentRegionAvailable = ImGui::GetContentRegionAvail();
                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4,4 });
                        float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
                        ImGui::Separator();

                        ImGui::SameLine();
                        bool open = ImGui::CollapsingHeader(store.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
                        ImGui::PopStyleVar();
                        ImGui::SameLine(ContentRegionAvailable.x - lineHeight * 0.5f); // Align to right (Button)
                        if (ImGui::Button(("+##AddOptions" + store).c_str(), ImVec2{ lineHeight,lineHeight }))
                        {
                            ImGui::OpenPopup(("AddOptionsPopup##" + store).c_str(), ImGuiPopupFlags_NoOpenOverExistingPopup);
                        }

                        if (ImGui::BeginPopup(("AddOptionsPopup##" + store).c_str(), ImGuiWindowFlags_NoMove))
                        {
                            if (ImGui::Button("Add New Entrance"))
                            {
                                for (auto& [time, entData] : timeNData)
                                {
                                    entData.Entrance.push_back({});
                                }
                                ImGui::CloseCurrentPopup();
                            }
                            if (ImGui::Button("Add New Time"))
                            {
                                ImGui::OpenPopup(("AddTimePopUp##" + store).c_str());
                            }

                            bool closePopup = false;
                            if (ImGui::BeginPopup(("AddTimePopUp##" + store).c_str(), ImGuiWindowFlags_NoMove))
                            {
                                ImGui::InputInt("##Hour: ", &mTimeBuffer, 1, 1);
                                if (ImGui::Button("Confirm##AddTimePopup"))
                                {
                                    if (mTimeBuffer >= 0 && mTimeBuffer < 24)
                                    {
                                        if (!mData[store].contains(mTimeBuffer))
                                        {
                                            auto entranceNum = mData[store].begin()->second.Entrance.size();

                                            mData[store][mTimeBuffer] = {};
                                            for (int i = 0; i < entranceNum; i++)
                                            {
                                                mData[store][mTimeBuffer].Entrance.push_back({});
                                            }
                                        }
                                        mTimeBuffer = 0;
                                    }
                                    closePopup = true;
                                    ImGui::CloseCurrentPopup();
                                }

                                if (ImGui::Button("Cancel##AddTimePopup"))
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
                                if (ImGui::Button(("-##timeMinus" + store + std::to_string(time)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                {
                                    mData[store].erase(time);
                                    break;
                                }
                                if (ImGui::IsItemHovered())
                                {
                                    ImGui::BeginTooltip();
                                    ImGui::Text("Remove this timing");
                                    ImGui::EndTooltip();
                                }
                                ImGui::SameLine();

                                if (ImGui::CollapsingHeader((fmtTime(time) + "##" + store).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                                {

                                    ImGui::Columns(2);

                                    ImGui::Text("Customer");
                                    ImGui::NextColumn();
                                    {
                                        int32_t buffer = mData[store][time].mCustomer;
                                        if (ImGui::InputInt(("##Customer" + store + std::to_string(time)).c_str(), &buffer, 1, 100))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&mData[store][time].mCustomer, mData[store][time].mCustomer, buffer));
                                        }
                                    }
                                    ImGui::NextColumn();

                                    ImGui::Text("Re-Customer");
                                    ImGui::NextColumn();
                                    {
                                        int32_t buffer = mData[store][time].mReCustomer;
                                        if (ImGui::InputInt(("##Customer Re-entry" + store + std::to_string(time)).c_str(), &buffer))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&mData[store][time].mReCustomer, mData[store][time].mReCustomer, buffer));
                                        }

                                    }
                                    ImGui::NextColumn();

                                    ImGui::Separator();

                                    ImGui::Text("Suspected Staff");
                                    ImGui::NextColumn();
                                    {
                                        int32_t buffer = mData[store][time].mSuspectedStaff;
                                        if (ImGui::InputInt(("##Suspected Staff" + store + std::to_string(time)).c_str(), &buffer))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&mData[store][time].mSuspectedStaff, mData[store][time].mSuspectedStaff, buffer));
                                        }
                                    }
                                    ImGui::NextColumn();

                                    ImGui::Text("Re-Suspected Staff");
                                    ImGui::NextColumn();
                                    {
                                        int32_t buffer = mData[store][time].mReSuspectedStaff;
                                        if (ImGui::InputInt(("##Suspected Staff Re-entry" + store + std::to_string(time)).c_str(), &buffer))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&mData[store][time].mReSuspectedStaff, mData[store][time].mReSuspectedStaff, buffer));
                                        }
                                    }
                                    ImGui::NextColumn();

                                    ImGui::Separator();

                                    ImGui::Text("Children");
                                    ImGui::NextColumn();
                                    {
                                        int32_t buffer = mData[store][time].mChildren;
                                        if (ImGui::InputInt(("##Children" + store + std::to_string(time)).c_str(), &buffer))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&mData[store][time].mChildren, mData[store][time].mChildren, buffer));
                                        }
                                    }
                                    ImGui::NextColumn();

                                    ImGui::Text("Re-Children");
                                    ImGui::NextColumn();
                                    {
                                        int32_t buffer = mData[store][time].mReChildren;
                                        if (ImGui::InputInt(("##Children Re-entry" + store + std::to_string(time)).c_str(), &buffer))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&mData[store][time].mReChildren, mData[store][time].mReChildren, buffer));
                                        }
                                    }
                                    ImGui::NextColumn();

                                    ImGui::Separator();

                                    ImGui::Text("Others");
                                    ImGui::NextColumn();
                                    {
                                        int32_t buffer = mData[store][time].mOthers;
                                        if (ImGui::InputInt(("##Others" + store + std::to_string(time)).c_str(), &buffer))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&mData[store][time].mOthers, mData[store][time].mOthers, buffer));
                                        }
                                    }
                                    ImGui::NextColumn();

                                    ImGui::Text("Re-Others");
                                    ImGui::NextColumn();
                                    {
                                        int32_t buffer = mData[store][time].mReOthers;
                                        if (ImGui::InputInt(("##Others Re-entry" + store + std::to_string(time)).c_str(), &buffer))
                                        {
                                            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&mData[store][time].mReOthers, mData[store][time].mReOthers, buffer));
                                        }
                                    }
                                    ImGui::NextColumn();

                                    ImGui::Columns(1);
                                    ImGui::Indent(lineHeight);
                                    for (int32_t entrance = 0; entrance < mData[store][time].Entrance.size(); entrance++)
                                    {
                                        if (ImGui::Button(("-##EntrancesMinus" + store + std::to_string(time) + std::to_string(entrance)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                        {
                                            for (auto& [time, counter] : mData[store])
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

                                        if (ImGui::CollapsingHeader(("Entrance " + std::to_string(entrance + 1) + "##Entrances" + store).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                                        {
                                            ImGui::Indent(lineHeight);
                                            for (auto entryType = (int)EntryType::ReCustomer; entryType <= ReOthers; entryType++)
                                            {
                                                int32_t deleteIdx = -1;
                                                if (ImGui::Button(("+##mTimestamp2" + EntryTypeToString((EntryType)entryType) + std::to_string(time) + store + std::to_string(entrance)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                                {
                                                    mData[store][time].Entrance[entrance][entryType].push_back({});
                                                }
                                                ImGui::SameLine();
                                                ImGui::Text((EntryTypeToString((EntryType)entryType) + " Descriptions").c_str());

                                                ImGui::Indent(lineHeight);
                                                for (int32_t entry = 0; entry < mData[store][time].Entrance[entrance][entryType].size(); entry++)
                                                {

                                                    auto& data = mData[store][time].Entrance[entrance][entryType][entry];
                                                    char DescBuffer[128] = {};
                                                    char timeStampBuffer[16] = {};
                                                    std::memcpy(timeStampBuffer, data.timeStamp.c_str(), data.timeStamp.size());
                                                    std::memcpy(DescBuffer, data.Description.c_str(), 128);

                                                    if (data.IsMale)
                                                    {
                                                        if (ImGui::Button(("M##" + EntryTypeToString((EntryType)entryType) + data.timeStamp + std::to_string(time) + store + std::to_string(entry)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                                        {
                                                            data.IsMale = false;
                                                        }
                                                    }
                                                    else
                                                    {
                                                        if (ImGui::Button(("F##" + EntryTypeToString((EntryType)entryType) + data.timeStamp + std::to_string(time) + store + std::to_string(entry)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                                        {
                                                            data.IsMale = true;
                                                        }
                                                    }
                                                    ImGui::SameLine();
                                                    ImGui::SetNextItemWidth(80 * ImGuiManager::styleMultiplier);
                                                    if (ImGui::InputText(("##timestamp" + EntryTypeToString((EntryType)entryType) + data.timeStamp + std::to_string(time) + store + std::to_string(entry)).c_str(),
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
                                                    ImGui::SameLine();
                                                    if (ImGui::InputText(("##Description" + EntryTypeToString((EntryType)entryType) + data.timeStamp + std::to_string(time) + store + std::to_string(entry)).c_str(), DescBuffer, 128, ImGuiInputTextFlags_EnterReturnsTrue))
                                                    {
                                                        data.Description = DescBuffer;
                                                    }
                                                    ImGui::SameLine();
                                                    if (ImGui::Button(("-##" + EntryTypeToString((EntryType)entryType) + data.timeStamp + store + std::to_string(time) + std::to_string(entry)).c_str(), ImVec2{ lineHeight ,lineHeight }))
                                                    {
                                                        deleteIdx = entrance;
                                                    }

                                                }
                                                ImGui::Unindent(lineHeight);
                                                ImGui::Separator();
                                                if (deleteIdx != -1)
                                                {
                                                    mData[store][time].Entrance[entrance][entryType].erase(mData[store][time].Entrance[entrance][entryType].begin() + deleteIdx);
                                                }

                                            }
                                            ImGui::Unindent(lineHeight);

                                        }
                                    }

                                    ImGui::Unindent(lineHeight);

                                }
                            }

                            ImGui::Unindent(lineHeight);

                            if (mData[store].empty())
                            {
                                mData.erase(store);
                                break;
                            }
                        }

                    }


                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
            }

            {
                auto open = ImGui::BeginTabItem("Aggregate##Toolsbar");
                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("Aggregate Tasks");
                    ImGui::EndTooltip();
                }
                if (open)
                {
                    //AGGREGATE TOOLSET HERE
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
    std::string EntryTypeToString(EntryType type)
    {
        switch (type)
        {
        case Customer:
            return "Customer";
        case ReCustomer:
            return "Re-entry Customer";
        case SuspectedStaff:
            return "Suspected Staff";
        case ReSuspectedStaff:
            return "Re-entry Suspected Staff";
        case Children:
            return "Children";
        case ReChildren:
            return "Re-entry Children";
        case Others:
            return "Others";
        case ReOthers:
            return "Re-entry Others";
        default:
            return "Unknown type";
        }
    }
}
