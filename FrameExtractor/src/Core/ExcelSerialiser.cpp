/******************************************************************************/
/*!
\file       ExcelSerialiser.cpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 15, 2025
\brief      Defines the Excel Serialiser class which serialises Data

 ******************************************************************************/

#include "FrameExtractorPCH.hpp"
 // Standard Library includes
#include <regex>
 // Third-party includes
#include <OpenXLSX.hpp>
// Project includes
#include "Core/ExcelSerialiser.hpp"
#include <Core/LoggerManager.hpp>
static const std::unordered_map<std::string, int> formatToId = {
	{ "D/M/YYYY", 14 },
	{ "D-MON-YY", 15 },
	{ "D-MON", 16 },
	{ "MON-YY", 17 },
	{ "H:MM AM/PM", 18 },
	{ "H:MM:SS AM/PM", 19 },
	{ "H:MM", 20 },
	{ "H:MM:SS", 21 }
};

namespace FrameExtractor
{
	std::string FormatToString(Date date, std::string fmt)
	{
		static const char* monthNames[] = {
		"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
		"JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
		};
		std::ostringstream oss;

		if (fmt == "D/M/YYYY")
		{
			oss << date.day << "/" << date.month << "/" << date.year;
		}
		else if (fmt == "D-MON-YY")
		{
			oss << date.day << "-" << monthNames[date.month - 1] << "-"
				<< std::setw(2) << std::setfill('0') << (date.year % 100);
		}
		else if (fmt == "D-MON")
		{
			oss << date.day << "-" << monthNames[date.month - 1];
		}
		else if (fmt == "MON-YY")
		{
			oss << monthNames[date.month - 1] << "-"
				<< std::setw(2) << std::setfill('0') << (date.year % 100);
		}
		else
		{
			return "<invalid format>";
		}

		return oss.str();
	}

	std::string FormatToString(Time time, const std::string& fmt)
	{
		std::ostringstream oss;

		bool ampm = fmt.find("AM/PM") != std::string::npos;
		bool includeSeconds = fmt.find("SS") != std::string::npos;

		int displayHour = time.hours;
		std::string suffix;

		if (ampm)
		{
			suffix = (displayHour < 12) ? "AM" : "PM";
			displayHour = displayHour % 12;
			if (displayHour == 0) displayHour = 12;
		}

		oss << displayHour << ":"
			<< std::setw(2) << std::setfill('0') << time.minutes;

		if (includeSeconds)
		{
			oss << ":" << std::setw(2) << std::setfill('0') << time.seconds;
		}

		if (ampm)
		{
			oss << " " << suffix;
		}

		return oss.str();
	}

	ExcelSerialiser::ExcelSerialiser(std::filesystem::path filepath) : mPath(filepath)
	{}

	ExcelSerialiser::~ExcelSerialiser()
	{}

	std::map<std::string, std::string> parseSpecifiers(const std::string& specStr)
	{
		std::map<std::string, std::string> result;
		std::stringstream ss(specStr);
		std::string part;

		while (std::getline(ss, part, ';'))
		{
			size_t eq = part.find('=');
			if (eq != std::string::npos)
			{
				std::string key = part.substr(0, eq);
				std::string val = part.substr(eq + 1);
				result[key] = val;
			}
		}

		return result;
	}
	std::string regex_escape(const std::string& s)
	{
		static const std::regex re(R"([.^$|()\\[\]{}*+?])");
		return std::regex_replace(s, re, R"(\$&)");
	}

