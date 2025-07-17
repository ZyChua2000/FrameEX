/******************************************************************************
/*!
\file       ExcelSerialiser.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 15, 2024
\brief      Declares the Excel Serialiser class which serialises Data

 /******************************************************************************/

#ifndef ExcelSerialiser_HPP
#define ExcelSerialiser_HPP
#include <filesystem>
#include <Core/Project.hpp>

namespace OpenXLSX
{
	class XLWorksheet;
}

namespace FrameExtractor
{
	class ExcelSerialiser
	{
	public:
		ExcelSerialiser(std::filesystem::path name);
		~ExcelSerialiser();
		void Export(DynamicTask& Task);
		void Import(DynamicTask& Task);
		void ExportSpikeDipReport(std::map<std::string, std::map<int32_t, CountData>>& countedData);
		std::map<std::string, std::map<int32_t, CountData>> ImportSpikeDipReport();
		std::map<std::string, std::map<int32_t, AggregateData>> ImportAggregatorReport();
	private:
		void WriteRecursive(const ReflectionMap& tree, std::vector<rttr::variant>& path, OpenXLSX::XLWorksheet* wks, DynamicTask& task, int& row);

		std::map<int, size_t> mNumberFmts;
		std::filesystem::path mPath;
	};
}

#endif
