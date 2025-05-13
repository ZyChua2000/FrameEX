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
		
	private:
		std::string mName;
		ImVec2 mViewportSize = ImVec2(0, 0);
		ImVec2 mViewportPos = ImVec2(0, 0);
		int32_t mFrameNumber = 0;
		float mSpeedMultiplier = 1;
		int8_t mSpeedMultiplierIndex = 1;
		bool mIsPlaying = true;
		Video* mVideo = nullptr;
		std::map<Icons, Ref<Texture>> mIcons;

		std::map<std::filesystem::path, std::map<int32_t, std::vector<BoundingBox>>> mBBCache;
	};

}

#endif
