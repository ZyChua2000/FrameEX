/******************************************************************************
/*!
\file       ExplorerPanel.hpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 13, 2024
\brief      Declares the Explorer Panel class which creates the interface for a
			exploring files

 /******************************************************************************/

#ifndef ExplorerPanel_HPP
#define ExplorerPanel_HPP
#include <filesystem>
#include <GUI/IPanel.hpp>
#include <Core/Core.hpp>
#include <Graphics/Texture.hpp>
#include <Core/Project.hpp>
namespace FrameExtractor
{
	class ExplorerPanel : public IPanel
	{
	public:
		ExplorerPanel(Project* project);
		~ExplorerPanel() override;
		virtual void OnImGuiRender(float dt) override;
		virtual const char* GetName() const override { return "File Explorer"; }
		Ref<Texture> GetExplorerFileIcon(std::filesystem::path);
		inline void SetCurrentPath(std::filesystem::path path) { mCurrentPath = path; }
	private:
		std::filesystem::path mCurrentPath;
		std::filesystem::path mSelectedPath;
		Ref<Texture> mFileIcon;
		Ref<Texture> mFolderIcon;
		std::map<std::filesystem::path, Ref<Texture>> mExplorerFileIcons;
		Project* mProject;
	};

}

#endif
