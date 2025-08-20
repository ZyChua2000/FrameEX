/******************************************************************************/
/*!
\file       PlatformUtils.hpp
\author     Chua Zheng Yang
\par		email: 2202829\sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 16, 2025
\brief      Declares the Platform specific utilities

 ******************************************************************************/

#ifndef PlatformUtils_HPP
#define PlatformUtils_HPP
 // Standard Library includes
#include <filesystem>
namespace FrameExtractor
{

	/*!***********************************************************************
		\brief
			Function to open a file dialog and return the selected file path.
		\param[in] filter
			The filter for the file dialog, e.g., 
			"Image Files (*.png;*.jpg)\0*.png;*.jpg\0All Files (*.*)\0*.*\0"
		\return
			The selected file path as a std::filesystem::path object.
	*************************************************************************/
	std::filesystem::path OpenFileDialog(const char* filter);

	/*!***********************************************************************
		\brief
			Function to open a save file dialog and return the selected file path.
		\param[in] filter
			The filter for the save file dialog, e.g., 
			"Image Files (*.png;*.jpg)\0*.png;*.jpg\0All Files (*.*)\0*.*\0"
		\return
			The selected file path as a std::filesystem::path object.
	*************************************************************************/
	std::filesystem::path SaveFileDialog(const char* filter);

	/*!***********************************************************************
		\brief
			Function to copy text to the clipboard.
		\param[in] text
			The text to copy to the clipboard.
	*************************************************************************/
	void CopyToClipboard(std::string text);

	/*!***********************************************************************
		\brief
			Function to hide the terminal window.
	*************************************************************************/
	void HideTerminal();
}

#endif