	void ExcelSerialiser::WriteRecursive(const ReflectionMap& tree, std::vector<rttr::variant>& path, OpenXLSX::XLWorksheet* wks, DynamicTask& task, int& row)
	{
		for (const auto& [key, value] : tree)
		{
			path.push_back(key); // Record the path

			if (value.is_type<std::vector<TaskData>>())
			{
				const std::vector<TaskData>& data = value.get_value<std::vector<TaskData>>();
				auto& fmt = task.mSpecs->mExportFormat->get_value<ExcelExport>();
				std::map<std::string, rttr::variant> dataMapping;
				// Variable mapping
				for (int i = 0; i < task.mSpecs->MaxCategories(); i++)
				{
					dataMapping[*task.mSpecs->mNodeCategories->at(i)->mName] = path[i];
				}
				dataMapping[*task.mSpecs->mTab->mName] = path.back();
				for (int i = 0; i < data.size(); i++)
				{
					dataMapping[data[i].mFieldName] = data[i].mFieldData;
				}

				// Printing
				for (auto& [column, txtFmt] : fmt.mDataColumnMapping)
				{
					//regex {$VAR} map to variables

					std::regex pattern(R"(\{\$([^:}]+)(?::([^}]+))?\})");
					std::smatch match;
					std::map<std::string, std::string> parsedSpecifiers;
					bool nonMixed = false;
					std::string varName;

					if (std::regex_search(txtFmt, match, pattern))
					{
						if (match[0] == txtFmt)
						{
							nonMixed = true;
						}

						varName = match[1];        // "Date"
						std::string specifiers = match[2];     // "format=D/M/YYYY;pad=true"
						if (!specifiers.empty())
						{
							parsedSpecifiers = parseSpecifiers(specifiers);
						}
					}
					auto cell = wks->cell(row, column + 1); // +1 because OpenXLSX is 1-indexed
					if (nonMixed)
					{
						if (dataMapping.contains(varName))
						{
							if (dataMapping[varName].is_type<int>())
							{
								cell.value() = dataMapping[varName].get_value<int>();
							}

							else if (dataMapping[varName].is_type<bool>())
							{
								cell.value() = dataMapping[varName].get_value<bool>();
							}
							else if (dataMapping[varName].is_type<float>())
							{
								cell.value() = dataMapping[varName].get_value<float>();
							}
							else if (dataMapping[varName].is_type<double>())
							{
								cell.value() = dataMapping[varName].get_value<double>();
							}
							else if (dataMapping[varName].is_type<std::string>())
							{
								cell.value() = dataMapping[varName].get_value<std::string>();
							}
							else if (dataMapping[varName].is_type<Time>())
							{
								auto myTime = dataMapping[varName].get_value<Time>();
								std::tm tm{};
								tm.tm_hour = myTime.hours;
								tm.tm_min = myTime.minutes;
								tm.tm_sec = myTime.seconds;
								tm.tm_mday = 1;
								OpenXLSX::XLDateTime dt(tm);
								cell.value() = dt;
								if (parsedSpecifiers.contains("format"))
								{
									std::string specifier = parsedSpecifiers["format"];
									if (formatToId.contains(specifier))
									{
										cell.setCellFormat(mNumberFmts[formatToId.at(specifier)]);
									}
								}
							}
							else if (dataMapping[varName].is_type<Date>())
							{
								auto myDate = dataMapping[varName].get_value<Date>();
								std::tm tm{};
								tm.tm_year = myDate.year - 1900; // tm_year is years since 1900
								tm.tm_mon = myDate.month - 1; // tm_mon is 0-11
								tm.tm_mday = myDate.day;
								OpenXLSX::XLDateTime dt(tm);
								cell.value() = dt;
								if (parsedSpecifiers.contains("format"))
								{
									std::string specifier = parsedSpecifiers["format"];
									if (formatToId.contains(specifier))
									{
										cell.setCellFormat(mNumberFmts[formatToId.at(specifier)]);
									}
								}
							}
						}
					}
					else // Mixed: full string format
					{
						std::vector<std::pair<std::string, std::string>> replacing;
						for (std::sregex_iterator it(txtFmt.begin(), txtFmt.end(), pattern), end; it != end; ++it)
						{
							std::smatch match = *it;

							std::string varName = match[1];
							std::string specifier = match[2];

							std::map<std::string, std::string> parsedSpecifiers;
							if (match[2].matched)
							{
								parsedSpecifiers = parseSpecifiers(specifier);
							}

							std::string replace_str;
							if (dataMapping.contains(varName))
							{
								if (dataMapping[varName].is_type<Date>())
								{
									auto myDate = dataMapping[varName].get_value<Date>();
									replace_str = FormatToString(myDate, parsedSpecifiers.contains("format") ? parsedSpecifiers["format"] : "D/M/YYYY");
								}
								else if (dataMapping[varName].is_type<Time>())
								{
									auto myTime = dataMapping[varName].get_value<Time>();
									replace_str = FormatToString(myTime, parsedSpecifiers.contains("format") ? parsedSpecifiers["format"] : "H:MM AM/PM");
								}
								else if (dataMapping[varName].can_convert<std::string>())
								{
									replace_str = dataMapping[varName].convert<std::string>();
								}
							}

							// Use full match for replacement
							replacing.push_back({ match.str(0), replace_str });
						}

						// Replace all matches in the string (in one pass)
						for (const auto& [matchText, replaceStr] : replacing)
						{
							txtFmt = std::regex_replace(txtFmt, std::regex(regex_escape(matchText)), replaceStr);
						}

						wks->cell(row, column + 1).value() = txtFmt; // Set the cell value to the formatted string
					}
				}

				row++;
			}
			else if (value.is_type<ReflectionMap>())
			{
				const auto& subtree = value.get_value<ReflectionMap>();
				WriteRecursive(subtree, path, wks, task, row);
			}

			path.pop_back(); // Backtrack
		}
	}

