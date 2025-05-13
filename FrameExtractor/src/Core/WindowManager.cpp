/******************************************************************************
/*!
\file       WindowManager.hpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 10, 2024
\brief      Defines the Window Manager class which handles the application window

 /******************************************************************************/

#include <FrameExtractorPCH.hpp>
#include <GLFW/glfw3.h>

#include <Core/LoggerManager.hpp>
#include <Core/WindowManager.hpp>
#include <Core/ApplicationManager.hpp>
namespace FrameExtractor
{
	static bool sGLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		FRAMEEX_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}


	WindowManager::WindowManager(const WindowProperties& properties)
	{
		Init(properties);
	}
	WindowManager::~WindowManager()
	{
		Shutdown();
	}
	void WindowManager::Update()
	{
		glfwPollEvents();
		mContext->SwapBuffers();
	}
	void WindowManager::Init(const WindowProperties& properties)
	{
		mData.mWidth = properties.mWidth;
		mData.mHeight = properties.mHeight;
		mData.mTitle = properties.mTitle.c_str();
		glfwSwapInterval(1);

		if (!sGLFWInitialized)
		{
			int success = glfwInit();
			FRAMEEX_CORE_ASSERT(success, "Failed to initialize GLFW!");
			sGLFWInitialized = true;
		}

		{
			//lock the version of opengl to 4.1
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

			mWindow = (void*)(glfwCreateWindow(mData.mWidth, mData.mHeight, mData.mTitle, nullptr, nullptr));
			mContext = new GraphicsContext((GLFWwindow*)mWindow);
			mContext->Init();
		}

		glfwSetWindowUserPointer((GLFWwindow*)mWindow, &mData);

		glfwSetErrorCallback(GLFWErrorCallback);

		glfwSetWindowCloseCallback((GLFWwindow*)mWindow, [](GLFWwindow* window)
		{
			ApplicationManager::GetInstance()->Quit();
		});

		glfwSetWindowPosCallback((GLFWwindow*)mWindow, [](GLFWwindow* window, int x, int y)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.mPosX = x;
			data.mPosY = y;
		});

		glfwSetWindowSizeCallback((GLFWwindow*)mWindow, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.mWidth = width;
			data.mHeight = height;
		});
	}
	void WindowManager::Shutdown()
	{
		glfwDestroyWindow((GLFWwindow*)mWindow);
		delete mContext;
	}
}

