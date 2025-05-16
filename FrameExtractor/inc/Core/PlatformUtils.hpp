/******************************************************************************
/*!
\file       PlatformUtils.hpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 16, 2024
\brief      Declares the Platform specific utilities

 /******************************************************************************/

#ifndef PlatformUtils_HPP
#define PlatformUtils_HPP
#include <filesystem>
namespace FrameExtractor
{
	std::filesystem::path OpenFileDialog(const char* filter);

	std::filesystem::path SaveFileDialog(const char* filter);
}

#endif
