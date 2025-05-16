/******************************************************************************
/*!
\file       YAMLSerialiser.hpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 16, 2024
\brief      Declares the YAML Serialiser class which serialises Data

 /******************************************************************************/

#ifndef YAMLSerialiser_HPP
#define YAMLSerialiser_HPP
#include <filesystem>
#include <GUI/ToolsPanel.hpp>
namespace FrameExtractor
{
	class YAMLSerialiser
	{
	public:
		YAMLSerialiser(std::filesystem::path name);
		~YAMLSerialiser();

		void ExportProject(std::map<std::string, std::map<int32_t, CountData>>& countedData);
		void ImportProject(std::map<std::string, std::map<int32_t, CountData>>& countedData);

	private:
		std::filesystem::path mPath;
	};
}

#endif
