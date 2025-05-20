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
#include <Core/Project.hpp>
namespace FrameExtractor
{
	class ExplorerPanel;

	class ToolsPanel : public IPanel
	{
	public:
		ToolsPanel(Project* project);
		~ToolsPanel() override;
		virtual void OnImGuiRender(float dt) override;
		virtual const char* GetName() const override;
		virtual void OnAttach() override;
	private:
		struct PageNumber
		{
			int mStorePage = 0;
			int mDatePage = 0;
			int mHourPage = 0;
		};

		std::string ExportAggregateStoreDataAsString();
		std::string errorLine1;
		std::string errorLine2;
		PageNumber mCountingPage;

		Project* mProject;
		int32_t mTimeBuffer = 0;
		int32_t mEntranceBuffer = 1;
		int32_t mEnterBuffer = 0;
		int32_t mExitBuffer = 0;
		char mStoreCodeBuffer[16] = {};
		char shopperIDBuffer[16] = {};
		char dateBuffer[9] = {};
		bool errorCodeBool = false;

		void CountingTab();

	};

}

#endif
