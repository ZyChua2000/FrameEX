/******************************************************************************
/*!
\file       GUIUtils.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 20, 2025
\brief      Declares Utility functions for GUI

 /******************************************************************************/

#ifndef GUIUtils_HPP
#define GUIUtils_HPP
#include <chrono>
#include <rttr/variant.h>
struct ImGuiInputTextCallbackData;
namespace FrameExtractor
{
	namespace Widget
	{
		void InputTime(const char* label, std::string& inText, float itemWidth);
		void InputTime(const char* label, rttr::variant& inText, float itemWidth);
		void InputDate(const char* label, rttr::variant& inDate, float itemWidth);

		void InputInt(const char* id, const char* display, int32_t& data);
	}

	namespace Format
	{
		int FilterNumbersAndColon(ImGuiInputTextCallbackData* data);
		bool isValidFormat(const char* buffer);
		std::string fmtTime(int inTime);
	}
}

#endif