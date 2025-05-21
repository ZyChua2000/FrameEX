/******************************************************************************
/*!
\file       GUIUtils.cpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 20, 2025
\brief      Defines Utility functions for GUI

 /******************************************************************************/

#include <FrameExtractorPCH.hpp>
#include <GUI/GUIUtils.hpp>
#include <Core/Command.hpp>
#include <imgui.h>
namespace FrameExtractor
{


    int Format::FilterNumbersAndColon(ImGuiInputTextCallbackData* data)
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


    bool Format::isValidFormat(const char* buffer) {
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

    std::string Format::fmtTime(int inTime)
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

	void Widget::Time(const char* label, std::string& inText, float itemWidth)
	{
        char buffer[16] = {};
        std::memcpy(buffer, inText.c_str(), inText.size());
        ImGui::SetNextItemWidth(itemWidth);
        if (ImGui::InputText(label, buffer, 16, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCharFilter, Format::FilterNumbersAndColon))
        {
            if (Format::isValidFormat(buffer))
            {
                CommandHistory::execute(std::make_unique<ModifyPropertyCommand<std::string>>(&inText, inText, std::string(buffer)));
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

                        CommandHistory::execute(std::make_unique<ModifyPropertyCommand<std::string>>(&inText, inText, ossTime.str()));
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

                        CommandHistory::execute(std::make_unique<ModifyPropertyCommand<std::string>>(&inText, inText, ossTime.str()));
                    }
                }
            }
        }
	}
    void Widget::InputInt(const char* label, const char* display, int32_t& data)
    {
        ImGui::Text("Customer: ");
        ImGui::SameLine();
        auto buffer = data;
        if (ImGui::InputInt(label, &buffer, 1, 100, ImGuiInputTextFlags_CharsDecimal))
        {
            CommandHistory::execute(std::make_unique<ModifyPropertyCommand<int32_t>>(&data, data, buffer));
        }
    }
}