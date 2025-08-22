/******************************************************************************/
/*!
\file       WindowManager.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 10, 2025
\brief      Declares the Window Manager class which handles the application window

 ******************************************************************************/

#ifndef WindowManager_HPP
#define WindowManager_HPP
 // Project includes
#include <Graphics/GraphicsContext.hpp>
#include <Core/Core.hpp>
namespace FrameExtractor
{
	/*!***********************************************************************
		\brief
			Structure that holds properties for the window.
	*************************************************************************/
	struct WindowProperties
	{
		std::string mTitle = "Window";
		uint32_t mWidth = 1920;
		uint32_t mHeight = 1080;
	};

	/*!***********************************************************************
		\brief
			Class that manages the application window.
	*************************************************************************/
	class WindowManager
	{
	public:
		/*!***********************************************************************
			\brief
				Default constructor that initializes the window manager with default properties.
			\param[in] properties
				The properties to initialize the window with.
		*************************************************************************/
		WindowManager(const WindowProperties& properties = WindowProperties());

		/*!***********************************************************************
			\brief
				Destructor that cleans up the window manager.
		*************************************************************************/
		~WindowManager();

		/*!***********************************************************************
			\brief
				Update function that processes events and updates the window state.
		*************************************************************************/
		void Update();

		/*!***********************************************************************
			\brief
				Get the width of the window.
			\return
				The width of the window.
		*************************************************************************/
		inline uint32_t GetWidth() const { return mData.mWidth; }

		/*!***********************************************************************
			\brief
				Get the height of the window.
			\return
				The height of the window.
		*************************************************************************/
		inline uint32_t GetHeight() const { return mData.mWidth; }

		/*!***********************************************************************
			\brief
				X Position of the window.
			\return
				The X position of the window.
		*************************************************************************/
		inline uint32_t GetWindowPosX() const { return mData.mPosX; }

		/*!***********************************************************************
			\brief
				Y Position of the window.
			\return
				The Y position of the window.
		*************************************************************************/
		inline uint32_t GetWindowPosY() const { return mData.mPosY; }

		/*!***********************************************************************
			\brief
				Get the position of the window.
			\return
				The position of the window as a pair of X and Y coordinates.
		*************************************************************************/
		inline std::pair<uint32_t, uint32_t> GetWindowPos() const { return { mData.mPosX, mData.mPosY }; }

		/*!***********************************************************************
			\brief
				Get the underlying pointer handle to the window.
			\return
				The pointer to the native window handle.
		*************************************************************************/
		inline void* GetNativeWindow() const { return mWindow; }

	private:
		/*!***********************************************************************
			\brief
				Initializes the window with the specified properties.
			\param[in] properties
				The properties to initialize the window with.
		*************************************************************************/
		void Init(const WindowProperties& properties);

		/*!***********************************************************************
			\brief
				Shuts down the window manager and cleans up resources.
		*************************************************************************/
		void Shutdown();

	private:
		Scope<GraphicsContext> mContext;	//<- Graphics context for rendering
		void* mWindow;				//<- Pointer to the native window handle (GLFWwindow* for GLFW)

		/*!***********************************************************************
			\brief
				Structure that holds data related to the window.
		*************************************************************************/
		struct WindowData
		{
			const char* mTitle;
			uint32_t mWidth;
			uint32_t mHeight;
			uint32_t mPosX;
			uint32_t mPosY;
		};

		WindowData mData; //<- Data related to the window properties
	};
}

#endif