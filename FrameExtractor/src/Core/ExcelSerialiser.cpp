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
FrameExtractor::ExcelSerialiser::ExcelSerialiser(std::filesystem::path filepath) : mPath(filepath)
{
}

FrameExtractor::ExcelSerialiser::~ExcelSerialiser()
{
}

// vector for each entrance
// array for each category of person(8)
// Each category has vector of ppl, if all vectors are empty then it's empty
static bool CheckEntryEmpty(std::array<std::vector<FrameExtractor::PersonDesc>, 8> data)
{
	for (auto& nData : data)
	{
		if (!nData.empty()) return false;
	}
	return true;
}

void FrameExtractor::ExcelSerialiser::ExportSpikeDipReport(std::map<std::string, std::map<int32_t, CountData>>& countedData)
{
	OpenXLSX::XLDocument doc;
	doc.create(mPath.string(), OpenXLSX::XLForceOverwrite);

	auto wks = doc.workbook().worksheet("Sheet1");
	wks.cell("A1").value() = "Store Code";
	wks.cell("B1").value() = "Time";
	wks.cell("C1").value() = "Customers";
	wks.cell("D1").value() = "Re-entry Customers";
	wks.cell("E1").value() = "Suspected Staff";
	wks.cell("F1").value() = "Re-Entry Suspected Staff";
	wks.cell("G1").value() = "Children";
	wks.cell("H1").value() = "Re-Entry Children";
	wks.cell("I1").value() = "Others";
	wks.cell("J1").value() = "Re-Entry Others";
	wks.cell("K1").value() = "Notes";

	int Count = 2;
	for (auto& [storeCode,timeNData] : countedData)
	{
		for (auto& [time, data] : timeNData)
		{
			wks.cell(Count, 1).value() = storeCode;
			wks.cell(Count, 2).value() = time;
			if (data.mCustomer)
			wks.cell(Count, 3).value() = data.mCustomer;
			if (data.mReCustomer)
			wks.cell(Count, 4).value() = data.mReCustomer;
			if (data.mSuspectedStaff)
			wks.cell(Count, 5).value() = data.mSuspectedStaff;
			if (data.mReSuspectedStaff)
			wks.cell(Count, 6).value() = data.mReSuspectedStaff;
			if (data.mChildren)
			wks.cell(Count, 7).value() = data.mChildren;
			if (data.mReChildren)
			wks.cell(Count, 8).value() = data.mReChildren;
			if (data.mOthers)
			wks.cell(Count, 9).value() = data.mOthers;
			if (data.mReOthers)
			wks.cell(Count, 10).value()= data.mReOthers;
			
			std::stringstream ss;
		
			for (auto entranceNum = 0; entranceNum < data.Entrance.size(); entranceNum++)
			{
				if (CheckEntryEmpty(data.Entrance[entranceNum]))
				{
					continue;
				}
				ss << "E" << entranceNum + 1 << ": ";

				for (auto type = (int)ReCustomer; type <= ReOthers; type++)
				{
					for (auto personDesc : data.Entrance[entranceNum][type])
					{
						if (personDesc.IsMale)
						{
							ss << "(M) ";
						}
						else
						{
							ss << "(F) ";
						}
						ss << EntryTypeToString((EntryType)type) + " wearing " << personDesc.Description << " at " << personDesc.timeStamp << ", ";
					}
				}
			}
			wks.cell(Count, 11).value() = ss.str();

			Count++;
		}
	}

	doc.save();
}
