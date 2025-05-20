/******************************************************************************
/*!
\file       Project.hpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 16, 2024
\brief      Declares the Project class that contains all data and settings
			related to the project. 

 /******************************************************************************/

#ifndef Project_HPP
#define Project_HPP
#include <filesystem>
#include <map>
#include <array>
#include <vector>

//#include <rttr/variant.h>
namespace FrameExtractor
{

	enum EntryType
	{
		Customer,
		ReCustomer,
		SuspectedStaff,
		ReSuspectedStaff,
		Children,
		ReChildren,
		Others,
		ReOthers,
	};

	std::string EntryTypeToString(EntryType type);

	struct StoreProp
	{
		std::string StoreCode = "";
		int32_t numEntrance = 1;
	};

	struct PersonDesc
	{
		bool IsMale = true;
		std::string timeStamp = "00:00:00";
		std::string Description = "";
	};

	struct CountingEntrance
	{
		std::array <std::vector<PersonDesc>, EntryType::ReOthers + 1> mDesc;
		std::vector<std::pair<std::string, std::string>> mFrameSkips; // time to time
		std::vector<std::string> mCorruptedVideos; // name
		std::vector<std::pair<bool,std::string>> mBlankedVideos; // time
		std::string mAdditionalNotes;
	};

	struct CountData // Represents 1 hour
	{
		int32_t mCustomer = 0;
		int32_t mReCustomer = 0;
		int32_t mSuspectedStaff = 0;
		int32_t mReSuspectedStaff = 0;
		int32_t mChildren = 0;
		int32_t mReChildren = 0;
		int32_t mOthers = 0;
		int32_t mReOthers = 0;

		std::vector<CountingEntrance> Entrance;
		// Entrance Num, Data Category, Person Data
		//Entrance
	};

	struct AggregateEntrance
	{
		std::vector<std::string> mCorruptedVideos;
		std::vector<std::string> mBlankedVideos; // video num to time
		std::vector<std::pair<std::string, std::string>> mFrameSkips; // time to time

	};

	struct AggregateData
	{
		// Pre-existing Data
		std::string StoreID = "DIOR224";
		int8_t Enters = 3;
		int8_t Exit = 0;

		// New Data
		int32_t mCustomer = 0;
		std::vector<AggregateEntrance> Entrance;
		// Entrance Num, Data(time to time)

	};

	class Project
	{
	public:
		Project();
		~Project();

		inline bool IsProjectLoaded() const { return !mProjectDir.empty(); }
		void CreateProject(std::string name, std::filesystem::path dir);
		void LoadProject(std::filesystem::path path);
		void SaveProject();
		inline std::filesystem::path GetAssetsDir() const { return mAssetDir; }


		using StoreCode = std::string;
		using Hour = int32_t;
		using Date = int32_t;

		std::map<StoreCode, std::map<Hour, CountData>> mCountingData;
		std::map<Date, std::map<StoreCode, std::map<Hour, AggregateData>>> mAggregateStoreData;
		std::vector<std::filesystem::path> mVideosInProject;

	private:

		std::string mName;
		std::filesystem::path mProjectDir;
		std::filesystem::path mAssetDir;
		std::filesystem::path mExportDir;
		std::filesystem::path mProjectFilePath;
		//std::vector<rttr::variant> mDataList;

	};
}

#endif
