/******************************************************************************
/*!
\file       ProjectPanel.hpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 13, 2024
\brief      Declares the Project Panel class which has an interface for all the
			videos involved in the project.

 /******************************************************************************/

#ifndef ProjectPanel_HPP
#define ProjectPanel_HPP
#include <string>
#include <vector>
#include <filesystem>
#include <GUI/IPanel.hpp>
namespace FrameExtractor
{
	class ExplorerPanel;
	class ViewportPanel;
	class ProjectPanel : public IPanel
	{
	public:
		ProjectPanel(ExplorerPanel* ex, ViewportPanel* vp);
		~ProjectPanel() override;
		virtual void OnImGuiRender(float dt) override;
		virtual const char* GetName() const override;
		virtual void OnAttach() override;
	private:
		std::vector<std::filesystem::path> videosInProject;
		ExplorerPanel* ExPanel;
		ViewportPanel* VpPanel;

	};

}

#endif
