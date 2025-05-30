/******************************************************************************
/*!
\file       ViewportPanel.hpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 10, 2024
\brief      Declares the Viewport Panel class.

 /******************************************************************************/

#ifndef ViewportPanel_HPP
#define ViewportPanel_HPP
#include <string>
#include <imgui.h>
#include <GUI/IPanel.hpp>
#include <Graphics/Video.hpp>
#include <Graphics/BoundingBox.hpp>
namespace FrameExtractor
{

	class ViewportPanel : public IPanel
	{
	public:
		ViewportPanel(const std::string& name, ImVec2& size, ImVec2& pos);
		~ViewportPanel() override;
		virtual void OnImGuiRender(float dt) override;
		virtual const char* GetName() const override;
		void SpeedUp();
		void SlowDown();
		inline void SkipToFrame(uint32_t frameNumber) { mFrameNumber = frameNumber; }
		inline void Play() { mIsPlaying = true; }
		inline void Pause() { mIsPlaying = false; }
		inline int32_t GetFrameNumber() const { return mFrameNumber; }
		inline float GetSpeedMultiplier() const { return mSpeedMultiplier; }
		void SetVideo(std::filesystem::path path);
		void ClearVideo() { if (mVideo) delete mVideo; mVideo = nullptr; }
		std::filesystem::path GetVideoPath() { 
			if (mVideo) 
				return mVideo->GetPath(); 
			else 
				return {};
		}
	private:
		std::string mName;
		std::map<std::filesystem::path, std::map<int32_t, std::vector<BoundingBox>>> mBBCache;
		Video* mVideo = nullptr;
		ImVec2 mViewportSize = ImVec2(0, 0);
		ImVec2 mViewportPos = ImVec2(0, 0);
		float DTTrack = 0.f;
		float mSpeedMultiplier = 1;
		int32_t wasPlaying = false;
		int32_t mFrameNumber = 0;
		int8_t mSpeedMultiplierIndex = 1;
		bool initialIn = false;
		bool mIsPlaying = true;


	};

}

#endif
