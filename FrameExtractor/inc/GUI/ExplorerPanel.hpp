/******************************************************************************/
/*!
\file       ExplorerPanel.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 13, 2025
\brief      Declares the Explorer Panel class which creates the interface for a
			exploring files

 ******************************************************************************/

#ifndef ExplorerPanel_HPP
#define ExplorerPanel_HPP
 // Standard Library includes
#include <filesystem>

// Project includes
#include <GUI/IPanel.hpp>
#include <Core/Core.hpp>
#include <Core/Project.hpp>
#include <Graphics/Texture.hpp>
namespace FrameExtractor
{
	/*!***********************************************************************
		\brief
			Class that represents the file explorer panel in the GUI.
	*************************************************************************/
	class ExplorerPanel : public IPanel
	{
	public:

		/*!***********************************************************************
			\brief
				Structure that holds the cache for an item
		*************************************************************************/
		struct Cache
		{
			Ref<Texture> mTexture;
			uint32_t mMaxFrames;
		};

		/*!***********************************************************************
			\brief
				Default constructor for ExplorerPanel class.
			\param[in] project
				The project to associate with this explorer panel.
		*************************************************************************/
		ExplorerPanel(Project* project);

		/*!***********************************************************************
			\brief
				Default destructor for ExplorerPanel class.
		*************************************************************************/
		~ExplorerPanel() override;

		/*!***********************************************************************
			\brief
				ImGui render function for the explorer panel.
			\param[in] dt
				The delta time since the last frame.
		*************************************************************************/
		virtual void OnImGuiRender(float dt) override;

		/*!***********************************************************************
			\brief
				Get the name of the panel.
			\return
				The name of the panel.
		*************************************************************************/
		virtual const char* GetName() const override { return "File Explorer"; }

		/*!***********************************************************************
			\brief
				Get the cache for a specific path.
			\param[in] path
				The path to get the cache for.
			\return
				The cache associated with the path.
		*************************************************************************/
		Cache GetCache(std::filesystem::path);

		/*!***********************************************************************
			\brief
				Sets the selected path in the explorer panel.
			\param[in] path
				The path to set as selected.
		*************************************************************************/
		inline void SetCurrentPath(std::filesystem::path path) { mCurrentPath = path; }
	private:

		/*!***********************************************************************
			\brief
				Draws the directory structure in the explorer panel.
		**************************************************************************/
		void DrawDirectory();

		/*!***********************************************************************
			\brief
				Draws the assets in the explorer panel.
		**************************************************************************/
		void DrawAssets();

		std::filesystem::path mCurrentPath;				// The current path being explored
		std::filesystem::path mSelectedPath;			// The currently selected path in the explorer panel
		std::map<std::filesystem::path, Cache> mCache;	// Cache for storing textures and frame counts for paths
		Project* mProject;								// Pointer to the project associated with this explorer panel
		bool mIsUsingDirectory = true;					// Flag to indicate if the panel is in directory mode or asset mode
	};
}

#endif