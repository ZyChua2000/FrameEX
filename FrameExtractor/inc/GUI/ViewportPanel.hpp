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
	enum KeyIcons
	{
		CTRL_ICON,
		SHIFT_ICON,
		RIGHTARROW_ICON,
		DOWNARROW_ICON,
		LEFTARROW_ICON,
		UPARROW_ICON,
		SPACE_ICON,
		PLUS_LOGO
	};
	enum Icons
	{
		PLAY_ICON,
		STOP_ICON,
		FORWARD_ICON,
		BACKWARD_ICON,
		SPEED_UP_ICON,
		SLOW_DOWN_ICON,
		SKIP_TO_END_ICON,
		SKIP_TO_START_ICON,

		TOTAL_ICONS
	};
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
		std::filesystem::path GetVideoPath() { return mVideo->GetPath(); }
	private:
		std::string mName;
		std::map<Icons, Ref<Texture>> mIcons;
		std::map<KeyIcons, Ref<Texture>> mKeyIcons;
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
