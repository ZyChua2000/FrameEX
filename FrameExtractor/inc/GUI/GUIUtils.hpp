/******************************************************************************/
/*!
\file       GUIUtils.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 20, 2025
\brief      Declares Utility functions for GUI

 ******************************************************************************/

#ifndef GUIUtils_HPP
#define GUIUtils_HPP
 // Standard Library includes
#include <chrono>

// Third-party includes
#include <rttr/variant.h>
struct ImGuiInputTextCallbackData;
namespace FrameExtractor
{
	namespace Widget
	{
		/*!***********************************************************************
			\brief
				Widget that creates a text input field for time input.
			\param[in] label
				The label for the input field.
			\param[in] inText
				The string to be modified by the input field.
			\param[in] itemWidth
				The width of the input field.
		*************************************************************************/
		void InputTime(const char* label, std::string& inText, float itemWidth);

		/*!***********************************************************************
			\brief
				Widget that creates a text input field for time input.
			\param[in] label
				The label for the input field.
			\param[in] inText
				The variant to be modified by the input field.
			\param[in] itemWidth
				The width of the input field.
		*************************************************************************/
		void InputTime(const char* label, rttr::variant& inText, float itemWidth);

		/*!***********************************************************************
			\brief
				Widget that creates a text input field for date input.
			\param[in] label
				The label for the input field.
			\param[in] inDate
				The variant to be modified by the input field.
			\param[in] itemWidth
				The width of the input field.
		*************************************************************************/
		void InputDate(const char* label, rttr::variant& inDate, float itemWidth);

		/*!***********************************************************************
			\brief
				Widget that creates a text input field for integer input.
			\param[in] id
				The ID for the input field.
			\param[in] display
				The display label for the input field.
			\param[in,out] data
				The integer reference to be modified by the input field.
		*************************************************************************/
		void InputInt(const char* id, const char* display, int32_t& data);
	}

	namespace Format
	{
		/*!***********************************************************************
			\brief
				Callback function to filter input for time format.
			\param[in] data
				The ImGui input text callback data.
			\return
				Integer indicating the result of the filtering.
		*************************************************************************/
		int FilterNumbersAndColon(ImGuiInputTextCallbackData* data);

		/*!***********************************************************************
			\brief
				Validates if the input buffer is in a valid time format.
			\param[in] buffer
				The input buffer to validate.
			\return
				Boolean indicating whether the format is valid.
		*************************************************************************/
		bool isValidFormat(const char* buffer);

		/*!***********************************************************************
			\brief
				Formats the input time in a human-readable format.
			\param[in] inTime
				The time in seconds to format.
			\return
				String representing the formatted time.
		*************************************************************************/
		std::string fmtTime(int inTime);
	}
}

#endif