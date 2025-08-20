/******************************************************************************/
/*!
\file       ToolsPanel.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 10, 2025
\brief      Declares the Tools Panel class which has all the tools.

 ******************************************************************************/

#ifndef ToolsPanel_HPP
#define ToolsPanel_HPP
 // Standard Library includes
#include <string>

// Third-party includes
#include <imgui.h>

// Project includes
#include <GUI/IPanel.hpp>
#include <Core/Project.hpp>
namespace FrameExtractor
{
	class ExplorerPanel;

	/*!***********************************************************************
		\brief
			Class that represents the Tools Panel in the GUI, which contains
			various tools for data extraction and manipulation.
	*************************************************************************/
	class ToolsPanel : public IPanel
	{
	public:

		/*!***********************************************************************
			\brief
				Constructor for the ToolsPanel class.
			\param[in] project
				The project associated with this tools panel.
		*************************************************************************/
		ToolsPanel(Project* project);

		/*!***********************************************************************
			\brief
				Default destructor for the ToolsPanel class.
		*************************************************************************/
		~ToolsPanel() override;

		/*!***********************************************************************
			\brief
				Renders the ImGui interface for the Tools Panel.
			\param[in] dt
				The delta time since the last frame.
		*************************************************************************/
		virtual void OnImGuiRender(float dt) override;

		/*!***********************************************************************
			\brief
				Returns the name of the panel.
			\return
				The name of the panel as a string.
		*************************************************************************/
		virtual const char* GetName() const override;

		/*!***********************************************************************
			\brief
				Callback function that is called when the panel is attached.
		*************************************************************************/
		virtual void OnAttach() override;
	private:
		struct PageNumber
		{
			int mStorePage = 0;
			int mHourPage = 0;
		};

		std::string ExportAggregateStoreDataAsString(int date);
		std::string errorLine1;
		std::string errorLine2;
		PageNumber mCountingPage;
		PageNumber mAggregatePage;

		Project* mProject;
		int32_t mTimeBuffer = 0;
		int32_t mEntranceBuffer = 1;
		int32_t mEnterBuffer = 0;
		int32_t mExitBuffer = 0;
		int32_t mDayBuffer = 1;
		int32_t mMonthBuffer = 1;
		int32_t mYearBuffer = 2000;
		char mStoreCodeBuffer[16] = {};
		char shopperIDBuffer[16] = {};
		char dateBuffer[9] = {};
		bool errorCodeBool = false;

		bool exportSettingbool = false;
		bool importSettingbool = false;
		std::string exportVarPayload;
		std::string importVarPayload;
		DynamicTaskInterface mTaskInterface;
		ExcelExport mExcelExportBuffer;
		ExcelImport mExcelImportBuffer;

		void CountingTab(float lineHeight);
		void AggregateTab(float lineHeight);
	};
}

#endif