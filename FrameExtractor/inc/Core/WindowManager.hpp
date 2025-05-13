/******************************************************************************
/*!
\file       WindowManager.hpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 10, 2024
\brief      Declares the Window Manager class which handles the application window

 /******************************************************************************/

#ifndef WindowManager_HPP
#define WindowManager_HPP
#include <Graphics/GraphicsContext.hpp>

namespace FrameExtractor
{

	struct WindowProperties
	{
		std::string mTitle = "Window";
		uint32_t mWidth = 1920;
		uint32_t mHeight = 1080;
	};

	class WindowManager
	{
	public:
		WindowManager(const WindowProperties& properties = WindowProperties());
		~WindowManager();


		void Update();

		inline uint32_t GetWidth() const { return mData.mWidth; }
		inline uint32_t GetHeight() const { return mData.mWidth; }
		inline uint32_t GetWindowPosX() const { return mData.mPosX; }
		inline uint32_t GetWindowPosY() const { return mData.mPosY; }
		inline std::pair<uint32_t,uint32_t> GetWindowPos() const { return { mData.mPosX, mData.mPosY }; }
		inline void* GetNativeWindow() const { return mWindow; }

	private:
		void Init(const WindowProperties& properties);
		void Shutdown();

	private:
		GraphicsContext* mContext;
		void* mWindow;

		struct WindowData
		{
			const char* mTitle;
			uint32_t mWidth;
			uint32_t mHeight;
			uint32_t mPosX;
			uint32_t mPosY;
		};

		WindowData mData;
		
	};
}

#endif
