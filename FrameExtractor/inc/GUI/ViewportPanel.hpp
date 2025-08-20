/******************************************************************************/
/*!
\file       ViewportPanel.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 10, 2025
\brief      Declares the Viewport Panel class.

 ******************************************************************************/

#ifndef ViewportPanel_HPP
#define ViewportPanel_HPP

 // Standard Library includes
#include <string>

// Third-party includes
#include <imgui.h>

// Project includes
#include <GUI/IPanel.hpp>
#include <Graphics/Video.hpp>
#include <Graphics/BoundingBox.hpp>
namespace FrameExtractor
{
	/*!***********************************************************************
		\brief
			Class that represents the Viewport Panel in the GUI.
			It contains an interface for viewing and controlling video playback.
	*************************************************************************/
	class ViewportPanel : public IPanel
	{
	public:

		/*!***********************************************************************
			\brief
				Constructor that initializes the viewport panel with a name, size, and position.
			\param[in] name
				The name of the viewport panel.
			\param[in] size
				The size of the viewport panel.
			\param[in] pos
				The position of the viewport panel.
		*************************************************************************/
		ViewportPanel(const std::string& name, ImVec2& size, ImVec2& pos);

		/*!***********************************************************************
			\brief
				Default destructor for ViewportPanel class.
		*************************************************************************/
		~ViewportPanel() override;

		/*!***********************************************************************
			\brief
				Renders the ImGui interface for the viewport panel.
			\param[in] dt
				The delta time since the last frame.
		*************************************************************************/
		virtual void OnImGuiRender(float dt) override;

		/*!***********************************************************************
			\brief
				Returns the name of the viewport panel.
			\return
				The name of the viewport panel.
		*************************************************************************/
		virtual const char* GetName() const override;

		/*!***********************************************************************
			\brief
				Speeds up the video playback.
		*************************************************************************/
		void SpeedUp();

		/*!***********************************************************************
			\brief
				Slows down the video playback.
		*************************************************************************/
		void SlowDown();

		/*!***********************************************************************
			\brief
				Skips to a specific frame in the video.
			\param [in] frameNumber
				The frame number to skip to.
		*************************************************************************/
		inline void SkipToFrame(uint32_t frameNumber) { mFrameNumber = frameNumber; }

		/*!***********************************************************************
			\brief
				Plays the video.
		*************************************************************************/
		inline void Play() { mIsPlaying = true; }

		/*!***********************************************************************
			\brief
				Pauses the video playback.
		*************************************************************************/
		inline void Pause() { mIsPlaying = false; }


		/*!***********************************************************************
			\brief
				Get the current frame number of the video.
			\return
				The current frame number.
		*************************************************************************/
		inline int32_t GetFrameNumber() const { return mFrameNumber; }

		/*!***********************************************************************
			\brief
				Sets the speed multiplier for video playback.
			\param [in] speedMultiplier
				The speed multiplier to set.
		*************************************************************************/
		inline float GetSpeedMultiplier() const { return mSpeedMultiplier; }

		/*!***********************************************************************
			\brief
				Sets the video to be displayed in the viewport.
			\param[in] handle
				The handle of the video asset to set.
		*************************************************************************/
		void SetVideo(AssetHandle handle);

		/*!***********************************************************************
			\brief
				Remove the current video from the viewport.
		*************************************************************************/
		void ClearVideo() { mVideo = nullptr; }

		/*!***********************************************************************
			\brief
				Gets the path of the currently displayed video.
			\return
				The path of the video if it exists, otherwise an empty path.
		*************************************************************************/
		std::filesystem::path GetVideoPath()
		{
			if (mVideo)
				return mVideo->GetPath();
			else
				return {};
		}
	private:
		std::string mName;	// Name of the viewport panel
		std::map<std::filesystem::path, std::map<int32_t, std::vector<BoundingBox>>> mBBCache; // Cache for bounding boxes per video and frame
		Ref<Video> mVideo = nullptr;			// Pointer to the video being displayed in the viewport
		ImVec2 mViewportSize = ImVec2(0, 0);	// Size of the viewport panel
		ImVec2 mViewportPos = ImVec2(0, 0);		// Position of the viewport panel
		float DTTrack = 0.f;					// Time tracking for video playback
		float mSpeedMultiplier = 1;				// Speed multiplier for video playback
		int32_t wasPlaying = false;				// Flag to check if the video was playing before interaction
		int32_t mFrameNumber = 0;				// Current frame number of the video
		int8_t mSpeedMultiplierIndex = 1;		// Index for speed multiplier, used to cycle through predefined speeds
		bool initialIn = false;					// Flag to check if the initial interaction has occurred
		bool mIsPlaying = true;					// Flag to indicate if the video is currently playing
	};
}

#endif