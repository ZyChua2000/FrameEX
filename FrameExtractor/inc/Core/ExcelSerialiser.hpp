/******************************************************************************
/*!
\file       ExcelSerialiser.hpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 15, 2024
\brief      Declares the Excel Serialiser class which serialises Data

 /******************************************************************************/

#ifndef ExcelSerialiser_HPP
#define ExcelSerialiser_HPP
#include <filesystem>
#include <Core/Project.hpp>
namespace FrameExtractor
{
	class ExcelSerialiser
	{
	public:
		ExcelSerialiser(std::filesystem::path name);
		~ExcelSerialiser();

		void ExportSpikeDipReport(std::map<std::string, std::map<int32_t, CountData>>& countedData);
		std::map<std::string, std::map<int32_t, CountData>> ImportSpikeDipReport();
		std::map<std::string, std::map<int32_t, AggregateData>> ImportAggregatorReport();
	private:
		std::filesystem::path mPath;
	};
}

#endif
