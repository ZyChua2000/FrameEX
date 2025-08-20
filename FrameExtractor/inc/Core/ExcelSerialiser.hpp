/******************************************************************************/
/*!
\file       ExcelSerialiser.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 15, 2025
\brief      Declares the Excel Serialiser class which serialises Data

 ******************************************************************************/

#ifndef ExcelSerialiser_HPP
#define ExcelSerialiser_HPP
 // Standard Library includes
#include <filesystem>

// Project includes
#include <Core/Project.hpp>

namespace OpenXLSX
{
	class XLWorksheet;
}

namespace FrameExtractor
{
	/*!***********************************************************************
		\brief
			Class that serialises and deserialises data to and from Excel files.
	*************************************************************************/
	class ExcelSerialiser
	{
	public:

		/*!***********************************************************************
			\brief
				Default constructor that initializes the ExcelSerialiser with a file path.
			\param[in] name
				The path to the Excel file.
		*************************************************************************/
		ExcelSerialiser(std::filesystem::path name);

		/*!***********************************************************************
			\brief
				Default destructor that cleans up the ExcelSerialiser.
		*************************************************************************/
		~ExcelSerialiser();

		/*!***********************************************************************
			\brief
				Exports the data from the project to an Excel file.
			\param[in] Task
				The dynamic task containing the data to export.
		*************************************************************************/
		void Export(DynamicTask& Task);

		/*!***********************************************************************
			\brief
				Imports data from an Excel file into the project.
			\param[in] Task
				The dynamic task to populate with imported data.
		*************************************************************************/
		void Import(DynamicTask& Task);
		void ExportSpikeDipReport(std::map<std::string, std::map<int32_t, CountData>>& countedData);
		std::map<std::string, std::map<int32_t, CountData>> ImportSpikeDipReport();
		std::map<std::string, std::map<int32_t, AggregateData>> ImportAggregatorReport();
	private:

		/*!***********************************************************************
			\brief
				Writes the data recursively to the Excel worksheet.
			\param[in] tree
				The reflection map containing the data to write.
			\param[in] path
				The current path in the reflection map.
			\param[in] wks
				The worksheet to write to.
			\param[in] task
				The dynamic task containing the data to write.
			\param[out] row
				The current row in the worksheet.
		*************************************************************************/
		void WriteRecursive(const ReflectionMap& tree, std::vector<rttr::variant>& path, OpenXLSX::XLWorksheet* wks, DynamicTask& task, int& row);


		std::map<int, size_t> mNumberFmts;	//<- Map of number formats to their indices
		std::filesystem::path mPath;		//<- Path to the Excel file
	};
}

#endif