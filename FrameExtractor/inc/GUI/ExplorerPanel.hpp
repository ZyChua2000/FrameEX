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
namespace FrameExtractor
{
	class ExplorerPanel : public IPanel
	{
	public:
		ExplorerPanel();
		~ExplorerPanel() override;
		virtual void OnImGuiRender(float dt) override;
		virtual const char* GetName() const override { return "File Explorer"; }
		std::filesystem::path GetRootPath () { return mRootPath; }
		void SetProjectPath(std::filesystem::path path);
		Ref<Texture> GetExplorerFileIcon(std::filesystem::path);
	private:
		std::filesystem::path mCurrentPath;
		std::filesystem::path mRootPath;
		std::filesystem::path mSelectedPath;
		Ref<Texture> mFileIcon;
		Ref<Texture> mFolderIcon;
		std::map<std::filesystem::path, Ref<Texture>> mExplorerFileIcons;
	};

}

#endif
