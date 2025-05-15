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
#include <GUI/ExplorerPanel.hpp>
#include <GUI/ImGuiManager.hpp>
#include <Core/ExcelSerialiser.hpp>

namespace FrameExtractor
{

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
        ImGui::Text("Store Code: ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100 * ImGuiManager::styleMultiplier);
        ImGui::InputText("##Store Code: ", mStoreCodeBuffer, 16);
        ImGui::SameLine();
        ImGui::Text("Hour: ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100 * ImGuiManager::styleMultiplier);
        ImGui::InputInt("##Hour: ", &mTimeBuffer, 1, 1);
        ImGui::Text("Entrances: ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100 * ImGuiManager::styleMultiplier);
        ImGui::InputInt("##Entrances: ", &mEntranceBuffer, 1, 1);
        ImGui::SameLine();
        if (ImGui::Button("Add Entry"))
        {
            std::string storeID(mStoreCodeBuffer);
            if (storeID != "")
            {
                std::memset(mStoreCodeBuffer, 0, 16);
                if (!mData.contains(storeID))
                {
                    mData[storeID] = {};
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
          
        }
        ImGui::SameLine();
        if (ImGui::Button("Export Data"))
        {
            ExcelSerialiser serialiser("TestPath.xlsx");
            serialiser.ExportSpikeDipReport(mData);
        }

        ImVec2 windowSize = ImGui::GetContentRegionAvail();
        ImGui::BeginChild("ScrollableRegion", ImVec2(windowSize.x, windowSize.y), true);

        for (auto& [store, timeNData] : mData)
        {
            if (ImGui::Button(("+##AddEntrances" + store).c_str()))
            {
                for (auto& [time, entData] : timeNData)
                {
                    entData.Entrance.push_back({});
                }
            }
            ImGui::SameLine();

            if (ImGui::CollapsingHeader(store.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Indent();
                for (auto [time, data] : timeNData)
                {
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
                        ImGui::Indent();
                        for (int32_t entrance = 0; entrance < mData[store][time].Entrance.size(); entrance++)
                        {
                            if (ImGui::CollapsingHeader(("Entrance " + std::to_string(entrance + 1) + "##Entrances" + store).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                            {
                                for (auto entryType = (int)EntryType::ReCustomer; entryType <= ReOthers; entryType++)
                                {
                                    int32_t deleteIdx = -1;
                                    if (ImGui::Button(("+##mTimestamp2" + EntryTypeToString((EntryType)entryType) + std::to_string(time) + store + std::to_string(entrance)).c_str()))
                                    {
                                        mData[store][time].Entrance[entrance][entryType].push_back({});
                                    }
                                    ImGui::SameLine();
                                    ImGui::Text((EntryTypeToString((EntryType)entryType) + " Descriptions").c_str());

                                    ImGui::Indent();
                                    for (int32_t entry = 0; entry < mData[store][time].Entrance[entrance][entryType].size(); entry++)
                                    {
                                        
                                        auto& data = mData[store][time].Entrance[entrance][entryType][entry];
                                        char DescBuffer[128] = {};
                                        char timeStampBuffer[16] = {};
                                        std::memcpy(timeStampBuffer, data.timeStamp.c_str(), data.timeStamp.size());
                                        std::memcpy(DescBuffer, data.Description.c_str(), 128);

                                        if (data.IsMale)
                                        {
                                            if (ImGui::Button(("M##" + EntryTypeToString((EntryType)entryType) + data.timeStamp + std::to_string(time) + store + std::to_string(entry)).c_str()))
                                            {
                                                data.IsMale = false;
                                            }
                                        }
                                        else
                                        {
                                            if (ImGui::Button(("F##" + EntryTypeToString((EntryType)entryType) + data.timeStamp + std::to_string(time) + store + std::to_string(entry)).c_str()))
                                            {
                                                data.IsMale = true;
                                            }
                                        }
                                        ImGui::SameLine();
                                        ImGui::SetNextItemWidth(100 * ImGuiManager::styleMultiplier);
                                        if (ImGui::InputText(("##timestamp" + EntryTypeToString((EntryType)entryType) + data.timeStamp + std::to_string(time) + store + std::to_string(entry)).c_str(), timeStampBuffer, IM_ARRAYSIZE(timeStampBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
                                        {
                                            if (isValidFormat(timeStampBuffer))
                                                data.timeStamp = timeStampBuffer;
                                        }
                                        ImGui::SameLine();
                                        if (ImGui::InputText(("##Description" + EntryTypeToString((EntryType)entryType) + data.timeStamp + std::to_string(time) + store + std::to_string(entry)).c_str(), DescBuffer, 128, ImGuiInputTextFlags_EnterReturnsTrue))
                                        {
                                            data.Description = DescBuffer;
                                        }
                                        ImGui::SameLine();
                                        if (ImGui::Button(("-##" + EntryTypeToString((EntryType)entryType) + data.timeStamp + store + std::to_string(time) + std::to_string(entry)).c_str()))
                                        {
                                            deleteIdx = entrance;
                                        }
                                        
                                    }
                                    ImGui::Unindent();
                                    ImGui::Separator();
                                    if (deleteIdx != -1)
                                    {
                                        mData[store][time].Entrance[entrance][entryType].erase(mData[store][time].Entrance[entrance][entryType].begin() + deleteIdx);
                                    }
                                    
                                }
                                
                            }
                        }
                        
                        ImGui::Unindent();
                      
                    }
                }
                ImGui::Unindent();

            }
            
        }


        ImGui::EndChild();
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
