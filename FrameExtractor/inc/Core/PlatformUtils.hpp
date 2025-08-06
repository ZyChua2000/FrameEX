/******************************************************************************
/*!
\file       PlatformUtils.hpp
\author     Chua Zheng Yang
\par		email: 2202829\sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 16, 2025
\brief      Declares the Platform specific utilities

 /******************************************************************************/

#ifndef PlatformUtils_HPP
#define PlatformUtils_HPP
#include <filesystem>
namespace FrameExtractor
{
	std::filesystem::path OpenFileDialog(const char* filter);

	std::filesystem::path SaveFileDialog(const char* filter);

	void CopyToClipboard(std::string text);

	void HideTerminal();
}

#endif