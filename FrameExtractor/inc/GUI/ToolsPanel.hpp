/******************************************************************************
/*!
\file       ToolsPanel.hpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 10, 2024
\brief      Declares the Tools Panel class which has all the tools.

 /******************************************************************************/

#ifndef ToolsPanel_HPP
#define ToolsPanel_HPP
#include <string>
#include <imgui.h>
#include <GUI/IPanel.hpp>
namespace FrameExtractor
{
	class ExplorerPanel;

	enum EntryType
	{
		Customer,
		ReCustomer,
		SuspectedStaff,
		ReSuspectedStaff,
		Children,
		ReChildren,
		Others,
		ReOthers
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

		std::vector<std::array<std::vector<PersonDesc>, EntryType::ReOthers + 1>> Entrance;
	};

	class ToolsPanel : public IPanel
	{
	public:
		ToolsPanel();
		~ToolsPanel() override;
		virtual void OnImGuiRender(float dt) override;
		virtual const char* GetName() const override;
		virtual void OnAttach() override;

		void SetData(std::map<std::string, std::map<int32_t, CountData>> inData) { mData = inData; }
		std::map<std::string, std::map<int32_t, CountData>>& GetData() { return mData; }
	private:
		std::vector<std::filesystem::path> videosInProject;
		std::map<std::string, std::map<int32_t,CountData>> mData;

		char mStoreCodeBuffer[16] = {};
		int32_t mTimeBuffer = 0;
		int32_t mEntranceBuffer = 1;
	};

}

#endif
