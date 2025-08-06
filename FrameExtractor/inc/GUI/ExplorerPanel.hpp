/******************************************************************************
/*!
\file       ExplorerPanel.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 13, 2025
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

		struct Cache
		{
			Ref<Texture> mTexture;
			uint32_t mMaxFrames;
		};

		ExplorerPanel(Project* project);
		~ExplorerPanel() override;
		virtual void OnImGuiRender(float dt) override;
		virtual const char* GetName() const override { return "File Explorer"; }
		Cache GetCache(std::filesystem::path);
		inline void SetCurrentPath(std::filesystem::path path) { mCurrentPath = path; }
	private:
		void DrawDirectory();
		void DrawAssets();

		std::filesystem::path mCurrentPath;
		std::filesystem::path mSelectedPath;
		std::map<std::filesystem::path, Cache> mCache;
		Project* mProject;
		bool mIsUsingDirectory = true;
	};
}

#endif