	void ExcelSerialiser::Export(DynamicTask& Task)
	{
		OpenXLSX::XLDocument doc;
		doc.create(mPath.string(), OpenXLSX::XLForceOverwrite);

		auto wks = doc.workbook().worksheet("Sheet1");

		auto& fmt = Task.mSpecs->mExportFormat->get_value<ExcelExport>();
		// write headers
		for (int i = 0; i < fmt.mColumnHeaders.size(); i++)
		{
			wks.cell(1, i + 1).value() = fmt.mColumnHeaders[i];
		}
		std::vector<rttr::variant> var;
		int row = 2;

		OpenXLSX::XLCellFormats& cellFormats = doc.styles().cellFormats();
		for (const auto& [str, num] : formatToId)
		{
			mNumberFmts[num] = cellFormats.create();
			auto format = cellFormats[mNumberFmts[num]];
			format.setNumberFormatId(num);
		}

		WriteRecursive(Task.mPages, var, &wks, Task, row);

		doc.save();
	}

	void ExcelSerialiser::Import(DynamicTask& Task)
	{
		OpenXLSX::XLDocument doc(mPath.string());
		if (!doc.isOpen())
		{
			APP_CORE_ERROR("Invalid Excel File for Spike Dip");
		}
		auto wks = doc.workbook().worksheet("Sheet1");
		std::map<std::string, DataSpecification> mSpecsMapping;
		auto& excelImport = Task.mSpecs->mImportFormat->get_value<ExcelImport>();
		Task.mPages.clear();
		for (auto fieldSpecs : *Task.mSpecs->mFieldSpecs)
		{
			mSpecsMapping[*fieldSpecs->mName] = *fieldSpecs;
		}
		for (auto categorySpecs : *Task.mSpecs->mNodeCategories)
		{
			mSpecsMapping[*categorySpecs->mName] = *categorySpecs;
		}
		mSpecsMapping[*Task.mSpecs->mTab->mName] = *Task.mSpecs->mTab;

		uint32_t startRow = excelImport.mHasHeader ? 2 : 1; // If has header, start from row 2, else start from row 1
		for (uint32_t row = startRow; row <= wks.rowCount(); row++)
		{
			std::map<std::string, rttr::variant> dataMapping;
			for (auto& [colIDX, varName] : excelImport.mDataMapping)
			{
				if (mSpecsMapping.contains(varName))
				{
					auto& specs = mSpecsMapping[varName];
					auto cell = wks.cell(row, colIDX + 1); // +1 because OpenXLSX is 1-indexed
					switch (*specs.mType)
					{
					case DataType::Int:
						if (!cell.empty() && cell.value().type() == OpenXLSX::XLValueType::Integer)
						{
							dataMapping[varName] = cell.value().get<int>();
						}
						else
						{
							dataMapping[varName] = specs.mDefault ? *reinterpret_cast<int*>(specs.mDefault) : 0; // Default value
						}
						break;
					case DataType::Bool:
						if (!cell.empty() && cell.value().type() == OpenXLSX::XLValueType::Boolean)
						{
							dataMapping[varName] = cell.value().get<bool>();
						}
						else
						{
							dataMapping[varName] = specs.mDefault ? *reinterpret_cast<bool*>(specs.mDefault) : false; // Default value
						}
						break;
					case DataType::Float:
						if (!cell.empty() && cell.value().type() == OpenXLSX::XLValueType::Float)
						{
							dataMapping[varName] = cell.value().get<float>();
						}
						else
						{
							dataMapping[varName] = specs.mDefault ? *reinterpret_cast<float*>(specs.mDefault) : 0.0f; // Default value
						}
						break;
					case DataType::Double:
						if (!cell.empty() && cell.value().type() == OpenXLSX::XLValueType::Float)
						{
							dataMapping[varName] = cell.value().get<double>();
						}
						else
						{
							dataMapping[varName] = specs.mDefault ? *reinterpret_cast<double*>(specs.mDefault) : 0.0; // Default value
						}
						break;
					case DataType::String:
						if (!cell.empty() && cell.value().type() == OpenXLSX::XLValueType::String)
						{
							dataMapping[varName] = cell.value().get<std::string>();
						}
						else
						{
							dataMapping[varName] = specs.mDefault ? *reinterpret_cast<std::string*>(specs.mDefault) : std::string(); // Default value
						}
						break;
					case DataType::Date:
						if (!cell.empty())
						{
							auto dt = cell.value().get<OpenXLSX::XLDateTime>();
							Date date;
							auto tm = dt.tm();
							date.year = tm.tm_year + 1900; // tm_year is years since 1900
							date.month = tm.tm_mon + 1; // tm_mon is 0-11
							date.day = tm.tm_mday;
							dataMapping[varName] = date;
						}
						else
						{
							dataMapping[varName] = specs.mDefault ? *reinterpret_cast<Date*>(specs.mDefault) : Date(); // Default value
						}
						break;
					case DataType::Time:
						if (!cell.empty())
						{
							auto dt = cell.value().get<OpenXLSX::XLDateTime>();
							Time time;
							time.hours = dt.tm().tm_hour;
							time.minutes = dt.tm().tm_min;
							time.seconds = dt.tm().tm_sec;
							dataMapping[varName] = time;
						}
						else
						{
							dataMapping[varName] = specs.mDefault ? *reinterpret_cast<Time*>(specs.mDefault) : Time(); // Default value
						}
						break;
					}
				}
			}
			std::vector<rttr::variant> keys;
			// Add dataMapping to Task
			for (auto cat : *Task.mSpecs->mNodeCategories)
			{
				if (dataMapping.contains(*cat->mName))
				{
					Task.AddPage(keys, dataMapping[*cat->mName]);
					keys.push_back(dataMapping[*cat->mName]);
				}
			}
			Task.AddTab(keys, dataMapping[*Task.mSpecs->mTab->mName]); // Add tab with keys
			keys.push_back(dataMapping[*Task.mSpecs->mTab->mName]);
			auto& variant = Task.GetTab(keys);

			std::vector<TaskData> datas;

			for (auto field : *Task.mSpecs->mFieldSpecs)
			{
				if (dataMapping.contains(*field->mName))
				{
					TaskData data;
					data.mFieldName = *field->mName;
					data.mFieldData = dataMapping[*field->mName];
					datas.push_back(data);
				}
			}

			variant = datas;
		}

		doc.close();
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
		wks.cell("B1").value() = "InputTime";
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
							ss << "Video is blanked after " << time.second << ", ";
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
					while ((start_pos = additionalNotes.find("\r\n", start_pos)) != std::string::npos)
					{
						additionalNotes.replace(start_pos, 2, " ");
						// No need to increment by 2 since we replaced it with 1 character
						start_pos += 1;
					}

					// Then replace remaining lone '\n' or '\r'
					start_pos = 0;
					while ((start_pos = additionalNotes.find("\n", start_pos)) != std::string::npos)
					{
						additionalNotes.replace(start_pos, 1, " ");
						start_pos += 1;
					}

					start_pos = 0;
					while ((start_pos = additionalNotes.find("\r", start_pos)) != std::string::npos)
					{
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

		for (auto col = 1; col <= wks.row(1).cellCount(); col++)
		{
			if (wks.cell(1, col).getString() == "Store Code")
			{
				storeCodeCol = col;
			}
			else if (wks.cell(1, col).getString() == "InputTime")
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

		for (uint32_t row = 2; row <= wks.rowCount(); row++)
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
				try
				{
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
		for (uint32_t row = 2; row <= wks.rowCount(); row++)
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