/******************************************************************************
/*!
\file       GraphicsContext.cpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 10, 2024
\brief      Defines the Graphics Context class which handles the context on which
			the graphics API will run on.

 /******************************************************************************/
#include <FrameExtractorPCH.hpp>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <Core/LoggerManager.hpp>
#include <Graphics/GraphicsContext.hpp>
extern "C"
{
#include <libavutil/avutil.h>
}

namespace FrameExtractor
{
	GraphicsContext::GraphicsContext(GLFWwindow* windowHandle) : mWindowHandle(windowHandle)
	{
		if (!mWindowHandle)
		{
			FRAMEEX_CORE_CRITICAL("Failed to create GLFW window!");
			glfwTerminate();
			return;
		}
	}

	void GraphicsContext::Init()
	{
		glfwMakeContextCurrent(mWindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		if (status == 0)
		{
			FRAMEEX_CORE_CRITICAL("Failed to initialize GLAD!");
			glfwTerminate();
			return;
		}

		FRAMEEX_CORE_INFO("OpenGL Info:");
		FRAMEEX_CORE_INFO("  Vendor: {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
		FRAMEEX_CORE_INFO("  Renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
		FRAMEEX_CORE_INFO("  Version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
	}
	void GraphicsContext::SwapBuffers()
	{
		glfwSwapBuffers(mWindowHandle);
	}
}

