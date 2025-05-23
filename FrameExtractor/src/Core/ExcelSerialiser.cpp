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
		for (auto& [storeCode, timeNData] : countedData)
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
					wks.cell(Count, 10).value() = data.mReOthers;

				std::stringstream ss;

				for (auto entranceNum = 0; entranceNum < data.Entrance.size(); entranceNum++)
				{
		
					// Person details
					if (data.Entrance.size() > 1)
						ss << "E" << entranceNum + 1 << ": ";

					for (auto& frameSkip : data.Entrance[entranceNum].mFrameSkips)
					{
						ss << "Video Skipped from " << frameSkip.first << " to " << frameSkip.second << ", ";
					}

					for (auto& video : data.Entrance[entranceNum].mCorruptedVideos)
					{
						ss << "Video " << video << " is corrupted, ";
					}

					for (auto& time : data.Entrance[entranceNum].mBlankedVideos)
					{
						if (time.first) // start
							ss << "Video starts after " << time.second << ", ";
						else
							ss << "Video is blanked after" << time.second << ", ";
					}
					// per entrance data
					if (CheckEntryEmpty(data.Entrance[entranceNum].mDesc))
					{
						continue;
					}

					for (auto type = (int)ReCustomer; type <= ReOthers; type++)
					{
						for (auto personDesc : data.Entrance[entranceNum].mDesc[type])
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

					std::string additionalNotes = data.Entrance[entranceNum].mAdditionalNotes;
					size_t start_pos = 0;
					while ((start_pos = additionalNotes.find("\r\n", start_pos)) != std::string::npos) {
						additionalNotes.replace(start_pos, 2, " ");
						// No need to increment by 2 since we replaced it with 1 character
						start_pos += 1;
					}

					// Then replace remaining lone '\n' or '\r'
					start_pos = 0;
					while ((start_pos = additionalNotes.find("\n", start_pos)) != std::string::npos) {
						additionalNotes.replace(start_pos, 1, " ");
						start_pos += 1;
					}

					start_pos = 0;
					while ((start_pos = additionalNotes.find("\r", start_pos)) != std::string::npos) {
						additionalNotes.replace(start_pos, 1, " ");
						start_pos += 1;
					}

					ss << additionalNotes;
				}
				wks.cell(Count, 11).value() = ss.str();

				Count++;
			}
		}

		doc.save();
	}

	std::map<std::string, std::map<int32_t, CountData>> ExcelSerialiser::ImportSpikeDipReport()
	{
		OpenXLSX::XLDocument doc(mPath.string());
		if (!doc.isOpen())
		{
			APP_CORE_ERROR("Invalid Excel File for Spike Dip");
		}
		std::map<std::string, std::map<int32_t, CountData>> output;

		auto wks = doc.workbook().worksheet("Sheet1");


		int storeCodeCol = -1;
		int timeCol = -1;
		int customersCol = -1;
		int reCustomersCol = -1;
		int staffCol = -1;
		int reStaffCol = -1;
		int childrenCol = -1;
		int reChildrenCol = -1;
		int othersCol = -1;
		int reOthersCol = -1;

		for (auto col = 1; col < wks.row(1).cellCount(); col++)
		{
			if (wks.cell(1, col).getString() == "Store Code")
			{
				storeCodeCol = col;
			}
			else if (wks.cell(1, col).getString() == "Time")
			{
				timeCol = col;
			}
			else if (wks.cell(1, col).getString() == "Customers")
			{
				customersCol = col;
			}
			else if (wks.cell(1, col).getString() == "Re-entry Customers")
			{
				reCustomersCol = col;
			}
			else if (wks.cell(1, col).getString() == "Suspected Staff")
			{
				staffCol = col;
			}
			else if (wks.cell(1, col).getString() == "Re-entry Suspected Staff")
			{
				reStaffCol = col;
			}
			else if (wks.cell(1, col).getString() == "Children")
			{
				childrenCol = col;
			}
			else if (wks.cell(1, col).getString() == "Re-entry Children")
			{
				reChildrenCol = col;
			}
			else if (wks.cell(1, col).getString() == "Others")
			{
				othersCol = col;
			}
			else if (wks.cell(1, col).getString() == "Re-entry Others")
			{
				reOthersCol = col;
			}
		}


		for (uint32_t row = 2; row < wks.rowCount(); row++)
		{
			if (storeCodeCol != -1)
			{
				if (!wks.cell(row, storeCodeCol).empty())
					output[wks.cell(row, storeCodeCol).getString()] = {};
			}
			if (timeCol != -1)
			{
				if (wks.cell(row, timeCol).empty())
				{
					continue;
				}

				int time = -1;
				auto timeStr = wks.cell(row, timeCol).getString();
				try {
					time = std::stoi(timeStr);
				}
				catch (...)
				{
					time = std::stoi(timeStr.substr(0, 2));
				}
				output[wks.cell(row, storeCodeCol).getString()][time] = {};

				if (customersCol != -1)
				{
					if (wks.cell(row, customersCol).value().type() != OpenXLSX::XLValueType::Empty)
						output[wks.cell(row, storeCodeCol).getString()][time].mCustomer = wks.cell(row, customersCol).value();
				}
				if (reCustomersCol != -1)
				{
					if (wks.cell(row, reCustomersCol).value().type() != OpenXLSX::XLValueType::Empty)
						output[wks.cell(row, storeCodeCol).getString()][time].mReCustomer = wks.cell(row, reCustomersCol).value();
				}
				if (staffCol != -1)
				{
					if (wks.cell(row, staffCol).value().type() != OpenXLSX::XLValueType::Empty)
						output[wks.cell(row, storeCodeCol).getString()][time].mSuspectedStaff = wks.cell(row, staffCol).value();
				}
				if (reStaffCol != -1)
				{
					if (wks.cell(row, reStaffCol).value().type() != OpenXLSX::XLValueType::Empty)
						output[wks.cell(row, storeCodeCol).getString()][time].mReSuspectedStaff = wks.cell(row, reStaffCol).value();
				}
				if (childrenCol != -1)
				{
					if (wks.cell(row, childrenCol).value().type() != OpenXLSX::XLValueType::Empty)
						output[wks.cell(row, storeCodeCol).getString()][time].mChildren = wks.cell(row, childrenCol).value();
				}
				if (reChildrenCol != -1)
				{
					if (wks.cell(row, reChildrenCol).value().type() != OpenXLSX::XLValueType::Empty)
						output[wks.cell(row, storeCodeCol).getString()][time].mReChildren = wks.cell(row, reChildrenCol).value();
				}
				if (othersCol != -1)
				{
					if (wks.cell(row, othersCol).value().type() != OpenXLSX::XLValueType::Empty)
						output[wks.cell(row, storeCodeCol).getString()][time].mOthers = wks.cell(row, othersCol).value();
				}
				if (reOthersCol != -1)
				{
					if (wks.cell(row, reOthersCol).value().type() != OpenXLSX::XLValueType::Empty)
						output[wks.cell(row, storeCodeCol).getString()][time].mReOthers = wks.cell(row, reOthersCol).value();
				}
				output[wks.cell(row, storeCodeCol).getString()][time].Entrance.push_back({});
			}
		}

		doc.close();

		return output;

	}

	std::map<std::string, std::map<int32_t, AggregateData>> ExcelSerialiser::ImportAggregatorReport()
	{
		OpenXLSX::XLDocument doc(mPath.string());
		if (!doc.isOpen())
		{
			APP_CORE_ERROR("Invalid Excel File for Spike Dip");
		}
		auto wks = doc.workbook().worksheet("Aggregates");


		std::map<std::string, std::map<int32_t, AggregateData>> data;
		for (uint32_t row = 2; row < wks.rowCount(); row++)
		{
			std::string shopperID = wks.cell(row, 1).getString();
			std::string storeCode = wks.cell(row, 2).getString();
			std::string DateStr = wks.cell(row, 3).getString();
			int32_t hour = wks.cell(row, 4).value();
			int32_t entry = wks.cell(row, 5).value();
			int32_t exit = wks.cell(row, 6).value();

			std::string dateFMT = DateStr.substr(6, 2) + DateStr.substr(4, 2) + DateStr.substr(0, 4);
			int32_t date = std::stoi(dateFMT);

			if (!data.contains(shopperID))
			{
				data[shopperID] = {};
			}

			if (!data[shopperID].contains(hour))
			{
				data[shopperID][hour] = {};
				data[shopperID][hour].Enters = entry;
				data[shopperID][hour].Exit = exit;
				data[shopperID][hour].StoreID = storeCode;
				data[shopperID][hour].Entrance.push_back({});
			}
		}

		return data;
	}

}