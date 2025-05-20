/******************************************************************************
/*!
\file       GUIUtils.hpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 20, 2025
\brief      Declares Utility functions for GUI

 /******************************************************************************/

#ifndef GUIUtils_HPP
#define GUIUtils_HPP

struct ImGuiInputTextCallbackData;
namespace FrameExtractor
{
	namespace Widget
	{
		void Time(const char* label, std::string& inText, float itemWidth);
	}

	namespace Format
	{
		int FilterNumbersAndColon(ImGuiInputTextCallbackData* data);
		bool isValidFormat(const char* buffer);
		std::string fmtTime(int inTime);
	}
}

#endif