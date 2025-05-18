/******************************************************************************
/*!
\file       ExcelSerialiser.cpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 15, 2024
\brief      Defines the Excel Serialiser class which serialises Data

 /******************************************************************************/

#include "FrameExtractorPCH.hpp"
#include "Core/ExcelSerialiser.hpp"
#include <OpenXLSX.hpp>
#include <Core/LoggerManager.hpp>
namespace FrameExtractor
{
	ExcelSerialiser::ExcelSerialiser(std::filesystem::path filepath) : mPath(filepath)
	{
	}

	ExcelSerialiser::~ExcelSerialiser()
	{
	}

	// vector for each entrance
	// array for each category of person(8)
	// Each category has vector of ppl, if all vectors are empty then it's empty
	static bool CheckEntryEmpty(std::array<std::vector<PersonDesc>, 8> data)
	{
		for (auto& nData : data)
		{
			if (!nData.empty()) return false;
		}
		return true;
	}

	void ExcelSerialiser::ExportSpikeDipReport(std::map<std::string, std::map<int32_t, CountData>>& countedData)
	{
	
	}

	std::map<std::string, std::map<int32_t, CountData>> ExcelSerialiser::ImportSpikeDipReport()
	{
		return {};

	}

}