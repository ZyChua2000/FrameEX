/******************************************************************************/
/*!
\file       GUIUtils.cpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 20, 2025
\brief      Defines Utility functions for GUI

******************************************************************************/

#include <FrameExtractorPCH.hpp>
 // Third-party includes
#include <imgui.h>

// Project includes
#include <GUI/GUIUtils.hpp>
#include <Core/Command.hpp>
#include <Template/DataSpecification.hpp>
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

	bool Format::isValidFormat(const char* buffer)
	{
		// Expected format is "DD:DD:DD" => length 8
		for (int i = 0; i < 8; ++i)
		{
			if (i == 2 || i == 5)
			{
				if (buffer[i] != ':') return false;
			}
			else
			{
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

	void Widget::InputTime(const char* label, std::string& inText, float itemWidth)
	{
		char buffer[16] = {};
		std::memcpy(buffer, inText.c_str(), inText.size());
		ImGui::SetNextItemWidth(itemWidth);
		if (ImGui::InputText(label, buffer, 16, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCharFilter, Format::FilterNumbersAndColon))
		{
			if (Format::isValidFormat(buffer))
			{
				inText = std::string(buffer);
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

						inText = ossTime.str();
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

						inText = ossTime.str();
					}
				}
			}
		}
	}

	static std::string to_string(const std::chrono::hh_mm_ss<std::chrono::seconds>& time)
	{
		std::ostringstream oss;
		oss << std::setfill('0')
			<< std::setw(2) << time.hours().count() << ":"
			<< std::setw(2) << time.minutes().count() << ":"
			<< std::setw(2) << time.seconds().count();
		return oss.str();
	}
	static std::chrono::hh_mm_ss<std::chrono::seconds> from_string(const std::string& time_str)
	{
		int h, m, s;
		char sep1, sep2;

		std::istringstream iss(time_str);
		if (!(iss >> h >> sep1 >> m >> sep2 >> s) || sep1 != ':' || sep2 != ':')
		{
			throw std::invalid_argument("Invalid InputTime format, expected hh:mm:ss");
		}

		using namespace std::chrono;
		return hh_mm_ss<seconds>(hours(h) + minutes(m) + seconds(s));
	}
	void Widget::InputTime(const char* label, rttr::variant& inVar, float itemWidth)
	{
		char buffer[16] = {};
		auto inText = inVar.get_value<Time>();
		std::memcpy(buffer, inText.to_string().c_str(), inText.to_string().size());
		ImGui::SetNextItemWidth(itemWidth);
		if (ImGui::InputText(label, buffer, 16, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCharFilter, Format::FilterNumbersAndColon))
		{
			if (Format::isValidFormat(buffer))
			{
				Time t = from_string(buffer);
				inVar = t;
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

						Time t = from_string(ossTime.str());
						inVar = t;
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
						Time t = from_string(ossTime.str());
						inVar = t;
					}
				}
			}
		}
	}
	bool Widget::InputTime(const char* label, Time& inVar, float itemWidth)
	{
		char buffer[16] = {};
		std::memcpy(buffer, inVar.to_string().c_str(), inVar.to_string().size());
		ImGui::SetNextItemWidth(itemWidth);
		if (ImGui::InputText(label, buffer, 16, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCharFilter, Format::FilterNumbersAndColon))
		{
			if (Format::isValidFormat(buffer))
			{
				Time t = from_string(buffer);
				inVar = t;
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

						Time t = from_string(ossTime.str());
						inVar = t;
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
						Time t = from_string(ossTime.str());
						inVar = t;
					}
				}
			}
			return true;
		}
		return false;
	}
	void Widget::InputDate(const char* label, rttr::variant& inDate, float itemWidth)
	{
		char buffer[16] = {};
		auto inText = inDate.get_value<Date>();
		std::memcpy(buffer, inText.to_string().c_str(), inText.to_string().size());
		itemWidth -= ImGui::GetStyle().ItemSpacing.x * 4; // Adjust for spacing between inputs
		itemWidth -= ImGui::GetStyle().FramePadding.x * 2.f; // Adjust for padding on both sides
		itemWidth -= ImGui::CalcTextSize("/").x * 2;
		itemWidth /= 4;
		ImGui::SetNextItemWidth(itemWidth);

		int day = inText.day;
		ImGui::InputInt(("##" + std::string("day") + label).c_str(), &day, 0, 0, ImGuiInputTextFlags_CharsDecimal);
		inText.day = day;
		ImGui::SameLine();

		ImGui::Text("/");
		ImGui::SameLine();

		ImGui::SetNextItemWidth(itemWidth);
		int month = inText.month;
		ImGui::InputInt(("##" + std::string("month") + label).c_str(), &month, 0, 0, ImGuiInputTextFlags_CharsDecimal);
		inText.month = month;
		ImGui::SameLine();

		ImGui::Text("/");
		ImGui::SameLine();

		ImGui::SetNextItemWidth(itemWidth * 2);
		int year = inText.year;
		ImGui::InputInt(("##" + std::string("year") + label).c_str(), &year, 0, 0, ImGuiInputTextFlags_CharsDecimal);
		inText.year = year;

		inDate = inText;
	}
	bool Widget::InputDate(const char* label, Date& inDate, float itemWidth)
	{
		bool output = false;
		char buffer[16] = {};
		std::memcpy(buffer, inDate.to_string().c_str(), inDate.to_string().size());
		itemWidth -= ImGui::GetStyle().ItemSpacing.x * 4; // Adjust for spacing between inputs
		itemWidth -= ImGui::GetStyle().FramePadding.x * 2.f; // Adjust for padding on both sides
		itemWidth -= ImGui::CalcTextSize("/").x * 2;
		itemWidth /= 4;
		ImGui::SetNextItemWidth(itemWidth);

		int day = inDate.day;
		if (ImGui::InputInt(("##" + std::string("day") + label).c_str(), &day, 0, 0, ImGuiInputTextFlags_CharsDecimal))
		{
			inDate.day = day;
			output = true;
		}
		ImGui::SameLine();

		ImGui::Text("/");
		ImGui::SameLine();

		ImGui::SetNextItemWidth(itemWidth);
		int month = inDate.month;
		if (ImGui::InputInt(("##" + std::string("month") + label).c_str(), &month, 0, 0, ImGuiInputTextFlags_CharsDecimal))
		{
			inDate.month = month;
			output = true;
		}
		ImGui::SameLine();

		ImGui::Text("/");
		ImGui::SameLine();

		ImGui::SetNextItemWidth(itemWidth * 2);
		int year = inDate.year;
		if (ImGui::InputInt(("##" + std::string("year") + label).c_str(), &year, 0, 0, ImGuiInputTextFlags_CharsDecimal))
		{
			inDate.year = year;
			output = true;
		}
		return output;
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