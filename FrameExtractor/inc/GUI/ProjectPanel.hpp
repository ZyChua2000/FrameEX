/******************************************************************************/
/*!
\file       ProjectPanel.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 13, 2025
\brief      Declares the Project Panel class which has an interface for all the
			videos involved in the project.

 ******************************************************************************/

#ifndef ProjectPanel_HPP
#define ProjectPanel_HPP
 // Standard Library includes
#include <string>
#include <vector>
#include <filesystem>

// Project includes
#include <GUI/IPanel.hpp>
#include <Core/Project.hpp>
namespace FrameExtractor
{
	class ExplorerPanel;
	class ViewportPanel;

	/*!***********************************************************************
		\brief
			Class that represents the Project Panel in the GUI.
			It contains an interface for all the assets involved in the project.
	*************************************************************************/
	class ProjectPanel : public IPanel
	{
	public:
		ProjectPanel(ExplorerPanel* ex, ViewportPanel* vp, Project* project);
		~ProjectPanel() override;
		virtual void OnImGuiRender(float dt) override;
		virtual const char* GetName() const override;
		virtual void OnAttach() override;
		void OnLoad();
	private:
		ExplorerPanel* ExPanel;
		ViewportPanel* VpPanel;
		Project* mProject;
	};
}

#endif